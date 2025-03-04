# WebAssembly Instruction Equivalence Classes

The SSO paper defines equivalence classes for both Opcodes and Operands. In
addition to replacing an opcode or operand with another from its equivalence
class, lines may be swapped or marked as unused. To mark a line as unused, we
will use the `nop` instruction. As in the SSO paper, the equivalence classes
defined here do not include all WebAssembly Instructions.

All WebAssembly instructions function on the stack; parameters may be provided
to some instructions but the most reliable way to determine equivalence is to
observe how an instruction modifies the stack.

### Opcode Equivalence
Two Opcodes are equivalent if they take the same number and type of arguments.

i32 unop: consume one operand, produce one result
```wat
i32.clz
i32.ctz
i32.popcnt
```

i64 unop: consume one operand, produce one result
```wat
i64.clz
i64.ctz
i64.popcnt
```

f32 funop: consume one operand, produce one result
```wat
f32.abs
f32.neg
f32.sqrt
f32.ceil
f32.floor
f32.trunc
f32.nearest
```

f64 funop: consume one operand, produce one result
```wat
f64.abs
f64.neg
f64.sqrt
f64.ceil
f64.floor
f64.trunc
f64.nearest
```

i32 binop: consume two operands and produce one result 
```wat
i32.add
i32.sub
i32.mul
i32.div_u
i32.div_s
i32.rem_u
i32.rem_s
i32.and
i32.or
i32.xor
i32.shl
i32.shr_u
i32.shr_s
i32.rotl
i32.rotr
```

i64 binop: consume two operands and produce one result 
```wat
i64.add
i64.sub
i64.mul
i64.div_u
i64.div_s
i64.rem_u
i64.rem_s
i64.and
i64.or
i64.xor
i64.shl
i64.shr_u
i64.shr_s
i64.rotl
i64.rotr
```

f32.binop: consume two operands and produce one result 
```wat
f32.add
f32.sub
f32.mul
f32.div
f32.min
f32.max
f32.copysign
```

f64.binop: consume two operands and produce one result 
```wat
f64.add
f64.sub
f64.mul
f64.div
f64.min
f64.max
f64.copysign
```

i32.relop: consume two operands and produce one boolean integer result
```wat
i32.eq
i32.ne
i32.lt_u
i32.lt_s
i32.gt_u
i32.gt_s
i32.le_u
i32.le_s
i32.ge_u
i32.ge_s
```

i64.relop: consume two operands and produce one boolean integer result
```wat
i64.eq
i64.ne
i64.lt_u
i64.lt_s
i64.gt_u
i64.gt_s
i64.le_u
i64.le_s
i64.ge_u
i64.ge_s
```

f32.relop: consume two operands and produce one boolean integer result
```wat
f32.eq
f32.ne
f32.lt
f32.gt
f32.le
f32.ge
```

f64.relop: consume two operands and produce one boolean integer result
```wat
f64.eq
f64.ne
f64.lt
f64.gt
f64.le
f64.ge
```

v128.binop
```wat
v128.and
v128.andnot
v128.or
v128.xor
```

v128.relop (int)
```wat
v128.eq
v128.ne
v128.lt_u
v128.lt_s
v128.gt_u
v128.gt_s
v128.le_u
v128.le_s
v128.ge_u
v128.ge_s
```

v128.relop (float)
```wat
v128.eq
v128.ne
v128.lt
v128.gt
v128.le
v128.ge
```

v128.unop (int)
```wat
v128.abs
v128.neg
```

v128.binop (int)
```wat
v128.add
v128.sub
```

v128.unop (float)
```wat
v128.abs
v128.neg
v128.sqrt
v128.ceil
v128.floor
v128.trunc
v128.nearest
```

v128.binop (float)
```wat
v128.add
v128.sub
v128.mul
v128.div
v128.min
v128.max
v128.pmin
v128.pmax
```