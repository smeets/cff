
[<~ previous](https://github.com/smeets/cff) -- [next ~>](cff)

# control flow flattening

## prelude

*Why do we obfuscate code? Is it good? Is it bad? Is it reasonable to obfuscate when SGX and ARM TrustZone exists, if so why? What is control flow flattening? Where does it fit in? How is it implemented? What are known weaknesses/attacks? Possible improvements?*

These are some of the underlying questions and thoughts this article aims to answer and discuss. As with many other things, there isn't a 100%-always-guaranteed-perfectly-correct-forever-and-ever answer. Obfuscation as a solution, however, is often frowned upon and it is hard to argue that it is implemented for the benefit of the consumer/end user. Instead, obfuscation is one of the tools business can use to protect their intellectual property (IP), and, depending on the implementation, may even result in a worse experience for the consumer/end user. While the specific reasonings why obfuscation is used are unknown to me, the general principle is clear: *to protect the product in a hostile environment*.

 > So what is this hostile environment, you ask? It is your phone, your computer, the wireless network in your home, the wonderful thing we call the internet, the servers that power your services. Generally, it is all the environments which the company does not fully control. One example are games running on your phone, console or computer. Big game publishers are using varying forms of digital rights management (DRM) software to protect their game (IP) against crackers and piracy.

To make some discussions easier and more informative these environments will be separated into *phone*, *pc* and *servers*. A short overview of protective measures and known attacks will be presented for each one of these environments.

## a brief introduction to obfuscation

Obfuscation is often regarded as a subpar solution to a difficult problem. While its reputation isn't necessarily undue it is important to recognise that obfuscation is a means to an end, whatever that is, with both poor and solid implementations.

As with security, obfuscation is outrageously hard to get right and simple mistakes and oversights can be devastating. Once the product is shipped, that's it - it's out there for the world to see and (ab)use.

Track record of obfuscation techniques?

### on your devices
(smartphone, console)

 - effect of Intel SGX, AMD ??, ARM TrustZone
 - vendor-controlled system (apple fairplay, google ??, microsoft ??)
 - exploitable --> rootable --> hostile
 - give your program to apple & trust them

### on your computer

 - effect of Intel SGX, AMD ??, ARM TrustZone
 - every man for himself
 - 3rd party protection: Denuvo, VMProtect
 - give your program to end user & trust them

### on someone else's server

 - effect of Intel SGX, AMD ??, ARM TrustZone
 - give your program to server owner & trust them

Next up:

 - section 1: [control flow flattening](cff)
 - section 2: [algorithm](algorithm)
 - section 3: [analysis](analysis)
 - section 4: [attacking cff](attacking)
 - section 5: [improving cff](improving)
 - section 6: [available tools](tooling)

## summary

In the end, (software) obfuscation is used simply because there isn't a better alternative available, yet.


## links

 - [Obfuscating C++ programs via control flow flattening, T László and A Kiss, 2009](http://ac.inf.elte.hu/Vol_030_2009/003.pdf)
