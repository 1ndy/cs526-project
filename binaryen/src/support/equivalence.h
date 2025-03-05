#include <cstdio>
#include <iostream>
#include <set>

// Binaryen includes
#include "support/sso_loss.h"

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
    FuncSize func_size;

  private:
    std::string wat_file;
    wasm_engine_t* engine;
    wasmtime_store_t* store;
    wasmtime_context_t* context;
    wasmtime_module_t *module;
    wasmtime_instance_t instance;
    wasmtime_extern_t func;
//    wasmtime_val_t* params;
//    wasmtime_val_t* results;

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

using namespace wasm;
class OpcodeEquivalenceClass {
  public:
    OpcodeEquivalenceClass() {
      i32_unop = std::set<UnaryOp>{ClzInt32, CtzInt32, PopcntInt32};
      i64_unop = std::set<UnaryOp>{ClzInt64, CtzInt64, PopcntInt64};
      f32_unop = std::set<UnaryOp>{AbsFloat32, NegFloat32, SqrtFloat32, CeilFloat32, FloorFloat32, TruncFloat32, NearestFloat32};
      f64_unop = std::set<UnaryOp>{AbsFloat64, NegFloat64, SqrtFloat64, CeilFloat64, FloorFloat64, TruncFloat64, NearestFloat64};
      i32_binop = std::set<BinaryOp>{AddInt32, SubInt32, MulInt32, DivUInt32, DivSInt32, RemUInt32, RemSInt32, AndInt32, OrInt32, XorInt32, ShlInt32, ShrUInt32, ShrSInt32, RotLInt32, RotRInt32};
      i64_binop = std::set<BinaryOp>{AddInt64, SubInt64, MulInt64, DivUInt64, DivSInt64, RemUInt64, RemSInt64, AndInt64, OrInt64, XorInt64, ShlInt64, ShrUInt64, ShrSInt64, RotLInt64, RotRInt64};
      f32_binop = std::set<BinaryOp>{AddFloat32, SubFloat32, MulFloat32, DivFloat32, MinFloat32, MaxFloat32, CopySignFloat32};
      f64_binop = std::set<BinaryOp>{AddFloat64, SubFloat64, MulFloat64, DivFloat64, MinFloat64, MaxFloat64, CopySignFloat64};
      i32_relop = std::set<BinaryOp>{EqInt32, NeInt32, LtUInt32, LtSInt32, GtUInt32, GtSInt32, LeUInt32, LeSInt32, GeUInt32, GeSInt32};
      i64_relop = std::set<BinaryOp>{EqInt64, NeInt64, LtUInt64, LtSInt64, GtUInt64, GtSInt64, LeUInt64, LeSInt64, GeUInt64, GeSInt64};
      f32_relop = std::set<BinaryOp>{EqFloat32, NeFloat32, LtFloat32, GtFloat32, LeFloat32, GeFloat32};
      f64_relop = std::set<BinaryOp>{EqFloat64, NeFloat64, LtFloat64, GtFloat64, LeFloat64, GeFloat64};
      v128_binop = std::set<BinaryOp>{AndVec128, AndNotVec128, OrVec128, XorVec128};

      v128_i8x16_irelop = std::set<BinaryOp>{EqVecI8x16, NeVecI8x16, LtUVecI8x16, LtSVecI8x16, GtUVecI8x16, GtSVecI8x16, LeUVecI8x16, LeSVecI8x16, GeUVecI8x16, GeSVecI8x16};
      v128_i16x8_irelop = std::set<BinaryOp>{EqVecI16x8, NeVecI16x8, LtUVecI16x8, LtSVecI16x8, GtUVecI16x8, GtSVecI16x8, LeUVecI16x8, LeSVecI16x8, GeUVecI16x8, GeSVecI16x8};
      v128_i32x4_irelop = std::set<BinaryOp>{EqVecI32x4, NeVecI32x4, LtUVecI32x4, LtSVecI32x4, GtUVecI32x4, GtSVecI32x4, LeUVecI32x4, LeSVecI32x4, GeUVecI32x4, GeSVecI32x4};
      v128_i64x2_irelop = std::set<BinaryOp>{EqVecI64x2, NeVecI64x2, LtSVecI64x2, GtSVecI64x2, LeSVecI64x2, GeSVecI64x2};

      v128_f16x8_frelop = std::set<BinaryOp>{EqVecF16x8, NeVecF16x8, LtVecF16x8, GtVecF16x8, LeVecF16x8, GeVecF16x8};
      v128_f32x4_frelop = std::set<BinaryOp>{EqVecF32x4, NeVecF32x4, LtVecF32x4, GtVecF32x4, LeVecF32x4, GeVecF32x4};
      v128_f64x2_frelop = std::set<BinaryOp>{EqVecF64x2, NeVecF64x2, LtVecF64x2, GtVecF64x2, LeVecF64x2, GeVecF64x2};

      v128_i8x16_unop = std::set<UnaryOp>{AbsVecI8x16, NegVecI8x16};
      v128_i16x8_unop = std::set<UnaryOp>{AbsVecI16x8, NegVecI16x8};
      v128_i32x4_unop = std::set<UnaryOp>{AbsVecI32x4, NegVecI32x4};
      v128_i64x2_unop = std::set<UnaryOp>{AbsVecI64x2, NegVecI64x2};
      v128_i8x16_binop = std::set<BinaryOp>{AddVecI8x16, SubVecI8x16};
      v128_i16x8_binop = std::set<BinaryOp>{AddVecI16x8, SubVecI16x8};
      v128_i32x4_binop = std::set<BinaryOp>{AddVecI32x4, SubVecI32x4};
      v128_i64x2_binop = std::set<BinaryOp>{AddVecI64x2, SubVecI64x2};

      v128_f16x8_unop = std::set<UnaryOp>{AbsVecF16x8, NegVecF16x8, SqrtVecF16x8, CeilVecF16x8, FloorVecF16x8, TruncVecF16x8, NearestVecF16x8};
      v128_f32x4_unop = std::set<UnaryOp>{AbsVecF32x4, NegVecF32x4, SqrtVecF32x4, CeilVecF32x4, FloorVecF32x4, TruncVecF32x4, NearestVecF32x4};
      v128_f64x2_unop = std::set<UnaryOp>{AbsVecF64x2, NegVecF64x2, SqrtVecF64x2, CeilVecF64x2, FloorVecF64x2, TruncVecF64x2, NearestVecF64x2};

      v128_f16x8_binop = std::set<BinaryOp>{AddVecF16x8, SubVecF16x8, MulVecF16x8, DivVecF16x8, MinVecF16x8, MaxVecF16x8, PMinVecF16x8, PMaxVecF16x8};
      v128_f32x4_binop = std::set<BinaryOp>{AddVecF32x4, SubVecF32x4, MulVecF32x4, DivVecF32x4, MinVecF32x4, MaxVecF32x4, PMinVecF32x4, PMaxVecF32x4};
      v128_f64x2_binop = std::set<BinaryOp>{AddVecF64x2, SubVecF64x2, MulVecF64x2, DivVecF64x2, MinVecF64x2, MaxVecF64x2, PMinVecF64x2, PMaxVecF64x2};

      unary_eqclasses = {
        i32_unop,
        i64_unop,
        f32_unop,
        f64_unop,
        v128_i8x16_unop,
        v128_i16x8_unop,
        v128_i32x4_unop,
        v128_i64x2_unop,
        v128_f16x8_unop,
        v128_f32x4_unop,
        v128_f64x2_unop
      };
      binary_eqclasses = {
        i32_binop,
        i64_binop,
        f32_binop,
        f64_binop,
        i32_relop,
        i64_relop,
        f32_relop,
        f64_relop,
        v128_binop,
        v128_i8x16_irelop,
        v128_i16x8_irelop,
        v128_i32x4_irelop,
        v128_i64x2_irelop,
        v128_f16x8_frelop,
        v128_f32x4_frelop,
        v128_f64x2_frelop,
        v128_i8x16_binop,
        v128_i16x8_binop,
        v128_i32x4_binop,
        v128_i64x2_binop,
        v128_f16x8_binop,
        v128_f32x4_binop,
        v128_f64x2_binop,
      };

    }

