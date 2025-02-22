#include <bitset>
#include "equivalence.h"

TestCase::TestCase(wasm::Signature sig) {
    this->sig = sig;
    auto params_type = this->sig.params;
    auto results_type = this->sig.results;

    // create wasmtime_val_t array of correct size for number of params
    std::vector<wasm::Type> params_list;
    if (params_type.isTuple()) {
        params_list = params_type.getTuple();
        this->test_params = (wasmtime_val_t*)malloc(sizeof(wasmtime_val_t) * params_list.size());
        this->num_params = params_list.size();
    } else {
        this->test_params = (wasmtime_val_t*)malloc(sizeof(wasmtime_val_t));
        this->test_params[0].kind = convert_type_name(params_type);
        this->num_params = 1;
        params_list = {params_type};
    }

    // fill the params array with values of the correct type
    for (int i = 0; i < (int)this->num_params; i++) {
        auto ty = params_list[i];
        this->test_params[i].kind = convert_type_name(ty);
        switch (this->test_params[i].kind) {
            case WASMTIME_I32: this->test_params[i].of.i32 = 1; break;
            case WASMTIME_I64: this->test_params[i].of.i64 = 1; break;
            case WASMTIME_F32: this->test_params[i].of.f32 = 0.0f; break;
            case WASMTIME_F64: this->test_params[i].of.f64 = 1.0f; break;
        }
    }

    // set up results arrays, first the length
    std::vector<wasm::Type> results_list;
    if (sig.results.isTuple()) {
        this->num_results = results_type.getTuple().size();
        results_list = results_type.getTuple();
    } else {
        this->num_results = 1;
        results_list = {results_type};
    }

    // then the allocation
    this->target_results = (wasmtime_val_t*)malloc(sizeof(wasmtime_val_t) * results_list.size());
    this->rewrite_results = (wasmtime_val_t*)malloc(sizeof(wasmtime_val_t) * results_list.size());
}

uint32_t TestCase::distance() {
    uint32_t distance = 0;
    for(size_t i = 0; i < this->num_results; i++) {
        distance += this->hammingDistance(target_results[i], rewrite_results[i]);
    }
    return distance;
}

void TestCase::printParams() {
    this->print(this->test_params, this->num_params, "params");
}

void TestCase::printResults() {
    this->print(this->target_results, this->num_results, " (target)results");
    this->print(this->rewrite_results, this->num_results, "(rewrite)results");
}

void TestCase::print(wasmtime_val_t* vals, int length, std::string name) {
    for (int i = 0; i < length; i++) {
        std::cout << name << "[" << i << "]: ";
        switch (vals[i].kind) {
            case WASMTIME_I32: std::cout << "i32 = " << vals[i].of.i32; break;
            case WASMTIME_I64: std::cout << "i64 = " << vals[i].of.i64; break;
            case WASMTIME_F32: std::cout << "f32 = " << vals[i].of.f32; break;
            case WASMTIME_F64: std::cout << "f64 = " << vals[i].of.f64; break;
        }
        std::cout << std::endl;
    }
}

wasmtime_val_t* TestCase::getParams() { return this->test_params; }
uint32_t TestCase::getParamsCount() { return this->num_params; }
uint32_t TestCase::getResultsCount() { return this->num_results; }
wasmtime_val_t* TestCase::getMutableTargetResults() { return this->target_results; }
wasmtime_val_t* TestCase::getMutableRewriteResults() { return this->rewrite_results; }

int TestCase::convert_type_name(wasm::Type ty) {
    if (ty.isI32()) {
        return WASMTIME_I32;
    } else if (ty.isI64()) {
        return WASMTIME_I64;
    } else if (ty.isF32()) {
        return WASMTIME_F32;
    } else if (ty.isF64()) {
        return WASMTIME_F64;
    } else {
        std::cerr << "Failed to match type" << std::endl;
        exit(1);
    }
}

