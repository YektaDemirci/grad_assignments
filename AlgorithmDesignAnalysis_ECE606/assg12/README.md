### Explanation
Consider the following encoding of a Binary Search Tree as an array,
A[0, . . . , n − 1]. The root is stored at A[0], the left child of a node at index i is stored at index
2(i + 1) − 1, and its right child at index 2(i + 1).
Given input an array A[0, . . . , n−1] of n distinct natural numbers, devise and code a O(n lg n)-
time algorithm that converts A in to a Binary Search Tree as described above. For example,
given as input A = [5, 3, 4, 2, 1, 6], up on return, A should be [4, 2, 6, 1, 3, 5].
