#include <cstdio>
#include <iostream>
#include <fstream>

#include <memory>

#include "pass.h"
#include "support/command-line.h"
#include "support/file.h"
#include "support/result.h"
#include "wasm-binary.h"
#include "wasm-interpreter.h"
#include "wasm-validator.h"
#include "support/sso_loss.h"


// #define TEST_FUNCTION_NAME "test"

using namespace wasm;

int main(int argc, const char* argv[]) {
  std::string infile = "-";
  Options options("wasm-shell", "Execute .wast files");
  options.add_positional(
    "INFILE",
    Options::Arguments::One,
    [&](Options* o, const std::string& argument) { infile = argument; });
  options.parse(argc, argv);

  FuncSize fs = calculate_function_length(infile);

  std::cout << "Function occupies " << fs.func_start << " - " << fs.func_end << std::endl;
  std::cout << "Normalized function length: " << fs.normalized_wat_length << std::endl;
  std::cout << fs.excerpt << std::endl;
}
