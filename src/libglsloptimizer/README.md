# GLSL optimizer

> NOTE: This is an embedded fork of `aras-p/glsl-optimizer` that was slightly modified
> for emscripten and is included in this repository for ease-of-use.
> See [`aras-p/glsl-optimizer` on GitHub](https://github.com/aras-p/glsl-optimizer).

A C++ library that takes GLSL shaders, does some GPU-independent optimizations on them and outputs GLSL or Metal source back. Optimizations are function inlining, dead code removal, copy propagation, constant folding, constant propagation, arithmetic optimizations and so on.
