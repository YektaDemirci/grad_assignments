### Explanation
A trie data structure is a particular kind of tree which exploits the fact
that strings we seek to store are in an underlying alphabet. For example, suppose our alphabet
is {0, 1, 2, 3}. And suppose we want to store the set of strings {010, 00, 0100, 210, 1231, 12}.
Assuming 0 ``<`` 1 ``<`` 2 ``<`` 3 and adopting the corresponding lexicographic ordering for the
strings, we could store those strings in a binary search tree that may look like the following. \
What you need to do you need to implement three methods.
* trieInsert(t, s) – inserts the string s in the alphabet {0, 1, 2, 3} into the trie t. If s is
already in t, then this method does nothing.
* trieDelete(t, s) – deletes the string s from the trie t. If s is not in t, then this method
does nothing.
* trieFind (t, s) – returns True if the string s is in t, and False otherwise. 

(My solution is between 5 and 7 lines of code for each of those three methods.)
Structure of a trie We model each node in the trie as a list of two items: (i) a list, and,
(ii) True or False. The list (i) has four items, each corresponding to each alphabet symbol.
We use an empty list [] for what we refer to as a Nil pointer above. Each entry in the list
(i) may itself be a trie node, i.e., a list of two items (i) and (ii). The boolean entry (ii) for a
node indicates whether this node is a terminal.
We adopt the convention that a trie t = [], i.e., an empty list, if and only if the trie t is
empty, i.e., has no strings in it. Thus, if t is [[[], [], [], []], True], then t is a trie
that comprises the empty string, which is often represented as , but in python would be ’’.
Note that [[[], [], [], []], False] is not a valid trie. More generally, every leaf node
should be a terminal. The trie [[[], [[[], [], [], []], True], [], []], False] con-
tains only one string, ‘1.’ The trie [[[[[], [], [], []], True], [[[[[], [], [], []],
True], [], [], []], False], [], []], True] contains three strings: , ‘0’ and ‘10.’ The
trie from the above picture, which encodes the set of strings {010, 00, 0100, 210, 1231, 12}, is
in the tester.
