#include "lexer.hpp"
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/Compiler.h>

namespace charinfo
{
    LLVM_READNONE inline bool is_whitespace(char c)
    {
        return c == ' ' || c == '\t' || c == '\f' || c == '\r' || c == '\v' || c == '\r' || c == '\n';
    }

    LLVM_READNONE inline bool is_digit(char c)
    {
        return c >= '0' && c <= '9';
    }

    LLVM_READNONE inline bool is_letter(char c)
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }
}

void Lexer::next(Token &token)
{
    while(*buffer_ptr && charinfo::is_whitespace(*buffer_ptr))
    {
        ++buffer_ptr;
    }

    if(!*buffer_ptr)
    {
        token.kind = Token::EOI;
        return;
    }

    if(charinfo::is_letter(*buffer_ptr))
    {
        const char *end = buffer_ptr + 1;

        while(charinfo::is_letter(*end))
        {
            ++end;
        }

        llvm::StringRef name(buffer_ptr, end - buffer_ptr);

        Token::TokenKind kind = 
            name == "with" ? Token::KW_WITH : Token::IDENT;

        form_token(token, end, kind);

        return;
    }
    else if(charinfo::is_digit(*buffer_ptr))
    {
        const char *end = buffer_ptr + 1;
        while(charinfo::is_digit(*end))
        {
            ++end;
        }
        form_token(token, end, Token::NUMBER);
        return;
    }
    else
    {
        switch(*buffer_ptr)
        {
            #define CASE(ch, tok)\
            case ch : form_token(token, buffer_ptr + 1, tok); break;
            CASE('+', Token::PLUS);
            CASE('-', Token::MINUS);
            CASE('*', Token::STAR);
            CASE('/', Token::SLASH);
            CASE('(', Token::Token::L_PAREN);
            CASE(')', Token::Token::R_PAREN);
            CASE(':', Token::Token::COLON);
            CASE(',', Token::Token::COMMA);
            #undef CASE
            default:
                form_token(token, buffer_ptr + 1, Token::Token::UNKNOWN);
        }
        return;
    }
}

void Lexer::form_token(Token &token, const char *token_end, Token::TokenKind kind)
{
    token.kind = kind;
    token.text = llvm::StringRef(buffer_ptr, token_end - buffer_ptr);
    buffer_ptr = token_end;
}
