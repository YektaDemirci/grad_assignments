"""
ECE606, F'19, Assignment 4, Problem 4
Skeleton solution file.
"""

"""
You are not allowed to import anything.
You need to implement the following method. You are
allowed to define whatever subroutines you like to
structure your code.
"""

def relax(d,pi,u,v,w):
    if( d[v] > d[u] + w ):
        d[v] = d[u] + w
        pi[v] = u
        return d , pi
    return d , pi

def printPath(pi,path,edges,s,v):
    if (v==None) or ((s in path) and (s==v)):
        path.append(v)
        return path
    else:
        path.append(v)
        path = printPath(pi,path,edges,s,pi[v])
        return path

def bellman(G,s,edges,d,pi,path):
    d[s]=0
        
    for i in range(0,len(G)-1):
        for j in range(0,len(edges)):
            d, pi =relax(d,pi, edges[j][0], edges[j][1], edges[j][2] )
    for j in range(0,len(edges)):
        if (d[edges[j][1]] > d[edges[j][0]]+edges[j][2]):
            #print(edges[j][0],edges[j][1])
            #print("False")
            path=printPath(pi,path,edges,edges[j][1],edges[j][1])
            return path
    return path

def negcycle(G):
    """
    You need to implement this method.
    The input A is a directed graph encoded as
    a list of lists of <neighbour, weight> pairs.
    """
    edges=[]
    d=[]
    pi=[]
    path=[]
     
    for a in range(0,len(G)):
        temp=G[a]
        for b in range(0,len(temp)):
            edges.append([ a, temp[b][0], temp[b][1]])

        
    for indx in range(0,len(G)):
    
        for i in range(0,len(G)):
            d.append(float("inf"))
            pi.append(None)
        
        path=bellman(G,indx,edges,d,pi,path)

        if path:
            dummy=[]
            for k in range(len(path)-1,-1,-1):
                dummy.append(path[k])
            return dummy
        else:
            d.clear()
            pi.clear()
            path.clear()
            
    return []
