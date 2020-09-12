#!/usr/bin/env python
import sys
import re
import math

class point:
    def __init__(self,x,y):
        self.x=x
        self.y=y

def onSegment(p, q,r): 
    if (q.x <= max(p.x, r.x) and q.x >= min(p.x, r.x) and q.y <= max(p.y, r.y) and q.y >= min(p.y, r.y)):
       return True
    return False
  
def orientation(p, q, r):
    val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y) 
    if (val == 0):
        return 0 
    elif (val > 0):
        return 1
    else:
        return 2
    
def intersect(p1,p2,p3,p4):
    x1, y1 = p1.x , p1.y
    x2, y2 = p2.x , p2.y
    x3, y3 = p3.x , p3.y
    x4, y4 = p4.x , p4.y
    
    xnum =  ((x1*y2-y1*x2)*(x3-x4)) - ((x1-x2)*(x3*y4-y3*x4)) 
    xden =  ((x1-x2)*(y3-y4)) - ((y1-y2)*(x3-x4)) 
    
    ynum = ( (x1*y2 - y1*x2)*(y3-y4)) - ((y1-y2)*(x3*y4-y3*x4) ) 
    yden = ( (x1-x2)*(y3-y4)) - ((y1-y2)*(x3-x4))
    
    interSec=point(round(float(xnum)/float(xden) , 2),round(float(ynum)/float(yden),2))
    return interSec

def doIntersect(p1, q1, p2, q2): 

    o1 = orientation(p1, q1, p2) 
    o2 = orientation(p1, q1, q2) 
    o3 = orientation(p2, q2, p1) 
    o4 = orientation(p2, q2, q1) 
   
    po=point(0,0)
    inters=[]
    dummyControl=0
    
    if (o1 != o2 and o3 != o4):
        po=intersect(p1,q1,p2,q2)
        inters.append(po)
        return [True,inters] 
   
    # p1, q1 and p2 are colinear and p2 lies on segment p1q1 
    if (o1 == 0 and onSegment(p1, p2, q1)):
        inters.append(p2)
        dummyControl=1

  
    # p1, q1 and q2 are colinear and q2 lies on segment p1q1 
    if (o2 == 0 and onSegment(p1, q2, q1)):
        inters.append(q2)
        dummyControl=1

    
    # p2, q2 and p1 are colinear and p1 lies on segment p2q2 
    if (o3 == 0 and onSegment(p2, p1, q2)):
        inters.append(p1)
        dummyControl=1

    # p2, q2 and q1 are colinear and q1 lies on segment p2q2 
    if (o4 == 0 and onSegment(p2, q1, q2)):
        inters.append(q1) 
        dummyControl=1

    if dummyControl == 1:
        return [True,inters]
    else:
        return [False,[]]


streets=[]

def checkInputA(inp):
    pattern= '(\s)*(a{1})(\s{1})("[a-zA-Z\s]+")(\s{1})(\([\s]*[-]?[\s]*[0-9]+[\s]*,{1}[\s]*[-]?[\s]*[0-9]+[\s]*\)[\s]*){2,}'
    match = re.search(pattern, inp)
    if match:
        if (match.group() ==  inp):
            return True
        else:
            return False
    else:
        return False
    
def checkInputC(inp):
    pattern= '(\s)*(c{1})(\s{1})("[a-zA-Z\s]+")(\s{1})(\([\s]*[-]?[\s]*[0-9]+[\s]*,{1}[\s]*[-]?[\s]*[0-9]+[\s]*\)[\s]*){2,}'
    match = re.search(pattern, inp)
    if match:
        if (match.group() ==  inp):
            return True
        else:
            return False
    else:
        return False

def checkInputR(inp):
    pattern= '(\s)*(r{1})(\s{1})("[a-zA-Z\s]+")(\s)*'
    match = re.search(pattern, inp)
    if match:
        if (match.group() ==  inp):
            return True
        else:
            return False
    else:
        return False
    
def checkInputG(inp):

    pattern= '(\s)*(g{1})(\s)*'
    match = re.search(pattern, inp)
    if match:
        if (match.group() ==  inp):
            return True
        else:
            return False
    else:
        return False

def addStreet(inp,idx):
    for idx2 in range(idx+3,len(inp)):
        if inp[idx2] == "\"":
            endIdx=idx2
            break
        
    streetName=inp[idx+3:endIdx]
    if streets:
        control5=1
        for st in streets:
            if (st[0].lower()==streetName.lower()):
                sys.stderr.write("Error: This street is already signed, either change its coordinates by \"c\" or use a new name \n")
                sys.stdout.flush()
                control5=0
                break
            else:
                continue
        if control5==1:
            street=[]
            street.append(streetName)
            street.append( strToPoints(inp[endIdx+1:]) )
            streets.append(street)
    else:
        street=[]
        street.append(streetName)
        street.append( strToPoints(inp[endIdx+1:]) )
        streets.append(street)



