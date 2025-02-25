#include <cstdio>
#include <iostream>

#include <memory>

#include "execution-results.h"
#include "ir/element-utils.h"
#include "parser/lexer.h"
#include "parser/wat-parser.h"
#include "pass.h"
#include "support/command-line.h"
#include "support/file.h"
#include "support/result.h"
#include "wasm-binary.h"
#include "wasm-interpreter.h"
#include "wasm-validator.h"

#include "support/equivalence.h"
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

int main(int argc, const char* argv[]) {
  Name entry;
  std::set<size_t> skipped;

  // Read stdin by default.
  std::string target_infile = "";
  std::string rewrite_infile = "";
  Options options("wasm-sso", "Equality tester for isolated wasm functions");
  options.add("--target", 
              "-t", 
              "Target's WAT file", 
              "general", 
              Options::Arguments::One,
              [&](Options* o, const std::string& argument) { target_infile = argument; });
  options.add("--rewrite", 
              "-r", 
              "Rewrite's WAT file", 
              "general", 
              Options::Arguments::One,
              [&](Options* o, const std::string& argument) { rewrite_infile = argument; });
  options.parse(argc, argv);

  auto target_input = read_file<std::string>(target_infile, Flags::Text);

  // try to parse the wasm module
  wasm::Module wasmModule;
  Lexer lexer(target_input);
  auto result = WATParser::parseModule(wasmModule, lexer);
  if (auto* err = result.getErr()) {
    std::cout << err->msg << std::endl;
    exit(1);
  } else {
      std::cout << "Parsed module successfully" << std::endl;
  }

  // extract function
  wasm::Function* f = wasmModule.getFunctionOrNull(TEST_FUNCTION_NAME);
  if (f == NULL) {
    std::cerr << "Could not find function named \"" << TEST_FUNCTION_NAME << 
    "\": signature must be (func $" << TEST_FUNCTION_NAME << "(export \"" << 
    TEST_FUNCTION_NAME << "\") ...)" << std::endl;
    exit(1);
  }

  FunctionSplitter balls;
  balls.walkFunctionInModule(f, &wasmModule);

  auto sig = f->getSig();
  std::cout << "Function has signature " << sig << std::endl;
    std::cout << "Function has signature " << *f << std::endl;

  // create test case from signature
  auto tc = TestCase(sig);
  tc.printParams();

  // create executor for target and rewrite
  Executor target(target_infile);
  Executor rewrite(rewrite_infile);
  TestCaseRunner tcr(target, rewrite, {tc});

  // run test case through executors
  tcr.runTestCases();
  tc.printResults();
  std::cout << "Distance: " << tc.distance() << std::endl;
}