    // given an opcode, return list of equivalent opcodes
    std::set<UnaryOp> operator[] (UnaryOp opcode);
    std::set<BinaryOp> operator[] (BinaryOp opcode);

    // static functions to pick a rand value from the sets returned by
    // operator[]
    // https://stackoverflow.com/a/3052796
    static UnaryOp choose(std::set<UnaryOp> opcodes);
    static BinaryOp choose(std::set<BinaryOp> opcodes);

  private:
    std::set<UnaryOp> i32_unop;
    std::set<UnaryOp> i64_unop;
    std::set<UnaryOp> f32_unop;
    std::set<UnaryOp> f64_unop;
    std::set<BinaryOp> i32_binop;
    std::set<BinaryOp> i64_binop;
    std::set<BinaryOp> f32_binop;
    std::set<BinaryOp> f64_binop;
    std::set<BinaryOp> i32_relop;
    std::set<BinaryOp> i64_relop;
    std::set<BinaryOp> f32_relop;
    std::set<BinaryOp> f64_relop;
    std::set<BinaryOp> v128_binop;
    std::set<BinaryOp> v128_i8x16_irelop;
    std::set<BinaryOp> v128_i16x8_irelop;
    std::set<BinaryOp> v128_i32x4_irelop;
    std::set<BinaryOp> v128_i64x2_irelop;
    std::set<BinaryOp> v128_f16x8_frelop;
    std::set<BinaryOp> v128_f32x4_frelop;
    std::set<BinaryOp> v128_f64x2_frelop;
    std::set<UnaryOp> v128_i8x16_unop;
    std::set<UnaryOp> v128_i16x8_unop;
    std::set<UnaryOp> v128_i32x4_unop;
    std::set<UnaryOp> v128_i64x2_unop;
    std::set<BinaryOp> v128_i8x16_binop;
    std::set<BinaryOp> v128_i16x8_binop;
    std::set<BinaryOp> v128_i32x4_binop;
    std::set<BinaryOp> v128_i64x2_binop;
    std::set<UnaryOp> v128_f16x8_unop;
    std::set<UnaryOp> v128_f32x4_unop;
    std::set<UnaryOp> v128_f64x2_unop;
    std::set<BinaryOp> v128_f16x8_binop;
    std::set<BinaryOp> v128_f32x4_binop;
    std::set<BinaryOp> v128_f64x2_binop;
    std::vector<std::set<UnaryOp>> unary_eqclasses;
    std::vector<std::set<BinaryOp>> binary_eqclasses;
};