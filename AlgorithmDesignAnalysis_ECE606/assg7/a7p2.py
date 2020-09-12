"""
ECE606, F'19, Assignment 7, Problem 2
Skeleton solution file.
"""

"""
You are not allowed to import anything.
You need to implement the following method. You are
allowed to define whatever subroutines you like to
structure your code.
"""


def minncoins(C, N, a):
    """
    You need to implement this method.
    C is a list of coin values with len(C) > 0, C[0] = 1, C[i] < C[i+1]
    N is a list of number of each coin value
    a is an amount
    """
    ans=subroutine(C,a,N)
    print(ans)
    return ans

    

def subroutine(C,a,N):
    
    m=len(C)
    table=[a+1]*(a+1)
    table[0]=0
    
    pi=[0]*(a+1)
    pi[0]=0
    
    piCons=[]
    temp=[]
    for i in range(0,m):
        temp.append(N[i])
    piCons.append(temp)    
    
    for i in range(1,a+1):
        control=1
        for j in range(0,m):
            if C[j] <= i:
                dummy = table[i-C[j]]
                temp=[]
                for k in piCons[i-C[j]]:
                    temp.append(k)
                    
                if (dummy == (a+1) and control == 1):
                    control=0
                    pi[i]=C[j]
                    piCons.append(temp)
                elif (dummy == (a+1) and control == 0):
                    pi[i]=C[j]
                    del piCons[len(piCons)-1]
                    piCons.append(temp)
                else:   
                    if ( (dummy+1 < table[i]) and temp[j]>0 and control == 1):
                        control=0
                        table[i]=dummy+1
                        pi[i]=C[j]
                        temp[j]=temp[j]-1
                        piCons.append(temp)
                    elif ( (dummy+1 < table[i]) and temp[j]>0 and control == 0):
                        table[i]=dummy+1
                        pi[i]=C[j]
                        temp[j]=temp[j]-1
                        del piCons[len(piCons)-1]
                        piCons.append(temp)
                    elif ( (dummy+1 < table[i]) and temp[j]==0 and control == 1):
                        control=0
                        pi[i]=C[j]
                        piCons.append(temp)
                    elif ( (dummy+1 < table[i]) and temp[j]==0 and control == 0):
                        pi[i]=C[j]
                        del piCons[len(piCons)-1]
                        piCons.append(temp)
                        
    if table[a]==a+1:
        return float('inf')
    else:
        return table[a]