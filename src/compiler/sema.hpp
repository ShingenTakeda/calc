#pragma once
#include "AST.hpp"
#include "lexer.hpp"

struct Sema
{
	bool semantic(AST *tree);
};
