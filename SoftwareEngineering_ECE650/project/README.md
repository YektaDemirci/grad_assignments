### Explanation
* Quantitatively analyze your software for various kinds of inputs.
* Augment your code from Assignment 4 in the way that is decribed below.
	* Make it multithreaded. You should have at least 4 threads: one for I/O, and one each for the
different approaches to solve the minimum vertex cover problem.
	* Implement the following two additional ways to solve MIN-VERTEX-COVER, in addition
to the REDUCTION-TO-CNF-SAT approach you had in Assignment 4. (We will call your
approach from Assignment 4, CNF-SAT-VC.)
	* 1. Pick a vertex of highest degree (most incident edges). Add it to your vertex cover and
throw away all edges incident on that vertex. Repeat till no edges remain. We will call
this algorithm APPROX-VC-1.
	* 2. Pick an edge hu, vi, and add both u and v to your vertex cover. Throw away all edges
attached to u and v. Repeat till no edges remain. We will call this algorithm APPROX-
VC-2.

Yekta & Eric
