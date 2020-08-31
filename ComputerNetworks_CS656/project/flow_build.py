import os
from scapy.all import *
import csv

#folderAddr="/home/yekta/Desktop/CompletePCAPs/"
folderAddr="/home/yekta/Desktop/self_data/"

file1 = open(folderAddr+"uni_http2_guys.txt","w") 

data=[]
flow_no=0
for filename in os.listdir(folderAddr):
    if filename.endswith(".pcap"):
        print(filename)
        #Getting the size of the file in MB, if it is too big computer stucks
        size=os.path.getsize(folderAddr+filename)/1000000
        #print(filename+":"+str(os.path.getsize(filename)/1000000))
        #Use it if it is less than 500MB
        if(size<500):
            
            load_layer("tls")
            packets = rdpcap(folderAddr+filename)
            
            #load_layer("tls")
            #packets = rdpcap("/home/yekta/Desktop/self_data.pcap")
             
            flowIDs=[]
            flowPckts=[]

            for i in range(0,len(packets)):
                try:
                    dst_temp, src_temp ,dport_temp ,sport_temp = packets[i][1].dst, packets[i][1].src, packets[i][1].dport, packets[i][1].sport
                    if [dst_temp, src_temp ,dport_temp ,sport_temp] in flowIDs:
                        index_temp=flowIDs.index([dst_temp, src_temp ,dport_temp ,sport_temp])
                        flowPckts[index_temp].append(packets[i])
                    else:
                        flowIDs.append([dst_temp, src_temp ,dport_temp ,sport_temp])
                        flowPckts.append([packets[i]])
                except:
                    continue
                
            for i in range(0,len(flowPckts)):
                #[Packet number, Byte no, Byte per pack, Flow duration, server-client, protocol]
                data.append([0,0,0,0,0,0])
                data[flow_no][0]=len(flowPckts[i])
                for j in range(0,len(flowPckts[i])):
                    if flowPckts[i][j][1].version==6:
                        data[flow_no][1]=data[flow_no][1]+flowPckts[i][j][1].plen
                    elif flowPckts[i][j][1].version==4:
                        data[flow_no][1]=data[flow_no][1]+flowPckts[i][j][1].len
                    try:
                        if "HTTP/1" in flowPckts[i][j].sprintf("%IP.load%"):
                            data[flow_no][5]=1
                            if "Host" in flowPckts[i][j].sprintf("%IP.load%"):
                                data[flow_no][4]=1
                            elif "Server" in flowPckts[i][j].sprintf("%IP.load%"):
                                data[flow_no][4]=2
                    except:
                        print("except of try 1")
                        continue
                    try:
                        if "h2-" in flowPckts[i][j].sprintf("%msg%"):
                            data[flow_no][5]=2
                            file1.write(filename+"\n")
                            file1.write(str(i)+"\n")
                            if "TLSClientHello" in flowPckts[i][j].sprintf("%msg%"):
                                file1.write(str(i)+"\n")
                                data[flow_no][4]=1
                            elif "TLSServerHello" in flowPckts[i][j].sprintf("%msg%"):
                                file1.write(str(i)+"\n")
                                data[flow_no][4]=2
                        else:
                            continue
                    except:
                        print("except of try 2")
                        continue
                data[flow_no][2]=( data[flow_no][1]/data[flow_no][0] )
                data[flow_no][3]=float( flowPckts[i][-1].time-flowPckts[i][0].time )
                
                flow_no=flow_no+1
                
            del packets
                
with open(folderAddr+"uni_Data.csv","w") as my_csv:
    csvWriter = csv.writer(my_csv,delimiter=',')
    csvWriter.writerows(data)         
           
file1.close()              

data_http=[]
data_http_cs=[]

for flw in data:
    if flw[5] != 0:
        data_http.append(flw)
        if flw[4] != 0:
            data_http_cs.append(flw)
        else:
            continue
    else:
        continue
###############################################################################

