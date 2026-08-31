# Lox Interpreter (tree-walker + bytecode VM) in C++

this is my implementation of Lox, the language from the book
*Crafting Interpreters*. I built it in two parts:

1. A tree-walking interpreter 
2. A bytecode compiler + VM + Hashmap Imp

I tried to use C++ features where it made sense (classes, std::vector,
templates, etc) instead of the book's style since I used a diff lamnguage. So if you're comparing this to the book, some stuff will look
different even though the overall design (bytecode format, VM
loop, GC algorithm) is the same.

## how to build and run it

You need g++ with C++17 support. From the project root:

```
make
./compiler path/to/script.lox
```

If you want a clean rebuild (useful if something's acting weird and you
suspect stale object files):
```
make clean
make
```

There's a `#define DEBUG_STRESS_GC` near the top of `Memory.hpp` that
forces the garbage collector to run on every single
allocation instead of only when memory usage crosses a threshold. It's
there for debugging GC bugs. comment it out for normal use
since it's really slow.

## project structure (roughly)

- `Scanner` - turns source text into tokens
- `Compiler` - parses tokens and emits bytecode directly (single pass,
  no separate AST like the tree-walker has)
- `Chunk` - holds a function's bytecode, constants, and line info for
  error messages
- `VM` - the actual bytecode interpreter loop, owns the stack, globals,
  call frames
- `Table` (in `hashmap/`) - my own hash table implementation, used for
  globals and string interning
- `Memory` / `GarbageCollector` - the GC, owned by `VM`
- `Obj` types (`ObjString`, `LoxFunction`, `ObjClosure`, `LoxClass`,
  `LoxInstance`, `ObjUpvalue`, `ObjNative`) - all the heap-allocated
  Lox value types

## what Lox features work

- numbers, strings, bools, nil
- arithmetic + comparisons (`+ - * / == != < > <= >=`)
- string concatenation with `+` (string + string only, no auto
  converting numbers to strings)
- `and` / `or` with short circuiting
- variables, both global and block-scoped locals
- `if`/`else`, `while`, `for`
- functions, recursion, closures with upvalues
- classes, instances, methods (implemented but I didn't test this part
  as heavily as the rest, so there could be bugs I haven't found)
- a native `clock()` function for timing stuff

## the hash table

I wrote my own instead of just using `std::unordered_map`. 
It's open addressing with linear probing, uses a sentinel `TOMBSTONE` 
pointer for deleted entries (so probing doesn't break after a delete), 
and grows/rehashes itself once it hits a load factor of 0.75. Used internally for the VM's 
global variables and for string interning (so equal string literals share one `ObjString`
instead of allocating duplicates).

## the garbage collector

How it works, roughly:
- Every object allocation goes through one templated function
  (`allocateObject<T>()`), which links the new object onto an internal
  linked list of every object ever allocated, and tracks total bytes
  allocated so it knows when to trigger a collection.
- When a collection runs, it first marks every "root" - objs the
  program can reach directly, like the VM's value stack, active call
  frames, global variables, and open upvalues.
- From there it traces outward (an object's fields/references get
  marked too) using an explicit stack of "gray" objects instead of
  actual C++ recursion, so it can't stack overflow on deeply
  nested data.
- Anything that never got marked gets swept (deleted) at the end.
- The string interning table is deliberately NOT treated as a root, so strings 
   nothing is using anymore can actually get collected instead of being kept alive forever just because they're
  cached. This needed an extra cleanup step to strip dead entries out
  of that table after tracing but before sweeping frees the memory,
  otherwise you'd be left with dangling pointers in the table.
- I also had to add a "pinned roots" mechanism that isn't really in the
  book, because my compiler and VM are more separated from each other.
  Things like a function that's still in the middle of being compiled, or a 
  closure that's half-built while it's capturing its upvalues aren't reachable through any of the normal
  roots yet so without this feature, a badly timed collection could delete them while they're still being built. 
  I found this out by turning on the stress-test flag mentioned above and
  watching everything break a lot. Took a while to track down but was a
  good learning experience honestly.

## known limitations / stuff I know isn't done

- No number -> string coercion, so `"count: " + 5` will error instead
  of working. This actually matches base Lox from the book, not a bug.
- Classes/instances are implemented and the GC does trace through them
  correctly, but I didn't write nearly as many tests for that part
  compared to functions/closures, so treat that part as "probably
  works" rather than "definitely works."
- Error messages are okay but not amazing - line numbers are there but
  don't expect anything fancy.

## running some quick tests

Just paste these into a file and run it with the cmnd ./compiler "insert filename here":

```
// basic stuff
var x = 5;
print x + 10;
if (x == 5) { print "yes"; } else { print "no"; }
for (var i = 0; i < 3; i = i + 1) { print i; }
```

```
// closures - each counter should track its own count independently
func makeCounter() {
    var count = 0;
    func inc() {
        count = count + 1;
        return count;
    }
    return inc;
}
var a = makeCounter();
var b = makeCounter();
print a(); // 1
print a(); // 2
print b(); // 1, not 3 - it's a separate closure
```

```
// hammer the allocator to force a bunch of GC cycles
for (var i = 0; i < 5000; i = i + 1) {
    var s = "junk" + "junk";
}
print "still alive"; // if this prints, nothing got freed too early
```