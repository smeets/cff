
[<~ previous](https://github.com/smeets/cff)
[next ~>](obfuscation)

# control flow flattening

## 0 - prelude

*Why do we obfuscate code? Is it good? Is it bad? Is it reasonable to obfuscate when SGX and ARM TrustZone exists, if so why? What is control flow flattening? Where does it fit in? How is it implemented? What are known weaknesses/attacks? Possible improvements?*

These are some of the underlying questions and thoughts this article aims to answer and discuss. As with many other things, there isn't a 100%-always-guaranteed-perfectly-correct-forever-and-ever answer. Obfuscation as a solution, however, is often frowned upon and it is hard to argue that it is implemented for the benefit of the consumer/end user. Instead, obfuscation is one of the tools business can use to protect their intellectual property (IP), and, depending on the implementation, may even result in a worse experience for the consumer/end user. While the specific reasonings why obfuscation is used are unknown to me, the general principle is clear: *to protect the product in a hostile environment*.

 > So what is this hostile environment, you ask? It is your phone, your computer, the wireless network in your home, the wonderful thing we call the internet, the servers that power your services. Generally, it is all the environments which the company does not fully control. One example are games running on your phone, console or computer. Big game publishers are using varying forms of digital rights management (DRM) software to protect their game (IP) against crackers and piracy.

To make some discussions easier and more informative these environments will be separated into *phone*, *pc* and *servers*. A short overview of protective measures and known attacks will be presented for each one of these environments.

# 1 - introduction

Apply a flattening transform on the control flow that hides the original flow while keeping a strict equivalence in terms of input/output. The core assumptions and observations can be expressed as:

 - The *source code* is compiled into *machine instructions* by the compiler
 - Machine *instructions* can be grouped into *blocks* by jumps & entry points
 - These *blocks* are chained together to form *procedures*
 - A *program* consists of one or more *procedures*
 - The control flow graph is a graph which describes all possible execution paths of a particular *program*, *procedure* or *block*
 - Static analysis of the control flow graph is constructed by finding branches (implicit & explicit) between *blocks*
 - The *flattening transform* essentially combines a longer chain of branches (a->b->c->d) into a looping state machine (a->{b,c,d}), keeping the original order but preventing static analysis from discovering it by computing the next block during program execution
 - It is *allowed* to do anything imaginable as long as the input/output equivalence holds, e.g. the state machine can be arbitrarily complex


Layout of this document.

 - section 2: [an intro to obfuscation](obfuscation)
 - section 3: [algorithm](algorithm)
 - section 4: [analysis](analysis)
 - section 6: [attacking cff](attacking)
 - section 7: [improving cff](improving)
 - section 8: [available tools](tooling)

## 10 - summary

In the end, (software) obfuscation is used simply because there isn't a better alternative available, yet.


## 11 - links

 - [Obfuscating C++ programs via control flow flattening, T László and A Kiss, 2009](http://ac.inf.elte.hu/Vol_030_2009/003.pdf)
