import time

import socket


def main():
    x,y, ipAddress, locations = readConfigFile()
    x = int (x)
    y = int(y)
    try:
        controllerPort = input ("what port do you want to use for controller? ")
    except ValueError:
        print ("did you enter a port number? ")
        quit()
    try:
        myIPAddr = ipAddress[int(controllerPort)]
    except KeyError:
        print ("can't find your port in the config file")
        quit()

    mySocket = createSocket(myIPAddr)

    file = openFile()
    for line in file:
       print ("line is ", line)
       tokens = list(line.split())
       processCommand(tokens, ipAddress, locations,
                      controllerPort, mySocket)
       time.sleep (1)
               
    
    

def sendCommand(mySocket,ipaddr, port, string2Send):


    UDP_IP = ipaddr
    UDP_PORT = port
    
    MESSAGE = string2Send
    #print("UDP target IP: %s" % UDP_IP)
    #print("UDP target port: %s" % UDP_PORT)
    #print("message: ",  MESSAGE)


    mySocket.sendto(MESSAGE.encode(), (UDP_IP, UDP_PORT))


def readConfigFile():
    filename = "config.txt"
    listOfAddresses = []
    ipAddress = {}
    location = {}
    file = open (filename)
    
    line = file.readline();
    rows, columns = line.split()
    for line in file:
        ip, port = line.split()
        ipAddress[int(port)] = ip
        location [int(port)] = location
        
        
    #print (ipAddress) 
    return rows,columns, ipAddress, location

def getCommand():
    command = input ("what command would you like to send?\n" +
                     "if you want to send MSG, format is <port> <cmd> <SEQ#> <message>" + 
                     "Format <port> <cmd>, entering all 0's for port sends to everyone: ")
    return command

def getCommandFile(file):
    command = file.readline()
    return command

def openFile ():
    file = open ("commands.txt")
    return file

def processCommand(tokens, ipAddress, locations, myPort, mySocket):
    if tokens[1] == "MOV":
        port = int (tokens[0])
        if port == 0:
            for key in ipAddress:
                ipaddr = ipAddress[int(key)]
                string2Send = "6:" + "MOV:" + "30:" + myPort  + ":" + str(key) + ":1:" +  "1: " +   myPort + ":" + tokens[2]
                #print (string2Send)
                sendCommand (mySocket, ipAddress[int(key)], key, string2Send);
        else:
            ipaddr = ipAddress[int(port)]
            #print ("Ip address for command is ", ipaddr)
            string2Send = "6:" + "MOV:" + "30:" + myPort + ":" + tokens[0] + ":1:" + "1:" +   myPort + ":" + tokens[2]
            #print (string2Send)
            sendCommand (mySocket, ipaddr, port, string2Send);
                   
            sendCommand (mySocket, ipaddr, port, string2Send);
    elif tokens[1] == "LOC":
        port = int (tokens[0])
        if port == 0:
            for key in ipAddress:
                ipaddr = ipAddress[int(key)]

                string2Send = "6:" + "LOC:" + "30:" + myPort + ":" + str(key) + ":1:"  + "1:" +   myPort +":" + "  "
                #print (string2Send)
                sendCommand (mySocket, ipAddress[int(key)], key, string2Send);
        else:
            ipaddr = ipAddress[int(port)]
            #print ("Ip address for command is ", ipaddr)
            string2Send = "6:" + "LOC:" + myPort + ":" + tokens[0] + ":1:"  + "1:" +   myPort +":" + "  "
            #print (string2Send)
            sendCommand (mySocket, ipaddr, port, string2Send);
    elif tokens[1] == "MSG":
        port = int (tokens[0])
        seqNumber = tokens [2]
        if port == 0:
            for key in ipAddress:
                ipaddr = ipAddress[int(key)]

                string2Send = "6:" + "INFO:" + "30:" + myPort + ":" + str(port) + ":4:"  + seqNumber + ":"+   myPort +":" + "HELLO-" + str(port) + "-" +seqNumber
                #print (string2Send)
                sendCommand (mySocket, ipAddress[int(key)], key, string2Send);
        else:
            for key in ipAddress:
                ipaddr = ipAddress[int(key)]
                string2Send = "6:" + "INFO:" + "30:" + myPort + ":" + str(port) +  ":4:"  + seqNumber + ":"+   myPort + ":"  + "HELLO-" + str(port) + "-" + seqNumber
                sendCommand (mySocket, ipaddr, key, string2Send);



def createSocket( myIPAddr):
       sock = socket.socket(socket.AF_INET, # Internet
                        socket.SOCK_DGRAM) # UDP
       
       return sock
 

main()
