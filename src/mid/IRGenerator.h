//
// Created by dyf on 2020/9/15.
//

#ifndef _LANG_IRGENERATOR_H
#define _LANG_IRGENERATOR_H

#include "front/define/AST.h"

llvm::Value *LogErrorV(const std::string &info);

void DumpCode();

void ModuleInit();


#endif //_LANG_IRGENERATOR_H
