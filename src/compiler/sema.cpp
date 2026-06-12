#include "sema.hpp"
#include "AST.hpp"
#include "llvm/ADT/StringSet.h"
#include <llvm/Support/raw_ostream.h>

namespace
{
    struct DeclCheck : ASTVisitor
    {
        llvm::StringSet<> scope;
        bool _has_error;    

        enum ErrorType
        {
            TWICE,
            NOT
        };

        void error(ErrorType et, llvm::StringRef v)
        {
            llvm::errs() << "Variable " << v << " " << (et == TWICE ? "already" : "not") << " declared\n";
            _has_error = true;
        }

        DeclCheck() : _has_error(false) {}
        bool has_error() {return _has_error;}

        virtual void visit(Factor &node) override
        {
            if(node.get_kind() == Factor::IDENT)
            {
                if(scope.find(node.get_val()) == scope.end())
                {
                    error(NOT, node.get_val());
                }
            }
        }

        virtual void visit(BinaryOp &node) override
        {
            if(node.get_left())
            {
                node.get_left()->accept(*this);
            }
            else
            {
                _has_error = true;
            }

            if(node.get_right())
            {
                node.get_right()->accept(*this);
            }
            else 
            {
                _has_error = true;
            }
        }

        virtual void visit(WithDecl &node) override
        {
            for(auto i = node.begin(), e = node.end(); i != e; i++)
            {
                if(!scope.insert(*i).second)
                {
                    error(TWICE, *i);
                }
            }
            
            if(node.get_expr())
            {
                node.get_expr()->accept(*this);
            }
            else
            {
                _has_error = true;
            }
        }
    };
}

bool Sema::semantic(AST *tree)
{
    if(!tree)
    {
        return false;
    }

    DeclCheck check;
    tree->accept(check);
    return check.has_error();
}
