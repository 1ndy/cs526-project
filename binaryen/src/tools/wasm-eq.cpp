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

// #define TEST_FUNCTION_NAME "test"

using namespace wasm;

using namespace wasm::WATParser;

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
  auto sig = f->getSig();
  std::cout << "Function has signature " << sig << std::endl;

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