def removeStreet(coors,idx):
    control3=0
    for idx2 in range(idx+3,len(coors)):
        if coors[idx2] == "\"":
            endIdx=idx2
            break
        
    streetName=coors[idx+3:endIdx]
    if streets:
        for st in streets:
            if (st[0].lower()==streetName.lower()):
                streets.remove(st)
                control3=1
                break
            else:
                continue
    else:
        sys.stderr.write("Error: Street database is empty \n")
        sys.stdout.flush()
        
    if control3 == 0:
        sys.stderr.write("Error: There is no such street registered as: "+streetName+ "\n")
        sys.stdout.flush()
    
def changeStreet(inpu,idx):
    control3=0
    for idx2 in range(idx+3,len(inpu)):
        if inpu[idx2] == "\"":
            endIdx=idx2
            break
        
    streetName=inpu[idx+3:endIdx]
    if streets:
        for st in streets:
            if (st[0].lower()==streetName.lower()):
                streets.remove(st)
                control3=1
                addStreet(inpu,idx)
                break
            else:
                continue
    else:
        sys.stderr.write("Error: Street database is empty \n")
        sys.stdout.flush()
        
    if control3 ==0:
        sys.stderr.write("Error: There is no such street registered as: "+streetName+"\n")
        sys.stdout.flush()
    
def strToPoints(coors):
    patt = '\s+'
    replace = ''
    coorNew = re.sub(patt,replace,coors)
    points=[]
    for i in range(0,len(coorNew)):
        if coorNew[i] == "(":
            a1=i+1
        elif coorNew[i] == ",":
            a2=i-1
            a3=i+1
        elif coorNew[i] == ")":
            a4=i-1
            points.append( point( int(coorNew[a1:a2+1]),int(coorNew[a3:a4+1]) ) )
    return points

def onLine(p1,q1,p2):
    o1 = orientation(p1, q1, p2)
    if (o1 == 0 and onSegment(p1, p2, q1)):
        return True     
        

