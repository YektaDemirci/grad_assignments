"""
ECE606, F'19, Assignment 9, Problem 4
Skeleton solution file.
"""

"""
You are not allowed to import anything.
You need to implement the following method. You are
allowed to define whatever subroutines you like to
structure your code.
"""
def Hamiltonian(k,G,x):
    a=0
    while(True):
        if a:
            return [x,a]
        x=nextVertex(k,G,x)
        ###print(x,k)
        if (x[k] == 0):
            return [x,a]
        if (k == len(x)-1):
            a=1
            return [x,a]
        else:
            [x,a]=Hamiltonian(k+1,G,x)
    
def nextVertex(k,G,x):
    ###print("I initial call"+str(x)+"I am k "+str(k))
    while(True):
        control=1
        x[k] = (x[k]+1) % (len(x)+1)
        #print(x)
        if (x[k]==0):
            ###print(str(x)+"I am k "+str(k))
            return x
        if(G[x[k-1]-1][x[k]-1] != 0):
            for j in range(0,k):
                if (x[k] == x[j]):
                    control=0
                    break
            if control:
                return x
            #if(k==len(x)-1):
                #print("I am k "+str(k)+" I am x[k]" +str(x[k]))
            #    return x
            #else:
            #    continue
            
        


def hampath(G):
    n=len(G)
    graph=[]
    for i in range(0,n):
        graph.append([0]*n)
        for j in range(0,len(G[i])):
            graph[i][G[i][j]]=1
    for idx in range(0,n):
        x=[0]*n
        x[0]=idx+1
        [x,a]=Hamiltonian(1,graph,x)
        if a == 1:
            finGraph=[]
            for i in range(0,len(x)):
                finGraph.append(x[i]-1)
            #print(idx)
            return finGraph
        else:
            continue
    return []
        