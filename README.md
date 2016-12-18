# Serialization Experiments Lain

A schema-based, statically typed serialization DSL for C++.

## What is this?

A data conversion library that tries to address the issue of serializing C++ data structures into arbitrary formats and vice-versa.

Four things make it stand out:

### Pure C++

Other C++ libraries that serialize data according to a fixed message schema such as the Protocol Buffers, Cap'n Proto and Bond use their own IDL and generate C++ code from it. In contrast, Lain brings its own DSL and heavily uses templates. Its structure schemas are written directly in C++.

### Static typing

The library uses the C++ type system as much as possible to ensure that the data structures, the types they use and the input/output formats are compatible with each other. As a rule of thumb, if it compiles, it will work at runtime as long as the processed data is valid.

### Complete control over data formats

Lain allows fine-grained control over the input and output data formats. It can be extended to support any existing one that can be represented as a byte stream.

### Minimal

Header-only, no dependencies but the C++ standard library.

