Stochastic Superoptimization of WebAssembly Bytecode Size

WebAssembly (wasm) is a binary instruction format for a stack-based virtual
machine intended to execute client-side computations in web browsers. Designed
to be performant and safe, it has become a compilation target for many source
languages including C/C++, Go, Rust, .NET, Zig, OCaml, Dart, Swift, and others.
Optimizing the resulting WebAssembly modules for size is critical for several
reasons (4). By its nature, WebAssembly code is frequently sent to clients via a
network connection. In this situation, every byte sent adds latency to the start
of execution of the client side code. Once the code is delivered to the browser
(and sometimes before it is completely transferred), a JIT compiler converts
WebAssembly instructions into ISA-specific actions on the host. Less code means
fewer instructions that the JIT compiler must process. This could mean less
power expended on processing, which is a concern for battery-powered devices
which access the web. Size-optimized WebAssembly can lead to efficiency gains in
multiple dimensions. Therefore optimal, minimal WebAssembly code is extremely
desirable.

With so many source languages targeting WebAssembly, some via their own code
generators, the diversity of bytecode is extremely high, even when the source
languages contain semantically identical functions. The resulting wider range of
inputs to an optimization pass increases the likelihood of missing optimization
opportunities. For example, (1) found that even Emscripten, arguably the most
mainstream WebAssembly compiler, produced “optimized” code that contained
redundant initializations. Binaryen, an optimizing compiler for WebAssembly,
contains in its codebase compiler passes specific to Emscripten for this reason;
it also includes several for Java, Javascript, and LLVM IR (9). WebAssembly is a
target for many compilers; manually developing optimizations specific to each
one’s output is unsustainable. An adaptable approach not based on manually
created rules will more effectively support the rapid adoption of WebAssembly.

We therefore propose a project which applies Stochastic Superoptimization (3) to
WebAssembly bytecode with the goal of minimizing bytecode size.

State of the Art

Super-optimization is the pursuit of the shortest possible sequence of machine
instructions required to solve a problem (2). Projects such as Souper (5) and
Ansor (6) apply inductive and evolutionary optimization techniques respectively,
while Stoke (3) employs a stochastic method to achieve the shortest code. Each
works by exploring the space, defined by the order of instructions, that the
input program inhabits in an attempt to find a semantically equivalent but more
optimal version of the code (4). While superoptimization on WebAssembly has been
attempted (7,8), it has leveraged methods which focus on completeness. In (7),
Arteaga et al. apply Souper to LLVM IR before compiling to WebAssembly. While
this means that the final bytecode benefits from the expansive optimization
passes available in that compiler infrastructure, the process neglects to
account for the idiosyncrasies of WebAssembly specifically, meaning that some
optimization opportunities could be neglected. In (8), Sprokholt applies the
CounterExample-Guided Inductive Synthesis (CEGIS, 10) approach on which Souper
is based to manipulate directly the WebAssembly code’s process graphs, using
approximate instruction costs to choose instructions during program synthesis.
While this approach demonstrates good performance improvements on a variety of
problems, its testing on real-world codebases yielded modules that were not less
than 99% the size of the original. Optimization times from real world programs
ranged from a few seconds to over 9 hours. (Benchmarks for the real world
programs were excluded due to large amounts of Javascript glue code.) These
results suggest that there is significant opportunity for optimization in
WebAssembly, and show that in some cases those optimizations require large
amounts of time to uncover.


Our Improvements 

Stochastic optimization (3) forgoes the SMT-based approach in
favor of a Markov Chain Monte Carlo (MCMC) technique that can optimize over an
irregular loss function. The stochastic technique starts from a random location
within the search space, meaning that it can find optimizations which are
distant from those performed by traditional optimization passes. This is
particularly important when homegrown code generators may be generating exotic
sequences of WebAssembly instruction patterns that do not match with existing
optimization rules. Although completeness cannot be guaranteed, this approach
provides greater speed than SMT-based methods as well as flexibility in the axes
on which to optimize: code size and power usage are explicitly mentioned as
measures that may be improved stochastically simply by changing the loss
function. With a sufficiently robust implementation, our contribution will lay
the groundwork for highly customizable optimization objectives, potentially at
the granularity of specific WebAssembly embedders such as Wasmtime and V8.

