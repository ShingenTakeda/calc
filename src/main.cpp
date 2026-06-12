#include <iostream>
#include "compiler/codegen.hpp"
#include "compiler/parser.hpp"
#include "compiler/parser.hpp"
#include "compiler/sema.hpp"

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/raw_os_ostream.h"

static llvm::cl::opt<std::string> input(llvm::cl::Positional, llvm::cl::desc("<input expression>"), llvm::cl::init(""));

int main(int argc, char const **argv)
{
    llvm::InitLLVM x(argc, argv);
    llvm::cl::ParseCommandLineOptions(argc, argv, "calc - the expression compiler!\n");
    Lexer lex(input);
    Parser parser(lex);
    AST *tree = parser.parse();

    if(!tree || parser.has_error)
    {
        llvm::errs() << "Syntax errors ocurred!\n";
        return 1;
    }

    Sema semantics;
    if(semantics.semantic(tree))
    {
        llvm::errs() << "Syntax errors ocurred!\n";
        return 1;
    }

    CodeGen code_gen;
    code_gen.compile(tree);

    return 0;
}
