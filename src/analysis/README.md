[<~ previous](../algorithm)
[next ~>](../attacking)

# 4 - analysis

## strengths

 - with sophisticated dispatcher constructions the obfuscated control flow is _very_ different from the original
 - possibility to achieve decent protection against dynamic analysis (e.g. using opaque prngs)
 - simple base idea/algorithm that is easy to extend and improve
 - obfuscation improves with other obfuscation techniques: inlining, substitution

## weaknesses
An obvious weakness of control flow flattening is the **performance** penalty caused by **cache locality** and other memory-related problems. While many compilers try to optimize the branches of a switch construction the flattened graph is by design the worst possible case: a shitload of unique cases.

Most compilers will optimize and generate a binary tree comparison procedure, with O(log N) complexity, for finding the correct case and this helps tremendously compared to standard linear probing, which is O(N). In fact most modern compilers will generate a combination of the two. This limits the penalty incurred by increasing the number of cases, which is required by some hardening techniques.

Each branch in the dispatcher takes up a slot in the branch predictor (of the CPU), which is guaranteed to be useless since the branch predictor _cannot_ predict the correct branch, unless we have a constant value obfuscated as an opaque predicate. Thus the generated branch will always have a mispredict overhead associated with it.

 - algorithm used to flatten is easy to undo if known (write optimization/deobfuscate pass for each obfuscation step)
 - cannot _guarantee_ protection against dynamic analysis (branches, memory access) on its own
 - resulting graph is _obviously_ obfuscated and easy to identify --> code can be lifted/hoisted (depends on inlining/fusing)