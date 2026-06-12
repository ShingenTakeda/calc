#pragma once
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

struct Expr;
struct AST;
struct Factor;
struct BinaryOp;
struct WithDecl;

struct ASTVisitor
{
	virtual void visit(AST &) {};
	virtual void visit(Expr &) {};
	virtual void visit(Factor &) = 0;
	virtual void visit(BinaryOp &) = 0;
	virtual void visit(WithDecl &) = 0;
};

struct AST
{
	virtual ~AST() {}
	virtual void accept(ASTVisitor &v) = 0;
};

struct Expr : AST
{
	Expr() {}
};

struct Factor : Expr
{
	enum ValueKind
	{
		IDENT,
		NUMBER
	};

	ValueKind kind;
	llvm::StringRef val;

	Factor(ValueKind k, llvm::StringRef v) : kind(k), val(v) {}
	ValueKind get_kind() {return kind;}
	llvm::StringRef get_val() {return val;}

	virtual void accept(ASTVisitor &v) override
	{
		v.visit(*this);
	}
};

struct BinaryOp : Expr
{
	enum Operator
	{
		PLUS,
		MINUS,
		MUL,
		DIV
	};

	Expr *left;
	Expr *right;
	Operator op;

	BinaryOp(Operator o, Expr *l, Expr *r) : op(o), left(l), right(r) {}

	Expr *get_left() {return left;}
	Expr *get_right() {return right;}
	Operator get_operator() {return op;}
	virtual void accept(ASTVisitor &v) override
	{
		v.visit(*this);
	}
};

struct WithDecl : AST
{
	using VarsVector = llvm::SmallVector<llvm::StringRef, 8>;
	
	VarsVector vars;
	Expr *e;

	WithDecl(llvm::SmallVector<llvm::StringRef, 8> v, Expr *e) : vars(v), e(e) {}
	VarsVector::const_iterator begin() {return vars.begin();}
	VarsVector::const_iterator end() {return vars.end();}
	Expr *get_expr() {return e;}

	virtual void accept(ASTVisitor &v) override
	{
		v.visit(*this);
	}
};
