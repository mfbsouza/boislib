# boislib: extending the C language ecosystem

[![Coverage Status
](https://coveralls.io/repos/github/mfbsouza/boislib/badge.svg?branch=main)
](https://coveralls.io/github/mfbsouza/boislib?branch=main)

boislib is a collection of libraries with useful functions, data structures
and algorithms to extended the default C ecosystem.

## Why?

As a embedded developer, I sometimes find myself in the need of common well
know data structures and algorithms that usually comes in many libraries
that aren't always available in all platform or easily portable.
That can be a real pain especially when doing firmware and bare-metal work.

The goal here is to provide implementation to those data structures and 
algorithms and also to extended and provide not such common ones, making
sure that the code can be, with zero headache, ported to most architectures
and systems.

## License

boislib is licensed under the MIT License, see [LICENSE](LICENSE) for more information.

## libraries

Quick summary of the libraries

### allocator.h

You give me a contiguous amount of memory, I give you dynamic memory
allocation! allocator implements dynamic memory management.

### circular_queue.h

You give me a contiguous amount of memory, I give a queue!
circular_queue implements a FIFO data structure.

## Contributions

Want to help out and submit your own library to the project? Feel free to!

Contributions can come in many forms. Please check out the [contributing
](CONTRIBUTING.md)
documented for more information.

## Dependencies

### C libraries

If your platform/libc has this, than you can compile to it!

- assert.h
- stdbool.h
- stddef.h
- stdint.h
- string.h

### Testing dependencies

- [Google Test](https://github.com/google/googletest) for unit testing
and mocking framework