The size of each opcode in WebAssembly is a single byte, meaning that a
reduction in the overall number of instructions will be the primary optimization
goal. However, given that data sent over the web is frequently compressed, we
will add a loss function which considers the compressed size of the optimized
code if time allows. Binaryen provides compiler infrastructure for
Webassembly-in, Webassembly-out optimization passes; we will explore this
framework as a place to house our contribution but may switch to a standalone
Python implementation if the mathematical component if the work becomes too
arduous in C++.


Tasks
1. Learn about how to implement Stochastic Superoptimization from (3). Decide if
   we want to implement this within Binaryen or make it standalone. (February
   16)
2. Choose optimization criteria and encode in a loss function. (February 20)
3. Create test programs, some in Wasm some in languages which can compile to
   Wasm, and define the optimizations we want to see happen for each. Examples
   should contain both obvious optimizations and ones which normal optimization
   passes could not reach. (February 23)
4. If Binaryen was chosen, learn how to implement passes within Binaryen. If
   Binaryen was not chosen, develop the code needed to manipulate Wasm within an
   optimizer. Implement Metropolis-Hastings algorithm to perform MCMC sampling.
   (March 2)
5. Create an exact equality function for use in MCMC. (March 9)
6. Create an approximate equality function for use in MCMC. (March 14)
7. Implement stochastic superoptimization without a synthesis phase on a subset
   of the example problems. Write the interim project report. (March 28)
8. Add a synthesis phase to the superoptimizer. (April 4)
9. Test superoptimizer on large programs and fix any bugs that come up. Write
   the second interim report. (April 18)
10. Write the final report and submit it. (April 30)

Task Distribution

Magilan: Implement stochastic superoptimization without synthesis. Execute
small-scale tests. Develop exact and approximate equality functions. Work on
Metropolis-Hastings Method. Contribute to synthesis. Develop some test programs.
Test real-world codebases.

Hunter: Evaluate Binaryen for suitability or develop Wasm handling code in
Python. Develop loss function for size and compressed size. Work on
Metropolis-Hastings Method. Contribute to synthesis. Develop some test programs.
Test real-world codebases.


References
1. Liu, Zhibo, et al. "Exploring missed optimizations in webassembly
   optimizers." Proceedings of the 32nd ACM SIGSOFT International Symposium on
   Software Testing and Analysis. 2023.

2. Massalin, Henry. "Superoptimizer: a look at the smallest program." ACM
   SIGARCH Computer Architecture News 15.5 (1987): 122-126.

3. Schkufza, Eric, Rahul Sharma, and Alex Aiken. "Stochastic superoptimization."
   ACM SIGARCH Computer Architecture News 41.1 (2013): 305-316.

4. Bartell, Sean. Optimizing whole programs for code size. Diss. University of
   Illinois at Urbana-Champaign, 2021.

5. Sasnauskas, Raimondas, et al. "Souper: A synthesizing superoptimizer." arXiv
   preprint arXiv:1711.04422 (2017).

6. Zheng, Lianmin, et al. "Ansor: Generating {High-Performance} tensor programs
   for deep learning." 14th USENIX symposium on operating systems design and
   implementation (OSDI 20). 2020.

7. Cabrera Arteaga, Javier, et al. "Superoptimization of WebAssembly bytecode."
   Companion Proceedings of the 4th International Conference on Art, Science,
   and Engineering of Programming. 2020.

8. Sprokholt, Dennis G. Superoptimization of WebAssembly Process Graphs. MS
   thesis. 2021.

9. https://github.com/WebAssembly/binaryen/tree/main/src/passes

10. Abate, Alessandro, et al. "Counterexample guided inductive synthesis modulo
    theories." International Conference on Computer Aided Verification. Cham:
    Springer International Publishing, 2018.
