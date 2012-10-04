tpmdos
======

TPM is a hardware approach which provides authentication to upper-layer system components [2]. Nowadays, about 300 million TPM chips have been deployed to worldwide computers [3]. Inspired by its functionality and broad availability, various security mechanisms have been proposed by using TPM as their Trusted Computing Base (TCB) [1, 4, 5, 9, 10]. Furthermore, many existing implementations store certain private information inside its NVRAM [1, 4]. When being initialized or responding to authentication/secrecy service requests, there will be certain I/O operations to the built-in NVRAM [1, 4, 10].

Thus, it is crucial to evaluate the persistence of the NVRAM since an easy-breaking NVRAM renders the whole TCB vulnerable. Unfortunately, existing NVRAM have certain limitation on their writing cycles, which is around 100,000 operations. As a result, writing once per second will exhaust the NVRAM in less than 28 hours [1, 6]. Besides, each write operation to NVRAM takes about 33.9-82.4ms [6]. Therefore, NVRAM could get worn out in even shorter time.

In reality, vendors tried to ameliorate this issue by various methods, such as wear leveling [7] and optimization carried out by manufactures [8]. However, no work has been done to evaluate the limit of real TPM NVRAM writing operations, nor their error models. To the best of our knowledge, there is no precise description on the error model of NVRAM. It is not known whether they would return one specific error codes, or vendor specific error codes. Meanwhile, the behavior caused by writing cycle exhaustion  is not defined in the public specification.

Our project will evaluate the persistence of NVRAMs from popular commercial TPMs. We expect it easy to break according to our result. We will also demonstrate that it is possible to modify TPM software stack to eliminate this undesirable vulnerability entirely. 

[1] Bryan Parno, Jacob R. Lorch, John R. Douceur, James Mickens, and Jonathan M. McCune: Memoir: Practical state continuity for protected modules. In: Proceedings of the IEEE Symposium on Security and Privacy (2011).
[2] Trusted Computing Group: How to Use the TPM: A Guide to Hardware-Based Endpoint Security
[3] Jared Schmitz, Jason Loew, Jesse Elwell, Dmitry Ponomarev, Nael Abu-Ghazaleh: TPM-SIM: A Framework for Performance Evaluation of Trusted Platform Modules
[4] Jonathan M. McCune, Bryan Parno, Adrian Perrig, Michael K. Reitery, Hiroshi Isozakiy. An Execution Infrastructure for TCB Minimization, 2008
[5] Jonathan M. McCune, Ning Qu, Yanlin Li Anupam Datta, Virgil D. Gligor, Adrian Perrig. TrustVisor: Efficient TCB Reduction and Attestation, 2011
[6] Bryan Parno, Jonathan M. McCune, Adrian Perrig. Bootstrapping Trust in Modern Computers, 2011
[7] Wear Leveling. http://en.wikipedia.org/wiki/Wear_leveling.
[8] The Intel Safer Computing Initiative, Chap. 8
[9] Luis F. G. Sarmenta, Marten van Dijk, Charles W. O¡¯Donnell, Jonathan Rhodes, and Srinivas Devadas, Virtual Monotonic Counters and Count-Limited Objects using a TPM without a Trusted OS, 2006
[10] Marten Van Dijk, Luis F. G. Sarmenta , Jonathan Rhodes , Srinivas Devadas. Securing Shared Untrusted Storage by using TPM 1.2 Without Requiring a Trusted OS, 2007

How to run
=====================
1. Install git
2. make [debug] [all]
3. The results are available at (git clone https://tpmdos@code.google.com/p/tpm-data/ )
4. You should use different hostname. because we use it to identify different results from different machines.