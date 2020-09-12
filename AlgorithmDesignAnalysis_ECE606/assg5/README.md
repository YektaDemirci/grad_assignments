### Explanation
Consider the following two versions of change-making. Both take the same input: (i)
hc 0 , c 1 , . . . , c k−1 i, k coin values where c 0 = 1, every c i ∈ N and c i ``<`` c i+1 , and, (ii) a ∈ N.In Version 1 of the problem, the output is the minimum number of coins that amount to a.
E.g., on input h1, 5, 10, 25i, 162, we get as output 9. In Version 2 of the problem, the output
is a tuple of size k, where each entry is the number of coins of that value, such that the coins
together make up the amount a. For the example above, the output would be h2, 0, 1, 6i.
Note that an output in Version 1 is unique, but not necessarily in Version 2. \
Given an oracle for Version 1, call it MinNumCoins, devise a polynomial-time
algorithm for Version 2. Write pseudo-code for it, explain briefly how it works, and why.
1An “oracle” is a tamper-resistant black-box that, given input that it understands, spits
out the output in time Θ(1). Its behaviour is undefined if it is given input that it does
not understand. \
Realize your algorithm as Python 3 code in a5p3b.py, as the
method mincoinslist. The oracle is provided to you as byte code, and already imported
via the import line. The arguments to your method are (i) a list, C, e.g., [1, 5, 10,
25], and, (ii) a natural number a. You are to return a list whose length is the same as
C.
