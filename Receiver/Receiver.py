import serial 
import struct
class Receiver:
    def __init__(self,port,rate,structure,size):#initialize elements needed for the communication
        self.ser=serial.Serial(port,rate)#initialize communication
        self.structure=structure#data structure 
        self.size=size#size expected
        self.data=[]#List to store the tuples

    def store(self):
        if(self.ser.is_open):
            if self.ser.read(2) == b'##':#checks for the header
                raw_data=self.ser.read(self.size)#Reads the bits

                if(self.size==len(raw_data)):#see if the sizes match
                    buf=struct.unpack(self.structure,raw_data)#translate the bits into data
                    self.data.append(buf)#data appends and stores the things
                    return buf #gives the tuple back
        return None #if it does not work returns none        
