cmake_minimum_required(VERSION 3.17)
project(_Lang)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Werror -fdeclspec)


find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(src)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
message(STATUS "Include Directory ${LLVM_INCLUDE_DIRS}")

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(_Lang ${SOURCES})
llvm_map_components_to_libnames(LLVM_LIBS core support executionengine passes interpreter
        orcjit x86asmparser x86disassembler x86codegen target
        aarch64asmparser aarch64codegen aarch64desc aarch64disassembler aarch64info aarch64utils target
        amdgpuasmparser amdgpucodegen amdgpudesc amdgpudisassembler amdgpuinfo amdgpuutils
        armasmparser armcodegen armdesc armdisassembler arminfo armutils asmparser asmprinter
        bitreader bitstreamreader bitwriter  bpfasmparser bpfcodegen bpfdesc bpfdisassembler bpfinfo
        hexagonasmparser hexagoncodegen hexagondesc hexagondisassembler hexagoninfo
        lanaiasmparser lanaicodegen lanaidesc lanaidisassembler lanaiinfo libdriver lineeditor linker lto
        mipsasmparser mipscodegen mipsdesc mipsdisassembler mipsinfo mirparser
        msp430asmparser msp430codegen msp430desc msp430disassembler msp430info
        nvptxcodegen nvptxdesc nvptxinfo powerpcasmparser powerpccodegen powerpcdesc powerpcdisassembler powerpcinfo profiledata
        riscvasmparser riscvcodegen riscvdesc riscvdisassembler riscvinfo riscvutils
        sparcasmparser sparccodegen sparcdesc sparcdisassembler sparcinfo
        systemzasmparser systemzcodegen systemzdesc systemzdisassembler systemzinfo
        webassemblyasmparser webassemblycodegen webassemblydesc webassemblydisassembler webassemblyinfo
        xcorecodegen xcoredesc xcoredisassembler xcoreinfo)
target_link_libraries(_Lang ${LLVM_LIBS})