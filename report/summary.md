## Summary

Proof of concept demonstrates that SHA-1 with dedicated AArch64 C instrinsics
can be executed via Wasm intrisics in Wasmtime at 1.3x native performance.

Potential issues revealed by this experiment:

* Semantics mismatches between C, Wasm and target instructions can eliminate
  performace gains if not handled carefully.
* Peak performance may be limited by relatively simple optimizers in JIT engines
* Supporting large sets of intrinsics in Wasm JITs would require careful
  engineering

## Application

I implemented a proof-of-concept for a representative use case, namely the SHA-1
hash algorithm using the Cryptographic Extension on AArch64.  The prototype
demonstrates how C code written against ARM's C intrinsics API can be executed
both natively and via Wasm.  Wasm execution is achieved with a Wasm AArch64
intrinsics C API layer that serves as a drop-in replacement for native intrinsic
header files".  In addition, I have a fork of Wasmtime with support for
intrinsic calls for a select group of AArch64 instructions. The end result is
SHA-1 execution via Wasm with intrinsics at 1.3x native AArch64 performance.

To give a feel for the implementation, four rounds of the SHA-1 compression
function in C with AArch64 intrinsics are:

```c
// Rounds 28-31
e0 = vsha1h_u32(vgetq_lane_u32(abcd, 0));
abcd = vsha1pq_u32(abcd, e1, t1);
t1 = vaddq_u32(m1, vdupq_n_u32(K1));
m2 = vsha1su1q_u32(m2, m1);
m3 = vsha1su0q_u32(m3, m0, m1);
```

These intrinsics are defined in `arm_neon.h`. The proof of concept provides an
alternate `wasm_arm_neon.{h,c}` that the C code can be compiled against
unchanged, and pure Wasm implementations that would work on any platform.
However, when executed under the modified Wasmtime calls to instrinsic functions
such as `vsha1h_u32` are recognized and compiled directly to the corresponding
hardware instructions like `SHA1H`.

## Lessons

Some lessons from this proof-of-concept, with the caveat that they may not
generalize to other intrinsics domains:

_Challenge of Semantics Mismatches_
Compilation via intrinsics passes through many layers: C intrinsics API, engine
intrinsics API, Wasm operators, CLIF IR and machine code representation. Each of
these has their own semantics and value representations.  Earlier stages of this
project showed that if not handled correctly, semantics mismatches can eliminate
any performance you might hope to gain from the intrinsics calls.  Specifically,
in this case some of the special SHA-1 instructions have the oddity that they
accept `s<n>` registers which are scalar 32-bit values in the low bits of vector
registers. Wasm engines naturally want to store 32-bit integers in general
purpose registers, therefore without careful handling the intrisincs calls are
surrounded by redundant register moves between vector and general purpose
register files (with a significant performance penalty).  These details are
important when the entire goal of hardware intrinsics in Wasm is reaching
near-native performance.  We might hope that the idiosyncracies of the SHA-1
instruction set are not widespread. However, it seems possible that this broader
problem of semantics mismatches could rear its head in other cases, for example
when attempting to use wide vector types (e.g.  Intel AVX-512) that do not have
Wasm equivalents.

_Significance of the Intrinsics API_
The design of the C API layer was critical in achieving near native performance.
Specifically, it should be designed to limit the number of intrinsics required
in the engine, and intrinsics offered by the engine should be as close as
possible to the machine instructions. Therefore:

* Implement C layer intrinsics as existing Wasm operators wherever possible. For
  example, the AArch64 intrinsic `vdupq_n_u32` can just be implemented as
  `i32x4.splat` (or `wasm_u32x4_splat` in C) without the need to add an
  intrinsic to the engine. Importantly, this also improves the ability of the
  AOT compiler to optimize code around the intrinsics.
* Engine intrinsics API should be as close as possible to machine instructions.
  This makes the engine work essentially a passthrough, and limits the
  optimizations required from the JIT. As a concrete example, the `vsha1h_u32`
  intrinsic takes and returns a `uint32_t`. However, it is best if the C layer
  maps to an internal `__intrinsic_vsha1h_u32` version that takes and returns a
  `v128`, since these match the underlying `SHA1H` instruction more closely.

_Importance of Accompanying Optimizations._
The first version of SHA-1 via Wasm instrinsics had poor performance (3.2x
native), showing that merely mapping to the right machine instructions is not
enough. Supporting optimization passes are critical.  In the SHA-1 case, it was
crucial to eliminate redundant moves between register classes, but it is
reasonable to expect instances of this problem for other classes of intrinsics.
Optimizing JITs are designed for compile speed and therefore have a much more
limited set of optimizations than a full AOT compiler. In this case we were able
to work around missing Cranelift JIT optimizations by moving the problem to the
AOT compilation layer, however it is not clear that would always be possible.
Indeed, the remaining approximately 30% overhead over native execution may be a
difficult gap to close, given the lack of optimizations such as instruction
scheduling in JIT compilers. Overall, we might expect that Wasm intrinsics
performance would be limited by JIT compiler optimization capabilities.

_Engineering Aspects._
The fork of Wasmtime for this project was modified with this proof-of-concept in
mind. While the engineering was reasonable, the approach taken is not one that
would scale to adding hundreds or thousands of intrinsic calls. At the time of
writing, the ARM intrinsics database contains 12,855 function calls, with 4,344
in the Neon instruction set extension. A full production-grade version of the
Wasm intrinsic header library and accompanying engine support would be a
substantial undertaking.  You would almost certainly want automation and
code-generation involved, but also certain parts of the engine integration would
not scale well.  The current hand-written assembler would need to support many
more instructions.  You also probably would not want to actually extend the
Engine's IR to support every intrinsic either, but instead perhaps support an
explicit passthrough or intrinsic IR node that would effectively perform a
trivial lowering to a wrapped machine instruction. None of these engineering
challenges are intractable, but they would need careful thought.

Full Report: https://mmcloughlin.com/hwwasm/hwwasm.pdf
