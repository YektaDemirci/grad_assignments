"""
Yekta Demirci
20837581
ECE606, F'19, Assignment 1, Problem 3(b)
Skeleton solution file.
"""

import random
"""
You are not allowed to import anything else
"""


def f(Val,m,randomSum):
        
    return (Val+randomSum) % m


def avgfalsepos(n,k,m):
    """
    You need to implement this method.
    You are certainly allowed to define any subroutines you want
    above this method in this file. You are not, however, allowed
    to import any more packages than already imported above.
    """
    
    falsePos=0
    
    numbTrial=100
    
    for trial in range(0,numbTrial):
    
        B = [0 for dummy in range(m)]
        
        for temp in range(0,k):
           B [ random.randrange(1, m, 1) ] = 1
           
        for temp in range(0,n-k):
        
            if B[ random.randrange(1, m, 1) ] == 1:
                falsePos = falsePos +1
            
            else:    
                continue
        
    return (falsePos / numbTrial)
        

    




    
    
