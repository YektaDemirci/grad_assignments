"""
ECE606, F'19, Assignment 3, Problem 4
Skeleton solution file.
"""

import random

"""
You are not allowed to import anything else.
You need to implement the following method. You are
allowed to define whatever subroutines you like to
structure your code.
"""

def median(lst):
    sortedLst = sorted(lst)
    lstLen = len(lst)
    index = (lstLen - 1) // 2

    if (lstLen % 2):
        return sortedLst[index]
    else:
        return (sortedLst[index] + sortedLst[index + 1])/2.0


def med(A):
    randomVals=random.sample(A,len(A))
    counter=0
    medianIs=median(A)
    while True:
        i=random.choice(randomVals)
        if medianIs==i:
            return counter
        del randomVals[randomVals.index(i)]
        counter=counter+1
    
def avgtrials(A):
    """
    You need to implement this method.
    The input A is a list of distinct integers,
    whose size is an odd number.
    """
    val=0
    iteration=100
    for j in range(0,iteration):
        new=med(A)
        val=val+new
        
    return val/iteration
