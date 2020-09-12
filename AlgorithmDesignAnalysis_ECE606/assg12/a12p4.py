"""
ECE606, F'19, Assignment 12, Problem 4
Skeleton solution file.
"""

import math # for things like log() and pow()


"""
You are not allowed to import anything else.
You need to implement the following method. You are
allowed to define whatever subroutines you like to
structure your code.
"""

def buildBST(A):
    dummyA=[]
    for i in A:
        dummyA.append(i)
    dummyA.sort()
    tree=sub(dummyA,A,0)


def sub(A,tree,idx):
    """
    You need to implement this method
    """
    #print(A)    
    
    length=len(A)
    
    if length==1:
        tree[idx]=A[0]
        return tree
        
    elif length==2:
        tree[idx]=A[1]
        tree=sub([A[0]],tree,(2*(idx+1)-1))
    
    else:
        depth=math.floor(math.log(length,2))
        
        upper=(2**(depth))-1    
        
        lastDepth=(length-upper)
        
        rightUpper=int((upper-1)/2)
        
        
        if( lastDepth > (2**(depth-1)) ):
            lastDepthLeft=(2**(depth-1))
            lastDepthRight=lastDepth-(2**(depth-1))
            
            
        else:
            lastDepthLeft=lastDepth
            lastDepthRight=0
            
        pivot=rightUpper+lastDepthRight            
        lastIndx=length-1
        root=lastIndx-pivot
        tree[idx]=A[root]
        
        if(len(A[:root])>0):
            tree=sub(A[:root],tree,(2*(idx+1)-1))
        if(len(A[root+1:])>0):
            tree=sub(A[root+1:],tree,(2*(idx+1)))
    
    return tree
