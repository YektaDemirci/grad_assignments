"""
ECE606, F'19, Assignment 6, Problem 4
Skeleton solution file.
"""

"""
You are not allowed to import anything.
You need to implement the following method. You are
allowed to define whatever subroutines you like to
structure your code.
"""

def mstexists(G, e):
    """
    You need to implement this method.
    G is an adjacency list in the format discussed in the handout
    e is a list of two items, e.g., [0,4]
    """
    e1=e[0]
    e2=e[1]
    if e1 > e2:
        e1=e[1]
        e2=e[0]
        
    edges=[]
    Grap=[]
    paths=[]
    for i in range(0,len(G)):
        paths.append([])
        dummy2=[]
        for j in range(0,len(G[i])):
            dummy2.append(G[i][j][0])
            dummy=[i]
            dummy.extend(G[i][j])
            edges.append(dummy)
            if (e1 == i) and (e2 == G[i][j][0]):
                weight=G[i][j][1]
        dummy2=list(set(dummy2))
        Grap.append(dummy2)
            
    edges.sort(key=lambda edges: edges[2])
    impIndx=edges.index([e1,e2,weight])
        
    vertices=[]
    tree=[]    
    ctrl=1
    for i in range(0,len(edges)):
        if (edges[i][2] == weight) and (ctrl==1):
            edges[i] , edges[impIndx] = edges[impIndx] , edges[i]
            ctrl=0
        if vertices:
            chck1=edges[i][0] in vertices
            chck2=edges[i][1] in vertices
            if chck1 and chck2:
                if ([edges[i][0],edges[i][1]] in tree) or ([edges[i][1],edges[i][0]] in tree):
                    #Do nothing
                    tempo=1
                else:
                    paths[edges[i][0]].append(edges[i][1])
                    paths[edges[i][1]].append(edges[i][0])
                    if DFS(paths):
                        tree.append([edges[i][0],edges[i][1]])
                    else:
                        del paths[edges[i][0]][len(paths[edges[i][0]])-1]
                        del paths[edges[i][1]][len(paths[edges[i][1]])-1]

            elif chck1 and not(chck2):
                paths[edges[i][0]].append(edges[i][1])
                paths[edges[i][1]].append(edges[i][0])               
                if DFS(paths):
                    tree.append([edges[i][0],edges[i][1]])
                    vertices.append(edges[i][1])
                else:
                    del paths[edges[i][0]][len(paths[edges[i][0]])-1]
                    del paths[edges[i][1]][len(paths[edges[i][1]])-1]
                 
            elif not(chck1) and chck2:
                paths[edges[i][1]].append(edges[i][0])
                paths[edges[i][0]].append(edges[i][1])                
                if DFS(paths):
                    tree.append([edges[i][0],edges[i][1]])
                    vertices.append(edges[i][0])
                else:
                    del paths[edges[i][1]][len(paths[edges[i][1]])-1]
                    del paths[edges[i][0]][len(paths[edges[i][0]])-1]
                    
            elif not(chck1) and not(chck2):
                paths[edges[i][0]].append(edges[i][1])
                paths[edges[i][1]].append(edges[i][0])
                if DFS(paths):
                    vertices.append(edges[i][0])
                    vertices.append(edges[i][1])
                    tree.append([edges[i][0],edges[i][1]])
                else:
                    del paths[edges[i][0]][len(paths[edges[i][0]])-1]
                    del paths[edges[i][1]][len(paths[edges[i][1]])-1]
                
        else:
            paths[edges[i][0]].append(edges[i][1])
            paths[edges[i][1]].append(edges[i][0])
            vertices.append(edges[i][0])
            vertices.append(edges[i][1])
            tree.append([edges[i][0],edges[i][1]])
        #print(str(i)+"   "+str(tree))
    
    for a in tree:
        a.sort(reverse=False)        
    if [e1,e2] in tree:
        #print(tree)
        return True
    else:
        #print(tree)
        return False
    
    #print(tree)
    #print(vertices)
    ##############AUTOMOCATICALLY ADDS ALL THE NEIGHBOUR BUT YOU SHOULD ADD ONLY THE CONNECTED FUCKERS BRO########################


def ancestor(pi,indx,path):
    if pi[indx]==-10:
        path.append(indx)
        return path
    else:
        path=ancestor(pi,pi[indx],path)
        path.append(indx)
        return path

def DFS(grap):
    control=0
    color = [0]*len(grap)
    parent = -10
    for k in range(0,len(grap)):
        if color[k]==0:
            color,control = DFSvisit(grap,color,k,parent,control)
    if control == 1:
        return False
    else:
        return True
            
def DFSvisit(grap,color,k,parent,control):
    color[k]=1
    for l in range(0,len(grap[k])):
        if color[grap[k][l]] == 0:
            color,control=DFSvisit(grap,color,grap[k][l],k,control)
        elif grap[k][l] == parent:
            continue
        else:
            control=1
    return color,control
    
    
        