## Post-Quantum Cryptography 

The development of mature and performant quantum computing systems puts several conventional cryptographic systems under pressure; famously the widely used RSA 
cryptosystem. In the so-called "Post-Quantum" cryptographic era, different cryptosystem technologies. Qose is a library for severely resource constrained embedded 
environments (e.g., those in which even basic dynamic memory allocation is infeasible) which need secure cryptosystems into the forseeable future. The library is 
built by integrating the [hash-sigs](https://github.com/cisco/hash-sigs) LMS library and integrating it with a commercial quality COSE library called [t_cose](https://github.com/laurencelundblade/t_cose). 

## Contents 
  1. [Installation & Dependencies]() 
  2. [Background]()
  3. [Demo]()
  4. [Integrating with t_cose]()  
  5. [References]()
  
  
## Installation and Dependencies 

Qose depends on [OpenSSL](https://www.openssl.org/) the general-purpose tookit for cryptography and secure communication. Qose's core dependency is [Hash-Sigs](https://github.com/cisco/hash-sigs), a full-featured implementation of Leighton-Micali signatures [1] using hierarchical signature systems. Qose provides a simple API for interacting with the underlying hash-sigs implementation, intended to be built upon for integration in `t_cose` (see [Integrating with t_cose]()). 

Qose comes packaged with a simple (GNU) makefile whose compiler flags can be easily modified to point to the appropriate locations of your install of hash-sigs and OpenSSL. 

## Background 

## Demo 

## Integrating with t_cose 

## References 
  [1] [Leighton-Micali Hash-Based Signatures](https://datatracker.ietf.org/doc/html/rfc8554)
