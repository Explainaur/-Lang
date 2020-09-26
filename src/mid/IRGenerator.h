//
// Created by dyf on 2020/9/15.
//

#ifndef _LANG_IRGENERATOR_H
#define _LANG_IRGENERATOR_H

#include "front/define/AST.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Utils.h"

#include "Kaleidoscope.h"

llvm::Value *LogErrorV(const std::string &info);

void DumpCode();

void ModuleInit();

void InitializeModuleAndPassManager(void);

void MidInit();

extern llvm::LLVMContext TheContext;
extern llvm::IRBuilder<> Builder;
extern std::unique_ptr<llvm::Module> TheModule;
extern std::map<std::string, llvm::AllocaInst *> NamedValues;
extern std::unique_ptr<llvm::legacy::FunctionPassManager> TheFPM;
extern std::unique_ptr<llvm::orc::KaleidoscopeJIT> TheJIT;
extern std::map<std::string, std::shared_ptr<front::ProtoTypeAST>> FunctionProtos;


#endif //_LANG_IRGENERATOR_H
