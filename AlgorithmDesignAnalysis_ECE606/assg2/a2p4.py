"""
ECE606, F'19, Assignment 2, Problem 4
Skeleton solution file.
"""

"""
You are not allowed to import anything.
You need to implement the following methods. You are
allowed to define whatever subroutines you like to
structure your code.
"""

def trieInsert(t,s):
    """
    You need to implement this method.
    """
    lenInput=len(s)
    if t == []:
        t.append( [[],[],[],[]] )
        t.append( False )
        
    if len(s) == 0:
        t[1]=True
    
    pointer=t
        
    for index in range(0,lenInput):
        pointer=pointer[0]
        
        pointer=pointer[ int(s[index]) ]
            
        if pointer == []:
            pointer.append( [[],[],[],[]] )
            pointer.append( False )
        
        if index == (lenInput-1):
            pointer[1]=True

def trieDelete(t,s):
    """
    You need to implement this method.
    """
    control=0
    if len(s) == 0:
        if (len(t[0][0]) == 0) and (len(t[0][1]) == 0) and (len(t[0][2]) == 0) and (len(t[0][3]) == 0):
            del t[1]
            del t[0]
        else:
            if t[1]==True:
                t[1]=False
    else:
        lenInput=len(s)
        pointer=t
        for index in range(0,lenInput):
            control=1
            try:
                pointer=pointer[0]
                pointer=pointer[ int(s[index]) ]
            except:
                control=0
                break
        try:
            dummy=pointer[0]
        except:
            control=0
                
        if control == 1:
            if (len(pointer[0][0]) == 0) and (len(pointer[0][1]) == 0) and (len(pointer[0][2]) == 0) and (len(pointer[0][3]) == 0):
                del pointer[1]
                del pointer[0]
            else:
                pointer[1] = False
            

        
        

def trieFind(t,s):
    """
    You need to implement this method.
    """
    if len(s) == 0:
        if t[1]==True:
            return True
        else:
            return False
    else:
        lenInput=len(s)
        pointer=t
        
        for index in range(0,lenInput):
            try:
                pointer=pointer[0]
                pointer=pointer[ int(s[index]) ]
            except:
                return False
                
            if pointer == []:
                return False
            
        if pointer[1] == True:
            return True
        else:
            return False

