#pragma once
#include "llvm/Support/raw_os_ostream.h"
#include "lexer.hpp"
#include "AST.hpp"

struct Parser
{
	Lexer &lex;
	Token tok;
	bool has_error;

	void error()
	{
		llvm::errs() << "Unexpected: " << tok.get_text() << "\n";
	
		has_error = true;
	}

	void advance() {lex.next(tok);}
	
	bool expect(Token::TokenKind kind)
	{
		if(tok.get_kind() != kind)
		{
			error();
			return true;
		}
		return false;
	}

	bool consume(Token::TokenKind kind)
	{
		if(expect(kind))
		{
			return true;
		}
		advance();
		return false;
	}

	AST *parse_calc();
	Expr *parse_expr();
	Expr *parse_term();
	Expr *parse_factor();

	Parser(Lexer &lex) : lex(lex), has_error(false)
	{
		advance();
	}

	bool has_error_() {return has_error;}
	AST *parse();
};
