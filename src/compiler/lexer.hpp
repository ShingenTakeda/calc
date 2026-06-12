#pragma once
#include <cstdint>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/MemoryBuffer.h>

struct Lexer;

struct Token
{
	friend struct Lexer;

	enum TokenKind : uint8_t
	{
		EOI,
		UNKNOWN,
		IDENT,
		NUMBER,
		COMMA,
		COLON,
		PLUS,
		MINUS,
		STAR,
		SLASH,
		L_PAREN,
		R_PAREN,
		KW_WITH,
	};

	TokenKind kind;
	llvm::StringRef text;

	TokenKind get_kind() const {return kind;};
	llvm::StringRef get_text() const {return text;}

	bool is(TokenKind K) const {return kind == K;}

	bool is_one_of(TokenKind k1, TokenKind k2) const {return is(k1) || is(k2);}

	template<typename ...Ts>
	bool is_one_of(TokenKind k1, TokenKind k2, Ts ...ks) const
	{
		return is(k1) || is_one_of(k2, ks...);
	}
};

struct Lexer
{
	const char *buffer_start;
	const char *buffer_ptr;

	Lexer(const llvm::StringRef &buffer)
	{
		buffer_start = buffer.begin();
		buffer_ptr = buffer_start;
	}

	void next(Token &token);
	void form_token(Token &token, const char *token_end, Token::TokenKind kind);
};
