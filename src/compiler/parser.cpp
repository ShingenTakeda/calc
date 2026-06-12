#include "AST.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/StringRef.h>

AST *Parser::parse()
{
    AST *res = parse_calc();
    expect(Token::EOI);
    return res;
}

AST *Parser::parse_calc()
{
    Expr *e;
    llvm::SmallVector<llvm::StringRef, 8> vars;

    if(tok.is(Token::KW_WITH))
    {
        advance();
        if(expect(Token::IDENT))
        {
            goto _error;
        }

        vars.push_back(tok.get_text());
        advance();
        
        while(tok.is(Token::COMMA))
        {
            advance();
            if(expect(Token::IDENT))
            {
                goto _error;
            }
            vars.push_back(tok.get_text());
            advance();
        }
        
        if(consume(Token::COLON))
        {
            goto _error;
        }
    }

    e = parse_expr();
    if(expect(Token::EOI)) {goto _error;}
    if(vars.empty())
    {
        return e;
    }
    else
    {
        return new WithDecl(vars, e);
    }

    _error:
        while(!tok.is(Token::EOI))
        {
            advance();
        }
        return nullptr;
}

Expr *Parser::parse_expr()
{
    Expr *left = parse_term();
    while (tok.is_one_of(Token::PLUS, Token::MINUS))
    {
        BinaryOp::Operator op =
            tok.is(Token::PLUS) ? BinaryOp::PLUS : BinaryOp::MINUS;
        advance();
        Expr *right = parse_term();
        left = new BinaryOp(op, left, right);
    }

    return left;
}

Expr *Parser::parse_term()
{
    Expr *left = parse_factor();
    while(tok.is_one_of(Token::STAR, Token::SLASH))
    {
        BinaryOp::Operator op = 
            tok.is(Token::STAR) ? BinaryOp::MUL : BinaryOp::DIV;
        advance();
        Expr *right = parse_factor();
        left = new BinaryOp(op, left, right);
    }
    return left;
}

Expr *Parser::parse_factor()
{
    Expr *res = nullptr;
    switch (tok.get_kind()) 
    {
        case Token::NUMBER:
            res = new Factor(Factor::NUMBER, tok.get_text());
            advance();
        break;
        case Token::IDENT:
            res = new Factor(Factor::IDENT, tok.get_text());
            advance();
        break;
        case Token::L_PAREN:
            advance();
            res = parse_expr();
            if(!consume(Token::R_PAREN))
            {
                break;
            }
        default:
            if(!res)
            {
                error();
            }
            
            while(!tok.is_one_of(Token::R_PAREN, Token::STAR, Token::PLUS, Token::MINUS, Token::SLASH, Token::EOI))
            {
                advance();
            }
    }
    
    return res;
}
