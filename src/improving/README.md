## improving

This section will try to describe improvements upon control flow flattening in a general sense and in relation to the toy implementation.

The most important aspect of any obfuscation technique is to generate **non-deterministic** output. The program _result_ should very much still be deterministic but the generated code mustn't be, or you risk an attacker cracking _all_ your programs. A simple solution is to sprinkle some **randomness** whenever possible. Some candidates are *case ordering*, *block layout* and *successor computation*.

 - scramble case order
 - split (large) basic blocks
 - duplicate blocks
 - dummy (bogus) blocks
 - opaque predicates
 - dispatcher obfuscation
 - sub-level dispatchers

### from the environment

A smartphone paltform can improve on control flow obfuscation by recompiling/retransforming the application sent to each user, such that each user ends up with a somewhat different binary.

### opaque predicates

As discussed previously in the section regarding **code lifting**, this obfuscation method can be used to introduce (useless) state obfuscation which is hard to both identify and undo. Consider this example:

```c
if (true) {
    return 0;
} else {
    return 1;
}
```

The first branch will always be taken but a standard control flow graph doesn't show that. An analyser must first inspect the branch statement and try to simplify it before the control flow graph can be reduced. Opaque predicates try to make this analysis as difficult as possible.

Some implementation methods use arithmetic (in)equalities to generate the predicate but these (in)equalities are difficult to construct and, once known, trivial to identify and simplify. From this problem ? to have the predicate masquerade as legit instructions.