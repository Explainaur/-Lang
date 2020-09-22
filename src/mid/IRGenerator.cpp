//
// Created by dyf on 2020/9/15.
//

#include <iostream>
#include <front/define/AST.h>


#include "IRGenerator.h"
#include "front/define/color.h"
#include "mid/JITSupport.h"

using namespace front;

llvm::LLVMContext TheContext;
llvm::IRBuilder<> Builder(TheContext);
std::unique_ptr<llvm::Module> TheModule;
std::map<std::string, llvm::Value *> NamedValues;
std::unique_ptr<llvm::legacy::FunctionPassManager> TheFPM;
std::unique_ptr<llvm::orc::KaleidoscopeJIT> TheJIT;
std::map<std::string, std::shared_ptr<ProtoTypeAST>> FunctionProtos;


void ModuleInit() {
    TheModule = std::make_unique<llvm::Module>("dyf", TheContext);
}

void JITInit() {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
}

void DumpCode() {
    TheModule->print(llvm::errs(), nullptr);
}

void LogError(const std::string &info) {
    auto green = Color::Modifier(Color::Code::FG_GREEN);
    auto def = Color::Modifier(Color::Code::FG_DEFAULT);
    std::cerr << green << "info: " << def
              << info << std::endl;
}

llvm::Value *LogErrorV(const std::string &info) {
    LogError(info);
    return nullptr;
}


llvm::Value *NumberAST::codegen() {
    return llvm::ConstantFP::get(TheContext, llvm::APFloat(value));
}


llvm::Value *VariableAST::codegen() {
    // Look this variable up in the function.
    llvm::Value *V = NamedValues[value];
    if (!V) {
        LogErrorV("Unknown variable name");
    }
    return V;
}

llvm::Value *IfExprAST::codegen() {
    llvm::Value *condV = condition->codegen();
    if (!condV) return nullptr;

    // Convert condition to a bool by comparing non-equal to 0.0.
    condV = Builder.CreateFCmpONE(
            condV, llvm::ConstantFP::get(TheContext, llvm::APFloat(0.0)), "ifcond");

    llvm::Function *theFunction = Builder.GetInsertBlock()->getParent();

    // Create blocks for the then and else cases.  Insert the 'then' block at the
    // end of the function.
    llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(TheContext, "then", theFunction);
    llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(TheContext, "else");
    llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(TheContext, "ifcont");

    Builder.CreateCondBr(condV, thenBB, elseBB);

    // Emit then value.
    Builder.SetInsertPoint(thenBB);

    llvm::Value *thenV = then_->codegen();
    if (!thenV) return nullptr;

    Builder.CreateBr(mergeBB);

    // Codegen of 'Then' can change the current block, update ThenBB for the PHI.
    thenBB = Builder.GetInsertBlock();

    // Emit else block.
    theFunction->getBasicBlockList().push_back(elseBB);
    Builder.SetInsertPoint(elseBB);

    llvm::Value *elseV = else_->codegen();
    if (!elseV)
        return nullptr;

    Builder.CreateBr(mergeBB);

    // codegen of 'Else' can change the current block, update ElseBB for the PHI.
    elseBB = Builder.GetInsertBlock();

    // Emit merge block.
    theFunction->getBasicBlockList().push_back(mergeBB);
    Builder.SetInsertPoint(mergeBB);
    llvm::PHINode *phiNode = Builder.CreatePHI(llvm::Type::getDoubleTy(TheContext), 2, "iftmp");

    phiNode->addIncoming(thenV, thenBB);
    phiNode->addIncoming(elseV, elseBB);
    return phiNode;
}

llvm::Value *ForExprAST::codegen() {
    llvm::Value *startValue = start_->codegen();
    if (!startValue) return nullptr;

    // Make the new basic block for the loop header, inserting after current
    // block.
    llvm::Function *theFunction = Builder.GetInsertBlock()->getParent();
    llvm::BasicBlock *preHeaderBB = Builder.GetInsertBlock();
    llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(
            TheContext, "loop", theFunction);

    // Insert an explicit fall through from the current block to the LoopBB.
    Builder.CreateBr(loopBB);

    // Start insertion in LoopBB.
    Builder.SetInsertPoint(loopBB);

    // Start the PHI node with an entry for Start.
    llvm::PHINode *phiNode = Builder.CreatePHI(
            llvm::Type::getDoubleTy(TheContext), 2, valueName);
    phiNode->addIncoming(startValue, preHeaderBB);

    // Within the loop, the variable is defined equal to the PHI node.  If it
    // shadows an existing variable, we have to restore it, so save it now.
    llvm::Value *oldVal = NamedValues[valueName];
    NamedValues[valueName] = phiNode;

    // Emit the body of the loop.  This, like any other expr, can change the
    // current BB.  Note that we ignore the value computed by the body, but don't
    // allow an error.
    //    loopBB = Builder.GetInsertBlock();
    //    Builder.SetInsertPoint(loopBB);
    llvm::Value *bodyValue = body->codegen();
    if (!bodyValue) return nullptr;

    // Emit the step value.
    llvm::Value *stepValue = nullptr;
    if (step) {
        stepValue = step->codegen();
        if (!stepValue)
            return nullptr;
    } else {
        // If not specified, use 1.0.
        stepValue = llvm::ConstantFP::get(TheContext, llvm::APFloat(1.0));
    }

    llvm::Value *nextValue = Builder.CreateFAdd(phiNode, stepValue, "nextValue");

    llvm::Value *endCond = end_->codegen();
    if (!endCond) return nullptr;

    endCond = Builder.CreateFCmpONE(
            endCond, llvm::ConstantFP::get(TheContext, llvm::APFloat(0.0)), "loopcond");

    // Create the "after loop" block and insert it.
    llvm::BasicBlock *loopEndBB = Builder.GetInsertBlock();
    llvm::BasicBlock *afterBB =
            llvm::BasicBlock::Create(TheContext, "afterloop", theFunction);

    // Insert the conditional branch into the end of LoopEndBB.
    Builder.CreateCondBr(endCond, loopBB, afterBB);

    // Any new code will be inserted in AfterBB.
    Builder.SetInsertPoint(afterBB);

    // Add a new entry to the PHI node for the backedge.
    phiNode->addIncoming(nextValue, loopEndBB);

    // Restore the unshadowed variable
    if (oldVal) {
        NamedValues[valueName] = oldVal;
    } else {
        NamedValues.erase(valueName);
    }

    return llvm::Constant::getNullValue(llvm::Type::getDoubleTy(TheContext));
}

