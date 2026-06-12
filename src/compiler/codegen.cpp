#include "codegen.hpp"
#include "AST.hpp"
#include "llvm/IR/Module.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/IR/IRBuilder.h"
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/GlobalValue.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

namespace
{
    struct ToIRVisitor : ASTVisitor
    {
        Module *m;
        IRBuilder<> builder;
        Type *void_ty;
        Type *int32_ty;
        PointerType *ptr_ty;
        Constant *int32_zero;
        Value *v;
        StringMap<Value *> name_map;

        ToIRVisitor(Module *m) : m(m), builder(m->getContext())
        {
            void_ty = Type::getVoidTy(m->getContext());
            int32_ty = Type::getInt32Ty(m->getContext());
            ptr_ty = PointerType::getUnqual(m->getContext());
            int32_zero = ConstantInt::get(int32_ty, 0, true);
        }

        void run(AST *tree)
        {
            FunctionType *main_ty = FunctionType::get(int32_ty, {int32_ty, ptr_ty}, false);
            Function *main_fn = Function::Create(main_ty, GlobalValue::ExternalLinkage, "main", m);

            BasicBlock *bb = BasicBlock::Create(m->getContext(), "entry", main_fn);
            builder.SetInsertPoint(bb);
            
            tree->accept(*this);

            FunctionType *calc_write_fn_ty = FunctionType::get(void_ty, {int32_ty}, false);
            Function *calc_write_fn = Function::Create(calc_write_fn_ty, GlobalValue::ExternalLinkage, "calc_write", m);
            builder.CreateCall(calc_write_fn_ty, calc_write_fn, {v});

            builder.CreateRet(int32_zero);
        }
        
        virtual void visit(WithDecl &node) override
        {
            FunctionType *read_ty = FunctionType::get(int32_ty, {ptr_ty}, false);
            Function *read_fn = Function::Create(read_ty, GlobalValue::ExternalLinkage, "calc_read", m);
            
            for(auto i = node.begin(), e = node.end(); i != e; i++)
            {
                StringRef var = *i;
                Constant *str_text = ConstantDataArray::getString(m->getContext(), var);
                GlobalVariable *str = new GlobalVariable(*m, str_text->getType(), true, GlobalValue::PrivateLinkage, str_text, Twine(var).concat(".str"));
                CallInst *call = builder.CreateCall(read_ty, read_fn, {str});
                name_map[var] = call;
            }

            node.get_expr()->accept(*this);
        }

        virtual void visit(Factor &node) override
        {
            if(node.get_kind() == Factor::IDENT)
            {
                v = name_map[node.get_val()];
            }
            else
            {
                int int_val;
                node.get_val().getAsInteger(10, int_val);
                v = ConstantInt::get(int32_ty, int_val, true);
            }
        }

        virtual void visit(BinaryOp &node) override
        {
            node.get_left()->accept(*this);
            Value *l = v;
            
            node.get_right()->accept(*this);
            Value *r = v;

            switch(node.get_operator())
            {
                case BinaryOp::PLUS:
                    v = builder.CreateNSWAdd(l, r);
                break;
                case BinaryOp::MINUS:
                    v = builder.CreateNSWSub(l, r);
                break;
                case BinaryOp::MUL:
                    v = builder.CreateNSWMul(l, r);
                break;
                case BinaryOp::DIV:
                    v = builder.CreateSDiv(l, r);
                break;
            }
        }
    };
};

void CodeGen::compile(AST *tree)
{
    LLVMContext ctx;
    Module *m = new Module("calc.expr", ctx);
    ToIRVisitor to_ir(m);
    to_ir.run(tree);
    m->print(outs(), nullptr);
}
