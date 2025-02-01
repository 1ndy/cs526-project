# A Code-Synthesis approach to Interoperability

## Motivation

In the age of the fourth paradigm[^1] of scientific research, computation plays a
pivotal role in achieving evermore precise and complex results. Despite their reliance on software, domain scientists rarely command an aptitude in computer science principles; often, their code is oriented towards producing results rather than encoding their method in reusable or even reproducible way. The preference within certain subfields for a specific language drives a wedge between already siloed disciplines, hampering collaboration.

Solutions to connect codebases in different languages do exist. Chief among them is the Foreign Function Interface (FFI). FFIs are bespoke software libraries written in addition to existing code which export the functionality of the software for use in another language. Unfortunately, most FFIs are designed to export functions to or use functions from a C program. They also require knowledge of the Application Binary Interface, data representation, computer memory, and other advanced topics. Lastly, writing FFIs does not contribute to a research product and therefore would not be prioritized even if it was easy.

The other major current solution to interoperability is serialization. Many programming languages possess the ability to automatically generate serialization and deserialization routines from static definitions of structs or classes. The routines will encode the data from these memory objects into a common, easy-to-parse format such as JSON. Another program which can accept serialized data in the same format may then parse that representation into its own internal data structure. Though this has a significantly lower barrier to use than FFIs, serialization techniques spend large amounts of time converting bytes from memory into a string representation. Finally, serialization formats support limited complexity within a data structure; for example, references cannot be effectively encoded in a serialized format.


## Background

WebAssembly (wasm) is a binary instruction format for a stack-based virtual
machine. Though originally intended to accelerate client-side code in web
browsers, wasm executables can be interpreted by an "embedder" and run in
virtually any environment. Wasm's emergence as a performant, portable
instruction format has made it a compilation target for many source languages
including C/C++, Go, Rust, .NET, Zig, Python, Javascript, Ocaml, and others.

With so many languages targeting wasm, it becomes possible to think about
software libraries which are comprised of functions and data structures authored
in different programming languages. While the existence of a portable format
makes a great stride towards realizing this, "source language artifacts" are
clearly visible in the generated wasm code and inhibit direct interoperation.
Examples of artifacts include runtimes, memory allocation schemas, arrays bounds
checking, and struct packing.

The next step in achieving effortless interoperability is to make compatible the
in-memory representations of data structures.


## Example Desired Outcome

Suppose we have a struct "Point", which stores the X, Y, and Z integer
coordinates and a name. We then define a function "distance" which, for
simplicity, returns the sum of the values on each axis. These objects can be
easily implemented in both C and Rust and then compiled to wasm with emscripten
and rustc respectively. The complete wasm from the C code is:
```wasm
(module
  (type (;0;) (func (param i32) (result i32)))
  (import "env" "__linear_memory" (memory (;0;) 1))
  (func $distance (type 0) (param i32) (result i32)
    (local i32 i32 i32 i32 i32)
    local.get 0
    i32.load
    local.set 1
    local.get 0
    i32.load offset=4
    local.set 2
    local.get 1
    local.get 2
    i32.add
    local.set 3
    local.get 0
    i32.load offset=8
    local.set 4
    local.get 3
    local.get 4
    i32.add
    local.set 5
    local.get 5
    return)
  (data $.L.str (i32.const 0) "hello\00")
  (data $pt (i32.const 8) "\01\00\00\00\02\00\00\00\03\00\00\00\00\00\00\00")
  (data $.L.str.1 (i32.const 24) "goodbye\00")
  (data $pt2 (i32.const 32) "\01\00\00\00\02\00\00\00\03\00\00\00\18\00\00\00"))
```
and a selection of the much larger wasm produced by the Rust compiler:
```wasm
(func $distance (type 3) (param i32) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
    local.get 0
    i32.load offset=8
    local.set 1
    local.get 0
    i32.load offset=12
    local.set 2
    i32.const 0
    local.set 3
    local.get 2
    local.get 3
    i32.lt_s
    local.set 4
    local.get 1
    local.get 2
    i32.add
    local.set 5
    local.get 5
    local.get 1
    i32.lt_s
    ...
```
Immediately, we see that the offsets for the C version of the code do not match
those of the Rust version. If the C function were to be passed the memory
location of the rust-produced struct, it would index into the data structure
incorrectly. Using the [Rust Reference on Data Layouts](https://doc.rust-lang.org/reference/type-layout.html) and the C99 specification, we can figure out that the C implementation did not insert padding bytes, while the Rust implementation did. With source language and compiler version information given as preconditions, it should be possible to apply rewrite rules to the data accesses such that the C function would correctly index into the memory allocated for the Rust struct.

---
1. https://www.microsoft.com/en-us/research/publication/fourth-paradigm-data-intensive-scientific-discovery/ 