llvm::Value *UnaryAST::codegen() {
    llvm::Value* operandV = operand->codegen();
    if (!operandV) return nullptr;

    llvm::Function *op = getFunction(std::string("unary") + this->oper);
    auto unarySSA = Builder.CreateCall(op, operandV, "unop");
    return unarySSA;
}

llvm::Value *BinaryAST::codegen() {
    llvm::Value *L = LHS->codegen();
    llvm::Value *R = RHS->codegen();

    if (!L || !R) return nullptr;

    if (op == "+") {
        return Builder.CreateFAdd(L, R, "addtmp");
    } else if (op == "-") {
        return Builder.CreateFSub(L, R, "subtmp");
    } else if (op == "*") {
        return Builder.CreateFMul(L, R, "multmp");
    } else if (op == "/") {
        return Builder.CreateFDiv(L, R, "divtmp");
    } else if (op == "<") {
        L = Builder.CreateFCmpULT(L, R, "cmptmp");
        // Convert bool 0/1 to double 0.0 or 1.0
        return Builder.CreateUIToFP(L, llvm::Type::getDoubleTy(TheContext),
                                    "booltmp");
    } else if (op == ">") {
        L = Builder.CreateFCmpUGT(L, R, "cmptmp");
        return Builder.CreateUIToFP(L, llvm::Type::getDoubleTy(TheContext),
                                    "booltmp");
    } else {
        return LogErrorV("invalid binary operator");
    }

}


llvm::Value *CallAST::codegen() {
    // Look up the name in the global module table.
    llvm::Function *calleeF = getFunction(symbol);

    if (!calleeF)
        return LogErrorV("Unknown function referenced");

    // If argument mismatch error.
    if (calleeF->arg_size() != args.size())
        return LogErrorV("Incorrect # arguments passed");

    std::vector<llvm::Value *> ArgsV;
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
        ArgsV.push_back(args[i]->codegen());
        if (!ArgsV.back())
            return nullptr;
    }

    return Builder.CreateCall(calleeF, ArgsV, "calltmp");
}

llvm::Function *ProtoTypeAST::codegen() {
    // Make the function type:  double(double,double) etc.
    std::vector<llvm::Type *> Doubles(
            args.size(), llvm::Type::getDoubleTy(TheContext));
    llvm::FunctionType *ft =
            llvm::FunctionType::get(
                    llvm::Type::getDoubleTy(TheContext), Doubles, false);

    llvm::Function *f =
            llvm::Function::Create(
                    ft, llvm::Function::ExternalLinkage,
                    funcName, TheModule.get());

    // Set names for all arguments.
    unsigned idx = 0;
    for (auto &arg : f->args())
        arg.setName(args[idx++]);

    return f;

}

llvm::Function *FunctionAST::codegen() {
    // Transfer ownership of the prototype to the FunctionProtos map, but keep a
    // reference to it for use below.
    auto &P = *protoType;
    FunctionProtos[protoType->getName()] = protoType;
    llvm::Function *TheFunction = getFunction(P.getName());
    if (!TheFunction)
        return nullptr;

    // Create a new basic block to start insertion into.
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(TheContext, "entry", TheFunction);
    Builder.SetInsertPoint(BB);

    // Record the function arguments in the NamedValues map.
    NamedValues.clear();
    for (auto &Arg : TheFunction->args())
        NamedValues[Arg.getName()] = &Arg;

    if (llvm::Value *RetVal = body->codegen()) {
        // Finish off the function.
        Builder.CreateRet(RetVal);

        // Validate the generated code, checking for consistency.
        verifyFunction(*TheFunction);

        // Optimize the function.
        TheFPM->run(*TheFunction);

        return TheFunction;
    }

    // Error reading body, remove function.
    TheFunction->eraseFromParent();
    return nullptr;
}

void InitializeModuleAndPassManager(void) {
    // Open a new module.
    TheModule = std::make_unique<llvm::Module>("zxy", TheContext);
    // Setup the data layout for the JIT
    TheModule->setDataLayout(TheJIT->getTargetMachine().createDataLayout());

    // Create a new pass manager attached to it.
    TheFPM = std::make_unique<llvm::legacy::FunctionPassManager>(TheModule.get());

    // Do simple "peephole" optimizations and bit-twiddling optzns.
    TheFPM->add(llvm::createInstructionCombiningPass());
    // Reassociate expressions.
    TheFPM->add(llvm::createReassociatePass());
    // Eliminate Common SubExpressions.
    TheFPM->add(llvm::createGVNPass());
    // Simplify the control flow graph (deleting unreachable blocks, etc).
    TheFPM->add(llvm::createCFGSimplificationPass());

    TheFPM->doInitialization();

}

void MidInit() {
    ModuleInit();
    JITInit();
    TheJIT = std::make_unique<llvm::orc::KaleidoscopeJIT>();
    assert(TheJIT != nullptr);

    InitializeModuleAndPassManager();

}