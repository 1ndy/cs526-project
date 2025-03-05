#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "wasm.h"
#include "wasmtime.h"

wasm_byte_vec_t* read_wat_file(std::string wat_file) {
    FILE *file = fopen(wat_file.c_str(), "r");
    if (!file) {
        std::cerr << "Error reading .wat file" << std::endl;
        exit(1);
    }
    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    wasm_byte_vec_t* wat = (wasm_byte_vec_t*)malloc(sizeof(wasm_byte_vec_t));
    wasm_byte_vec_new_uninitialized(wat, file_size);
    if (fread(wat->data, file_size, 1, file) != 1) {
        std::cout << "Error readng module" << std::endl;
        exit(1);
    }
    fclose(file);
    return wat;
}