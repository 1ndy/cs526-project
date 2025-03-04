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

char* read_wat_file(std::string filename) {
    FILE *file = fopen(filename.c_str(), "r");
    if (!file) {
        std::cerr << "Error reading .wat file" << std::endl;
        exit(1);
    }
    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    char* wat = (char*)malloc(file_size);
    if (fread(wat, file_size, 1, file) != 1) {
        std::cout << "Error readng module" << std::endl;
        exit(1);
    }
    fclose(file);
    return wat;
}

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
  std::cout << "Normalized function length: " << fs.normalized_length << std::endl;
  std::cout << fs.excerpt << std::endl;
}
