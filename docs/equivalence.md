# Equivalence of Rewrites

The STOKE method approximates equivalence of a rewrite via sampling. 32 test
cases are generated automatically and the states of registers and memory are
compared between the Target code and the Rewrite to calculate similarity.
WebAssembly is a stack-based virtual machine and therefore does not have
registers; a different equivalence method is needed.

# Equivalence in WebAssembly

Equivalence will still be defined via sampling of test cases. Equivalence will
need to be checked in terms of both the stack and linear memory.


## Stack Equivalence

WebAssembly is a typed language and is explicit about the state of the stack at the
beginning and end of each function. No executable code can exist outside of a
function. The structure of a WebAssembly function is:
```wat 
(func $add (param i32 i32) (result i32) (local i32)
    (local.get 0) 
    (local.get 1) 
    (i32.add)
    (local.set 2)
    (local.get 2)
)
```
The `param`, `result`, and `local` expression indicate how the stack will be
used within the function. Functions many not access parts of the stack outside
of the regions defined by these expressions.
- `param` is a list of parameter types which are implicitly numbered starting
  from zero. This is what the function expects to find on the part of the stack
  which it can manipulate.
- `result` is a list of types which will remain on the function's mutable
  portion of the stack when control is returned to the caller
- `local` is a list of types which define stack locations that will be used only
  within the function. These stack locations are completely opaque to the caller.

## Memory Equivalence

Along with manipulating the stack, WebAssembly functions may modify linear
memory. STOKE identifies all load/store operations in the code segment being
evaluated and checks for similarity among corresponding locations. No
modification is needed to adapt this technique to WebAssembly.

# Implementing Equivalence

Although WebAssembly is interpreted, it must follow a strict structure to be
considered valid.

## Stack Equivalence

No embedder exists which allows us to directly query the state of the stack, so
we must rely on the `param` and `result` expressions as shown above. WAST, a
simple script-like extension to WebAssembly meant to enable testing, adds the
ability to "invoke" functions. A WAST script can set up the stack, call a
function, and access the `result`s left on the stack by the callee. WAST scripts
can be interpreted directly by the [WebAssembly Reference
Interpreter](https://github.com/WebAssembly/spec/tree/main/interpreter) and
return results in a simple textual format. While this satisfies the requirements
for validating stack equivalence, it cannot be used in an analogous manner for
memory. Therefore, functions will be annotated with `export` keywords and will
be called from a host embedder. Checking for equivalence between Target and
Rewrite results remains trivial under this technique.

## Memory Equivalence

As in STOKE, straight-line segments of code may be statically analyzed to
identify memory accesses. WebAssembly has several methods of memory access that
differentiate between locations:

- Linear memory can be modified using `<type>.store` instructions
- Regions of linear memory can be filled with a value using `memory.fill` and
  initialized from data segments with `memory.init`
- Global variables may be modified with `global.set`
- Table entries may be modified with `table.set`, `table.fil`, `table.init`, and
  `table.copy`

For more information:
- [Memory
Instructions](https://webassembly.github.io/spec/core/binary/instructions.html#memory-instructions)
- [Global Variable
  Instructions](https://webassembly.github.io/spec/core/binary/instructions.html#variable-instructions)
- [Table
  Instructions](https://webassembly.github.io/spec/core/syntax/instructions.html#syntax-instr-table)

In regular straight-line code, memory and global load/set instructions should be
the most common. Table functions and other memory functions are more advanced
and will likely not appear until testing real-world code bases, if at all.

We can calculate similarity between the output of the Target and output of the
Rewrite by monitoring the locations modified by these instructions. The STOKE
paper performs additional checks to verify that any memory accesses are within
the memory bounds of the process; they do this for security. In our case no
extra checking is necessary because WebAssembly runtimes already do this.

Embedders such as Wasmtime expose rich access to the linear memory in addition
to being able to call functions. For every location that code modifies, the
Hamming distance between the value produced by the target and the value produced
by the candidate Rewrite will define the memory equivalence.
