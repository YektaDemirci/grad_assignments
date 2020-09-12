#include<iostream>
#include<fstream>
#include<vector>
#include<unistd.h>


class coor
{
    public:
        coor(float, float);
        float x,y;
};

coor::coor(float X=0, float Y=0)
{
    x = X;
    y = Y;
}


class streets
{
    public:
        streets(std::vector<coor>*, int);
        int ssN;
        std::vector<coor>* street;
        void setS(int);
        void show();
        void setPoint(int,coor);
        std::vector<coor> returnStreet(int);
        void erasePoint(int,int);
        void clean();


};

streets::streets(std::vector<coor>* grap=NULL, int sNumber=0)
{
    ssN=sNumber;
    street = grap;
}

void streets::setS(int ssNo)
{
    delete[] street;
    ssN=ssNo;
    street = new std::vector<coor>[ssN];
}

void streets::show()
{
    
    for(int i=0; i<ssN; i++)
    {
        std::vector<coor> temp = street[i];
        for (std::vector<coor>::iterator it = temp.begin(); it != temp.end(); it++ )
        {   
            coor temp2 = *it;
            std::cout << "[From show]: For street" << i <<" x is "<< temp2.x  << " y is "<< temp2.y << std::endl;
        }
    }
    
}

void streets::setPoint(int indx, coor pnt)
{
    street[indx].push_back(pnt);
}

std::vector<coor> streets::returnStreet(int idx)
{
    return street[idx];
}

void streets::erasePoint(int idxStreet,int idxLine)
{
    street[idxStreet].erase(street[idxStreet].begin()+idxLine);
}

void streets::clean()
{
    street[ssN-1].clear();
}


  
bool onSegment(coor p, coor q, coor r) 
{ 
    if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) && 
        q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y)) 
       return true; 
  
    return false; 
} 
  
int orientation(coor p, coor q, coor r) 
{  
    int val = (q.y - p.y) * (r.x - q.x) - 
              (q.x - p.x) * (r.y - q.y); 
  
    if (val == 0) return 0;  // colinear 
  
    return (val > 0)? 1: 2; // clock or counterclock wise 
}

float round(float var)
{
    float value = (int)(var * 100 + .5);
    return (float)value / 100;
}


coor intersect(coor p1,coor p2,coor p3,coor p4)
{
    float x1 = p1.x; 
    float y1 = p1.y;
    float x2 = p2.x;
    float y2 = p2.y;
    float x3 = p3.x;
    float y3 = p3.y;
    float x4 = p4.x;
    float y4 = p4.y;
    
    float xnum = ( ( (x1*y2)-(y1*x2) )*(x3-x4) ) - ((x1-x2)*( (x3*y4) - (y3*x4))); 
    float xden = ((x1-x2)*(y3-y4)) - ((y1-y2)*(x3-x4)); 
    
    float ynum = ( ( (x1*y2)-(y1*x2) )*(y3-y4) ) - ((y1-y2)*( (x3*y4) - (y3*x4))); 
    float yden = ( (x1-x2)*(y3-y4)) - ((y1-y2)*(x3-x4));
    
    coor interSec;
    interSec.x = round(xnum/xden);
    interSec.y = round(ynum/yden);
    return interSec;
} 
 
std::vector<coor> doIntersect(coor p1, coor q1, coor p2, coor q2) 
{ 
    int o1 = orientation(p1, q1, p2); 
    int o2 = orientation(p1, q1, q2); 
    int o3 = orientation(p2, q2, p1); 
    int o4 = orientation(p2, q2, q1);

    coor inter;
    std::vector<coor> intersections;
  
    // General case 
    if (o1 != o2 && o3 != o4)
    {
        inter=intersect(p1,q1,p2,q2);
        intersections.push_back(inter);
        return intersections; 
    }
    // p1, q1 and p2 are colinear and p2 lies on segment p1q1 
    if (o1 == 0 && onSegment(p1, p2, q1))
    {
        intersections.push_back(p2);  
    }
    // p1, q1 and q2 are colinear and q2 lies on segment p1q1 
    if (o2 == 0 && onSegment(p1, q2, q1))
    {
        intersections.push_back(q2);
    }  
  
    // p2, q2 and p1 are colinear and p1 lies on segment p2q2 
    if (o3 == 0 && onSegment(p2, p1, q2))
    {
        intersections.push_back(p1);
    }
  
     // p2, q2 and q1 are colinear and q1 lies on segment p2q2 
    if (o4 == 0 && onSegment(p2, q1, q2))
    {
        intersections.push_back(q1);
    } 
  
    return intersections; 
} 



