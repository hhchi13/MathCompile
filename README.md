MathCompile is a package that translates *Wolfram Language* functions into C++ code and generate dynamic libraries that can be called in *Wolfram Language*.

MathCompile is focused on improving the availability of functional programming and covering the functionalities provided by the built-in compiler. Currently, MathCompile supports over 240 functions and over 20 constants; see the [wiki page](https://github.com/njpipeorgan/MathCompile/wiki/Compilable-Functions) for the full list. 

# Prerequisites

To compile *Wolfram Language* functions to C++ code, *Wolfram Mathematica* 11 or higher is required. Starting from version 12, `NumericArray` types are introduced and can be compiled. An alternative to *Mathematica* is [*Wolfram Engine*](https://www.wolfram.com/engine/), which is freely available to developers.

To compile functions to binary, a C++ compiler supporting C++17 standard is required. See this [wiki page](https://github.com/njpipeorgan/MathCompile/wiki/Prerequisites-for-C-Compiler) for the list of all supported C++ compilers, their availabilities, and how to use them. 

# Documentation

To know more about MathCompile, see the [Wiki](https://github.com/njpipeorgan/MathCompile/wiki).

To learn about how to install and use MathCompile, see the [Guide](https://github.com/njpipeorgan/MathCompile/wiki/Guide).

# Basic usage

First, load the package:
```
<<MathCompile`
```
If you have a supported C++ compiler, compile a function to binary using `CompileToBinary`:
```
cf=CompileToBinary[
  Function[{Typed[x,{Integer,1}]},Apply[Times,x]]
]
```
Use this compiled function just like a normal *Wolfram Language* funcion:
```
cf[{1,2,3,4}]    (* gives 24 *)
```

Without a C++ compiler, you can still check the intermediate C++ code using `CompileToCode`:
```
CompileToCode[
  Function[{Typed[x,{Integer,1}]},Apply[Times,x]]
]
```
The result is a C++ function named `main_function`:
```c++
auto main_function(const wl::ndarray<int64_t, 1>& v35) {
    return wl::val(wl::apply(WL_FUNCTION(wl::times), WL_PASS(v35)));
}
```
The disassembly generated by GCC 7.4 and Clang 5.0 for this function is available through [Compiler Explorer](https://godbolt.org/z/tElm9M).