uint32_t TestCase::hammingDistance(wasmtime_val_t& target_result, wasmtime_val_t& rewrite_result) {
    uint32_t distance = 0;
    if (target_result.kind == WASMTIME_I32) {
        std::bitset<32> a(target_result.of.i32);
        std::bitset<32> b(rewrite_result.of.i32);
        distance = (a^b).count();
    } else if (target_result.kind == WASMTIME_F32) {
        std::bitset<32> a(target_result.of.f32);
        std::bitset<32> b(rewrite_result.of.f32);
        distance = (a^b).count();
    } else if (target_result.kind == WASMTIME_I64) {
        std::bitset<64> a(target_result.of.i64);
        std::bitset<64> b(rewrite_result.of.i64);
        distance = (a^b).count();
    } else if (target_result.kind == WASMTIME_F64) {
        std::bitset<64> a(target_result.of.f64);
        std::bitset<64> b(rewrite_result.of.f64);
        distance = (a^b).count();
    } else {
        std::cerr << "Unknown type in hamming distance" << std::endl;
        exit(1);
    }
    return distance;
}


// ============================================================================
// Executor
// ============================================================================
Executor::Executor(std::string wat_file) {
    this->wat_file = wat_file;

    // wasmtime boilerplate
    this->engine = wasm_engine_new();
    assert(engine != NULL);
    this->store = wasmtime_store_new(engine, NULL, NULL);
    assert(store != NULL);
    this->context = wasmtime_store_context(store);

    // Load input wat files
    wasm_byte_vec_t* wat = this->read_wat_file(this->wat_file);

    // Parse the target and rewrite wats into wasm binary
    wasm_byte_vec_t wasm;
    wasmtime_error_t *error = wasmtime_wat2wasm(wat->data, wat->size, &wasm);
    if (error != NULL) {
    std::cerr << "Failed to parse wat into wasm" << std::endl;
    exit(1);
    }
    wasm_byte_vec_delete(wat);

    // Compile modules
    this->module = NULL;
    error = wasmtime_module_new(engine, (uint8_t*)wasm.data, wasm.size, &(this->module));
    if (this->module == NULL) {
    std::cerr << "Failed to compile module" << std::endl;
    exit(1);
    }
    wasm_byte_vec_delete(&wasm);

    // instantiate module
    wasm_trap_t* trap = NULL;
    error = wasmtime_instance_new(this->context, this->module, NULL, 0, &(this->instance), &trap);
    if (error != NULL || trap != NULL) {
    std::cerr << "Failed to instantiate module" << std::endl;
    exit(1);
    }

    // Create handle to exported test function
    bool ok = wasmtime_instance_export_get(this->context, &(this->instance), TEST_FUNCTION_NAME, std::string(TEST_FUNCTION_NAME).length(), &(this->func));
    assert(ok);
    assert(this->func.kind == WASMTIME_EXTERN_FUNC);
}

void Executor::runTestCase(TestCase& tc, bool is_target) {
    auto params = tc.getParams();
    wasmtime_val_t* results;
    if (is_target) {
        results = tc.getMutableTargetResults();
    } else {
        results = tc.getMutableRewriteResults();
    }
    wasm_trap_t* trap = NULL;
    wasmtime_error_t* error = wasmtime_func_call(this->context, &(this->func.of.func), params, tc.getParamsCount(), results, tc.getResultsCount(), &trap);
    if (error != NULL || trap != NULL) {
        std::cerr << "Failed to call func" << std::endl;
        exit(1);
    }
}

wasm_byte_vec_t* Executor::read_wat_file(std::string wat_file) {
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

// ============================================================================
// TestCaseRunner
// ============================================================================
TestCaseRunner::TestCaseRunner(Executor target, Executor rewrite, std::vector<TestCase> testcases)
    : target(target), rewrite(rewrite) {
    this->target = target;
    this-> rewrite = rewrite;
    this->testcases = testcases;
}

void TestCaseRunner::runTestCases() {
    for(size_t i = 0; i < this->testcases.size(); i++) {
        TestCase tc = this->testcases[i];
        // each call to RunTestCase populates a wasmtime_val_t result struct
        this->target.runTestCase(tc, true);
        this->rewrite.runTestCase(tc, false);
    }
}
