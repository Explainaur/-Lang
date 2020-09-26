//
// Created by dyf on 2020/9/26.
//

#ifndef _LANG_TARGET_H
#define _LANG_TARGET_H

#include "mid/IRGenerator.h"
#include "front/define/AST.h"

#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

int TargetGen();

#endif //_LANG_TARGET_H
