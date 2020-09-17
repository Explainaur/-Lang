//
// Created by dyf on 2020/9/15.
//

#include <iostream>
#include <front/define/AST.h>


#include "IRGenerator.h"
#include "front/define/color.h"

using namespace front;

static llvm::LLVMContext TheContext;
static llvm::IRBuilder<> Builder(TheContext);
static std::unique_ptr<llvm::Module> TheModule;
static std::map<std::string, llvm::Value *> NamedValues;
static std::unique_ptr<llvm::legacy::FunctionPassManager> TheFPM;
static std::unique_ptr<llvm::orc::KaleidoscopeJIT> TheJIT;


void ModuleInit() {
    TheModule = std::make_unique<llvm::Module>("dyf", TheContext);
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
    llvm::Function *CalleeF = TheModule->getFunction(symbol);
    if (!CalleeF)
        return LogErrorV("Unknown function referenced");

    // If argument mismatch error.
    if (CalleeF->arg_size() != args.size())
        return LogErrorV("Incorrect # arguments passed");

    std::vector<llvm::Value *> ArgsV;
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
        ArgsV.push_back(args[i]->codegen());
        if (!ArgsV.back())
            return nullptr;
    }

    return Builder.CreateCall(CalleeF, ArgsV, "calltmp");
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
    // First, check for an existing function from a previous 'extern' declaration.
    llvm::Function *TheFunction = TheModule->getFunction(protoType->getName());

    if (!TheFunction)
        TheFunction = protoType->codegen();

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
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    ModuleInit();
    TheJIT = std::make_unique<llvm::orc::KaleidoscopeJIT>();
    InitializeModuleAndPassManager();

}