def graph():
    vertices=[]
    edges=[]
    if streets:
        rawEdges=[]
        for k in range(0,len(streets)):
            checks1=streets[k][1]
            for l in range(k+1,len(streets)):
                checks2=streets[l][1]
                for p1 in range(0,len(checks1)-1):
                    p11=p1+1
                    for p2 in range(0,len(checks2)-1):
                        p22=p2+1
                        cvp=doIntersect(checks1[p1],checks1[p11],checks2[p2],checks2[p22])
                        if cvp[0] == True:
                            interSecteds=cvp[1]
                            for inter in interSecteds:
                                if not( (inter.x , inter.y) in vertices ):
                                    dummyx=inter.x
                                    dummyy=inter.y
                                    if (dummyx-int(dummyx)==0):
                                        dummyx=int(dummyx)
                                    else:
                                        dummyx=round(dummyx,2)
                                        
                                    if (dummyy-int(dummyy)==0):
                                        dummyy=int(dummyy)
                                    else:
                                        dummyy=round(dummyy,2)
                                    vertices.append( (dummyx , dummyy ) )
                                    
                            if not( (checks1[p1].x, checks1[p1].y) in vertices):
                                vertices.append( ( checks1[p1].x, checks1[p1].y) )
                                
                            if not( (checks1[p11].x, checks1[p11].y) in vertices):
                                vertices.append( ( checks1[p11].x, checks1[p11].y) )
                                
                            if not( (checks2[p2].x, checks2[p2].y) in vertices):
                                vertices.append( ( checks2[p2].x, checks2[p2].y) )

                            if not( (checks2[p22].x, checks2[p22].y) in vertices):
                                vertices.append( ( checks2[p22].x, checks2[p22].y) )
                                
                            indexp1=vertices.index( (checks1[p1].x, checks1[p1].y) )
                            indexp11=vertices.index( (checks1[p11].x, checks1[p11].y) )
                            indexp2=vertices.index( (checks2[p2].x, checks2[p2].y) )
                            indexp22=vertices.index ( (checks2[p22].x, checks2[p22].y) )
                            for inter in interSecteds:
                                indexInter=vertices.index ( (inter.x , inter.y ) )
                                if indexp1 < indexp11:
                                    if not( ([[indexp1,indexp11],indexInter])  in  rawEdges):
                                        rawEdges.append([[indexp1,indexp11],indexInter])
                                else:
                                    if not( ([[indexp11,indexp1],indexInter] )  in  rawEdges):
                                        rawEdges.append([[indexp11,indexp1],indexInter])
                                    
                                if indexp2 < indexp22:
                                    if not( ([[indexp2,indexp22],indexInter])  in  rawEdges):
                                        rawEdges.append([[indexp2,indexp22],indexInter])
                                else:
                                    if not( ([[indexp22,indexp2],indexInter])  in  rawEdges):
                                        rawEdges.append([[indexp22,indexp2],indexInter])
                        else:
                            continue
        #print(rawEdges)
        while rawEdges:
            tempList=[]
            temp=rawEdges[0]
            head=(temp[0][0])
            pair=(temp[0][0])
            tempList.append(temp[0][0])
            tempList.append(temp[0][1])
            tempList.append(temp[1])
            if len(rawEdges)>1:
                ctr=1
                while ctr<len(rawEdges):
                    temp2=rawEdges[ctr]
                    if (temp[0] == temp2[0]) and (temp[1]!=temp2[1]):
                        tempList.append(temp2[1])
                        rawEdges.remove(temp2)
                    else:
                        ctr=ctr+1
            rawEdges.remove(temp)
            tempList=list(set(tempList))
            
            while (len(tempList)>1):
                distance=float('inf')
                pair=head
                for elem in tempList:
                    if elem == head:
                        continue
                    else:
                        disto=dist(vertices[head],vertices[elem])
                        #print(disto)
                        if disto < distance:
                            distance = disto
                            pair = elem
                        else:
                            continue
                if pair == head:
                    sys.stderr.write("Fatal error \n")
                    sys.stdout.flush()
                else:
                    if (not ( ([head,pair]) in edges) ) and (not ( ([pair,head]) in edges) ):
                        edges.append([head,pair])
                        tempList.remove(head)
                        head=pair
                    else:
                        tempList.remove(head)
                        head=pair
                    #print(edges)
        
        #print("V = {")
        #for vv in range (0,len(vertices)):
        #    if len(str(vv)) == 1:
        #        print("  "+str(vv)+":  ("+str(vertices[vv][0])+","+str(vertices[vv][1])+")")
        #    elif len(str(vv)) == 2:
        #        print("  "+str(vv)+": ("+str(vertices[vv][0])+","+str(vertices[vv][1])+")")
        #    else:
        #        print("  "+str(vv)+":("+str(vertices[vv][0])+","+str(vertices[vv][1])+")")
        #print("}")
        print("V "+str(len(vertices)) )
        sys.stdout.flush()
        stringo="E {"
        for ee in range (0,len(edges)):
            if ee != (len(edges)-1):
                stringo=stringo+("<"+str(edges[ee][0])+","+str(edges[ee][1])+">,")
            else:
                stringo=stringo+("<"+str(edges[ee][0])+","+str(edges[ee][1])+">")
        stringo=stringo+"}"
        print(stringo)
        sys.stdout.flush()

    else:
        sys.stderr.write("Error: Empty graph is genarated")
        sys.stdout.flush()
        
def dist(a,b):
    return math.sqrt( ((a[0]-b[0])**2)+((a[1]-b[1])**2) )

if __name__ == '__main__':
    while True:
        control1=0        
        inp = sys.stdin.readline()
        
        if (inp == '') or (inp.isspace() ) :
            break
        
        for idx in range(0,len(inp)):
            if inp[idx] == "a":
                if checkInputA(inp):
                    addStreet(inp,idx)
                else:
                    sys.stderr.write("Error: Your input format is wrong, please check it \n")
                    sys.stdout.flush()
                control1=1
                break
            
            elif inp[idx] == "c":
                if checkInputC(inp):
                    changeStreet(inp,idx)
                else:
                    sys.stderr.write("Error: Your input format is wrong, please check it \n")
                    sys.stdout.flush()
                control1=1
                break
            
            elif inp[idx] == "r":
                if checkInputR(inp):
                    sys.stdout.flush()
                    removeStreet(inp,idx)
                else:
                    sys.stderr.write("Error: Your input format is wrong, please check it \n")
                    sys.stdout.flush()
                control1=1
                break
            
            elif inp[idx] == "g":
                if checkInputG(inp):
                    sys.stdout.flush()
                    graph()
                else:
                    sys.stderr.write("Error: Your input format is wrong, please check it \n")
                    sys.stdout.flush()
                control1=1
                break
            else:
                continue
            
        if control1 == 0:
            sys.stderr.write("Error: Your input format is wrong, please check it \n")
            sys.stdout.flush()
