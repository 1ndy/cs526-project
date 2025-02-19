#include <cstdio>
#include <iostream>

// wasmtime includes
#include "wasm.h"
#include "wasmtime.h"

#define TEST_FUNCTION_NAME "test"

// TestCase is a collection of parameters and results for one wasm function
class TestCase {
  public:
    TestCase(wasm::Signature sig);

    uint32_t distance();
    void printParams();
    void printResults();
    wasmtime_val_t* getParams();
    uint32_t getParamsCount();
    uint32_t getResultsCount();
    wasmtime_val_t* getMutableTargetResults();
    wasmtime_val_t* getMutableRewriteResults();

  private:
    // sig comes from Binaryen's parser
    wasm::Signature sig;

    // results will be filed by calling the executor on this object
    wasmtime_val_t* target_results;
    wasmtime_val_t* rewrite_results;
        
    uint32_t num_params;
    uint32_t num_results;

    // the params from the test case, derived from sig
    wasmtime_val_t* test_params;

    int convert_type_name(wasm::Type ty);
    uint32_t hammingDistance(wasmtime_val_t& target, wasmtime_val_t& rewrite);
    void print(wasmtime_val_t* vals, int length, std::string name);
};

class Executor {

  public:
    Executor(std::string wat_file);
    void runTestCase(TestCase& tc, bool is_target);

  private:
    std::string wat_file;
    wasm_engine_t* engine;
    wasmtime_store_t* store;
    wasmtime_context_t* context;
    wasmtime_module_t *module;
    wasmtime_instance_t instance;
    wasmtime_extern_t func;
    wasmtime_val_t* params;
    wasmtime_val_t* results;

    static wasm_byte_vec_t* read_wat_file(std::string wat_file);
};

class TestCaseRunner {
  public:
    TestCaseRunner(Executor target, Executor rewrite, std::vector<TestCase>);
    void runTestCases();


  private:
    Executor target;
    Executor rewrite;
    std::vector<TestCase> testcases;
    std::vector<int> distances;
};