int main(int argc, char *argv[])
{
    std::ifstream urandom("/dev/urandom");
    if (urandom.fail()) 
    {
        std::cerr << "Error: cannot open /dev/urandom\n";
        return 1;
    }

    unsigned int st = 42;
    urandom.read((char*)&st, sizeof(int));
    int stU = atoi(argv[1]);
    st = 2 + (st % (stU+1-2) );
    ///////////////std::cout << "Generated street numb is " << st << std::endl;

    unsigned int ln = 42;
    urandom.read((char*)&ln, sizeof(int));
    int lnU = atoi(argv[2]);
    ln = 1 + ( ln % (lnU+1-1) );
    ////////////////std::cout << "Generated line numb is " << ln << std::endl;
    ln=ln+1;

    unsigned int wt = 42;
    urandom.read((char*)&wt, sizeof(int));
    int wtU = atoi(argv[3]);
    wt = 5 + ( wt % (wtU+1-5) );
    ////////////////std::cout << "Generated wait numb is " << wt << std::endl;

    int pcoor = atoi(argv[4]);
    int ncoor = pcoor*(-1);

    int initialOnly=0;

    while(true)
    {
        int curSt=0;
        int totalIntersects=0;
        streets sTRt;
        sTRt.setS(st);
        int counterStreet=0;


        while(curSt<st)
        {
            int curLn=0;
            int counterLine = 0;
            while( (curLn<ln) && (counterLine<25) )
            {
                int kontrolNoktasi1=0;
                int kontrolNoktasi2=0;

                unsigned int xx = 42;
                urandom.read((char*)&xx, sizeof(int));
                int xxx;
                xxx = ncoor + ( xx % (pcoor+1-ncoor) );

                unsigned int yy = 42;
                urandom.read((char*)&yy, sizeof(int));
                int yyy;
                yyy = ncoor + ( yy % (pcoor+1-ncoor) );

                coor dummy;
                dummy.x=xxx;
                dummy.y=yyy;

                sTRt.setPoint(curSt, dummy);
                curLn=curLn+1;

                std::vector<coor> currentStreet;
                currentStreet = sTRt.returnStreet(curSt);
                //Check if initial two lines are the same
                if(currentStreet.size() == 2)
                {
                    coor first = currentStreet.at(0);
                    coor second = currentStreet.at(1);
                    if ( (first.x == second.x) && (first.y == second.y) )
                    {
                        //std::cout << "First two lines, they intersected" << std::endl;
                        kontrolNoktasi1=1;
                    }
                    
                }
                //Check if the last appended line segment intersect with the street itself
                else if(currentStreet.size() > 2)
                {
                    int endIdx = currentStreet.size();
                    endIdx=endIdx-1;
                    coor q222 = currentStreet.at(endIdx);
                    coor q111 = currentStreet.at(endIdx-1);
                    for(int cntr=0; cntr < endIdx-1; cntr++)
                    {
                        coor p111 = currentStreet.at(cntr);
                        coor p222 = currentStreet.at(cntr+1);
                        std::vector<coor> temporary;
                        temporary = doIntersect(p111,p222,q111,q222);
                        if (temporary.size() > 0)
                        {
                            if ( (p222.x == q111.x) && (p222.y == q111.y) &&
                             (temporary.size()==2) )
                            {
                                continue;
                            }
                            else if( (abs(temporary.at(0).x-q111.x)<0.03 ) && (abs(temporary.at(0).y-q111.y)<0.03 ) &&
                             (temporary.size()==1) )
                             {
                                 continue;
                             }
                            else
                            {
                                //for(int kk=0; kk < endIdx-1; kk++ )
                                //{
                                //    std::cout << "cad " << currentStreet.at(kk).x << ' ' << currentStreet.at(kk).y << ' ' << currentStreet.at(kk+1).x << ' ' << currentStreet.at(kk+1).y << std::endl; 
                                //}
                                //std::cout << "err  " << currentStreet.at(curLn-1).x << ' ' << currentStreet.at(curLn-1).y << std::endl;
                                //sTRt.show();
                                //for (std::vector<coor>::iterator it = temporary.begin(); it != temporary.end(); it++ )
                                //{   
                                //    coor temp2 = *it;
                                //    std::cout << " Hey Douglas: " << "x is "<< temp2.x  << " y is "<< temp2.y << std::endl;
                                //}
                                //std::cout << "++++++2 Lines, they intersected" << std::endl;
                                kontrolNoktasi1=1;
                            }
                        }
                        else
                        {
                            continue;
                        }
                    }
                }
                //If made to here so far, we know the segment has not intersect with itself
                //Check if the appended line segments completely overlaps with  another linesegments in the graph
                if (curSt)
                {
                    if(currentStreet.size() == 1)
                    {
                        //Check if the point is on any street
                        coor pp = currentStreet.at(currentStreet.size()-1);
                        for(int counterStr=0; counterStr < curSt; counterStr++)
                        {
                            std::vector<coor> temporaryStreet;
                            temporaryStreet = sTRt.returnStreet(counterStr);
                            for(int counterLn=0; counterLn <= (temporaryStreet.size()-2);counterLn++ )
                            {
                                coor pp1 = temporaryStreet.at(counterLn);
                                coor pp2 = temporaryStreet.at(counterLn+1);
                                int orient;
                                orient = orientation(pp1,pp2,pp);
                                if (orient == 0)
                                {
                                    //std::cout << "--, point overlap" << std::endl;
                                    kontrolNoktasi2=1;
                                }
                                else
                                {
                                    continue;
                                }
                            }
                        }
                    }
                    else  
                    {
                        //Check if the line segment is on any street
                        coor qq1 = currentStreet.at(currentStreet.size()-2);
                        coor qq2 = currentStreet.at(currentStreet.size()-1);
                        for(int counterStr=0; counterStr < curSt; counterStr++)
                        {
                            std::vector<coor> temporaryStreet;
                            temporaryStreet = sTRt.returnStreet(counterStr);
                            for(int counterLn=0; counterLn <= (temporaryStreet.size()-2);counterLn++ )
                            {
                                coor pp1 = temporaryStreet.at(counterLn);
                                coor pp2 = temporaryStreet.at(counterLn+1);
                                std::vector<coor> temps;
                                temps = doIntersect(pp1,pp2,qq1,qq2);
                                if (temps.size() == 1)
                                {
                                    //Means intersection
                                    totalIntersects++;
                                }
                                else if (temps.size() > 1)
                                {
                                    //Means overlap
                                    //std::cout << "-------------, Line overlap" << std::endl;
                                    kontrolNoktasi2=1;
                                }
                                else
                                {
                                    continue;
                                }
                            }  
                        }
                    }
                }
                if (kontrolNoktasi1  || kontrolNoktasi2)
                {
                    sTRt.erasePoint(curSt,curLn);
                    curLn=curLn-1;
                    counterLine = counterLine+1;
                    ////////////////////std::cout << "Deleted" << std::endl;
                }
                else
                {
                    counterLine=0;
                    ///////////////////std::cout << "Reset time " << std::endl; 
                }
            //Line did not intersect with street itself and no overlaps
            }
            if(counterLine==25)
            {
                std::cerr <<"Error: A line segment could not be generated in 25 times" << std::endl;
                exit(1);
            }
            curSt=curSt+1;
            if( (curSt == st) && (totalIntersects == 0) ) //None of the streets intersects so generate the last one again count 25 times
            {
                sTRt.clean();
                curSt=curSt=curSt-1;
                counterStreet=counterStreet+1;
            }
            if(counterStreet==25)
            {
                std::cerr <<"Error: A street could not be generated in 25 times" << std::endl;
                exit(1);
            }
        }
        ////////////////////sTRt.show();
        ////////////////////std::cout << "Intersection total" << totalIntersects << std::endl;


        //Streets have been generated so far.
        if(initialOnly)
        {
            //Remove the streets first
            for(int ii=0; ii<st; ii++)
            {
                std::string tempoString ="r \"";
                for(int mm=0; mm<=ii; mm++)
                {
                    tempoString=tempoString+"y";
                }
                tempoString=tempoString+"\"";
                std::cout << tempoString << std::endl;
                usleep(5000);

            }
            for(int ii=0; ii<st; ii++)
            {
                std::string tempoString ="a ";
                tempoString=tempoString+"\"";
                for(int mm=0; mm<=ii; mm++)
                {
                    tempoString=tempoString+"y";
                }
                tempoString=tempoString+"\"";

                std::vector<coor> tempo;
                tempo = sTRt.returnStreet(ii);
                for(int jj=0; jj < tempo.size(); jj++)
                {
                    coor temps=tempo.at(jj);
                    tempoString=tempoString+" ("+std::to_string((int)temps.x)+","+std::to_string((int)temps.y)+")";
                }
                std::cout << tempoString << std::endl;
                usleep(5000);
            }
            std::cout << "g" << std::endl;
            usleep(5000);
        }
        else
        {
            initialOnly=1;
            for(int ii=0; ii<st; ii++)
            {
                std::string tempoString ="a ";
                tempoString=tempoString+"\"";
                for(int mm=0; mm<=ii; mm++)
                {
                    tempoString=tempoString+"y";
                }
                tempoString=tempoString+"\"";

                std::vector<coor> tempo;
                tempo = sTRt.returnStreet(ii);
                for(int jj=0; jj < tempo.size(); jj++)
                {
                    coor temps=tempo.at(jj);
                    tempoString=tempoString+" ("+std::to_string((int)temps.x)+","+std::to_string((int)temps.y)+")";
                }
                std::cout << tempoString << std::endl;
                usleep(5000);
            }
            std::cout << "g" << std::endl;
            usleep(5000);
        }
        sleep(wt);
    }
    return 0;
}
