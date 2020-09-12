### Explanation
Devise an algorithm and code it in Python 3 for the following problem.
Inputs: (i) hc 0 , c 1 , . . . , c k−1 i, a sequence of coin values with c 0 = 1 and c i−1 ``<`` c i for all
i = 1, . . . , k − 1, (ii) hn 0 , n 1 , . . . , n k−1 i, the number of coins we have of each value, and, (iii)
1a, an amount for which we seek to make change. All the numbers are non-negative integers.
Output: the fewest number of coins that amount to a, or ∞ (float(’inf’) in Python 3) if
we are unable to hand out coins that add up to a.
For example, if (i) is h1, 5, 10, 25i, (ii) is h6, 3, 5, 1i and (iii) is 62, the output is 7, because we
would hand out 1 × 25 + 3 × 10 + 1 × 5 + 2 × 1. If (ii) is h1, 8, 15, 2i instead, the output is ∞.
