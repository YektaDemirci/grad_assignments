### Explanation
(3)\
Consider the following randomized algorithm to find the median in an input array
A[1, . . . , n] of distinct integers, where n is odd.
Perform an expected-case analysis for the number of times Line (3) is executed.

Coding task:\
Empirically confirm your analytical solution to Problem (3) above via
a python program. More specifically, first implement the above algorithm Med, but include
in it the ability to record the number of executations of Line (3) using a counter variable. On
each input, which is a list of an odd number of distinct integers, invoke Med 100 times, and
compute the average number of executions of Line (3) across the 100 invocations of Med.
That is to be your return value from the method avgtrials.
