"""
ECE606, F'19, Assignment 5, Problem 3b
Skeleton solution file.
"""
from a5p3bminnumcoins import minnumcoins
"""
You are not allowed to import anything else.
You need to implement the following method. You are
allowed to define whatever subroutines you like to
structure your code.
"""
def mincoinslist(coins,V):
    
    m=len(coins)
    coins.sort(reverse=True)
    res = subroutine(coins,V,m)
    coins.sort(reverse=False)
    fin=[0]*m
    
    for i in res:
        indx=coins.index(i)
        fin[indx]=fin[indx]+1
    return fin 



def subroutine(coins,V,m):
    table=[V+1]*(V+1)
    table[0]=0
    
    pi=[0]*(V+1)
    pi[0]=0
    
    for i in range(1,V+1):
        for j in range(0,m):
            if coins[j] <= i:
                dummy = table[i-coins[j]]
                if ( (dummy!= (V+1) ) and (dummy+1 < table[i]) ):
                    table[i]=dummy+1
                    pi[i]=coins[j]
    cont=V
    listo=[]
    while cont != 0:
        listo.append(pi[cont])
        cont=cont-pi[cont]
        
    return listo