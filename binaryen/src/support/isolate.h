#include <cstdio>
#include <iostream>

#include <memory>

// #include "execution-results.h"
#include "ir/element-utils.h"
#include "parser/lexer.h"
#include "parser/wat-parser.h"
#include "pass.h"
// #include "support/command-line.h"
// #include "support/file.h"
// #include "support/result.h"
#include "wasm-binary.h"
#include "wasm-interpreter.h"
#include "wasm-validator.h"

#include "ir/linear-execution.h"

// #define TEST_FUNCTION_NAME "test"

using namespace wasm;

using namespace wasm::WATParser;

struct FunctionSplitter : public LinearExecutionWalker<FunctionSplitter, Visitor<FunctionSplitter, void>> {
    wasm::Function* current = nullptr;
    wasm::Tuple params;
    Block* body = nullptr;
    std::vector<Expression*> expressions_stack;

    void doWalkFunction(Function* func) {
        reset_splitter();
        walk(func->body);
    }

    void reset_splitter() {
        current = new wasm::Function();
        params.clear();
        body = getModule()->allocator.alloc<Block>();
        for (size_t i = 0 ; i < expressions_stack.size(); i++) {
            params.push_back(expressions_stack[i]->type);
            LocalGet* new_get = getModule()->allocator.alloc<LocalGet>();
            new_get->index = i;
            new_get->type = expressions_stack[i]->type;
            expressions_stack[i] = new_get;
        }

        for (size_t i = 0; i < getFunction()->getNumLocals(); i++) {
            params.push_back(getFunction()->getLocalType(i));
        }

        current->body = body;
    }

    void noteNonLinear(Expression* curr) {
        current->clearNames();

        wasm::Tuple results_type;
        TupleMake* tuple_make = getModule()->allocator.alloc<TupleMake>();
        wasm::ExpressionList& results = tuple_make->operands;
        for (Expression* expr : expressions_stack) {
            results_type.push_back(expr->type);
            results.push_back(expr);
        }

        for (size_t i = 0; i < getFunction()->getNumLocals(); i++) {
            results_type.push_back(getFunction()->getLocalType(i));
            LocalGet* expr = getModule()->allocator.alloc<LocalGet>();
            expr->index = i;
            expr->type = getFunction()->getLocalType(i);
            results.push_back(expr);
        }
        body->list.push_back(tuple_make);

        current->type = HeapType(Signature(params, results_type));

        current->name = "test";
        std::cout << *current << std::endl;

        delete current;
        reset_splitter();
    }

    void visitConst(Const* curr) {
        Const* expr = getModule()->allocator.alloc<Const>();
        expr->value = curr->value;
        expr->type = curr->type;
        expressions_stack.push_back(expr);
    }

    void visitLocalGet(LocalGet* curr) {
        current->clearNames();
        LocalGet* expr = getModule()->allocator.alloc<LocalGet>();
        expr->index = curr->index;
        expr->type = curr->type;
        expressions_stack.push_back(expr);
    }

    void visitLocalSet(LocalSet* curr) {
        LocalSet* expr = getModule()->allocator.alloc<LocalSet>();
        expr->index = curr->index;
        expr->type = curr->type;
        expr->value = expressions_stack.back();
        expressions_stack.pop_back();
        if (expr->isTee()) {
            expressions_stack.push_back(expr);
        } else {
            body->list.push_back(expr);
        }
    }

    void visitDrop(Drop* curr) {
        Drop* expr = getModule()->allocator.alloc<Drop>();
        expr->value = expressions_stack.back();
        expressions_stack.pop_back();
        body->list.push_back(expr);
    }

    void visitBreak(Break* curr) {
        if (curr->condition) {
            expressions_stack.pop_back();
        }
    }

    void visitBinary(Binary* curr) {
        Binary* expr = getModule()->allocator.alloc<Binary>();
        assert(expressions_stack.size() >= 2);
        expr->right = expressions_stack.back();
        expressions_stack.pop_back();
        expr->left = expressions_stack.back();
        expressions_stack.pop_back();
        expr->type = curr->type;
        expr->op = curr->op;
        expressions_stack.push_back(expr);
    }
};