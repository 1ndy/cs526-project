#include <cstdio>
#include <iostream>

#include <memory>

#include "ir/element-utils.h"
#include "parser/lexer.h"
#include "parser/wat-parser.h"
#include "support/command-line.h"
#include "support/file.h"

#include "support/isolate.h"

using namespace wasm;

using namespace wasm::WATParser;

int main(int argc, const char* argv[]) {
  Name entry;
  std::set<size_t> skipped;

  // Read stdin by default.
  std::string infile = "-";
  Options options("wasm-split", "Split WebAssembly Module into SSO units");
  options.add_positional("INFILE", 
                         Options::Arguments::One,
                         [&](Options* o, const std::string& argument) { infile = argument; });
  options.parse(argc, argv);

  auto input_module = read_file<std::string>(infile, Flags::Text);

  // try to parse the wasm module
  wasm::Module wasmModule;
  Lexer lexer(input_module);
  auto result = WATParser::parseModule(wasmModule, lexer);
  if (auto* err = result.getErr()) {
    std::cout << err->msg << std::endl;
    exit(1);
  } else {
      std::cout << "Parsed module successfully" << std::endl;
  }

  // iterate through functions and split them up
  FunctionSplitter balls;
  for (auto name : wasmModule.getFunctionNames()) {
    wasm::Function* f = wasmModule.getFunction(name);
    // TODO 
    //  1. walk function and serialize to file
    //  2. define some predictable naming scheme for 
    //     function files to ease re-integration
    std::cout << "Function with signature " << f->getSig() << std::endl;
  }
}
