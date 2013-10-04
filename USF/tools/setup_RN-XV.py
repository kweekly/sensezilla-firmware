import serial
import os,time,sys

SERIAL_PORT = sys.argv[1]
SERIAL_SPEED = sys.argv[2]
CONF = sys.argv[3]

fin = open(CONF,'r');
lines = []
for line in fin:
    if '#' in line:
        line = line[:line.find('#')]
    
    line = line.rstrip().lstrip()
    if ( len(line) > 0 ) :
        lines.append(line)

fin.close()

def wr(str):
    print "> "+str.rstrip()
    ser.write(str);

def rd():
    l = ser.readline()
    if ( "set" not in l and "4.00" not in l):
        print "< "+l.rstrip()
    return l
    
def fl():
    while len(rd()) > 0:pass
    
def cmdmode():
    # enter command mode
    time.sleep(0.1)
    wr("$$$")
    time.sleep(0.1)
    while True:
        l = rd();
        if ( len(l) == 0):
            raise Exception("Did not recieve $$$ or CMD");
        elif ("CMD" in l or "$$$" in l):
            break
                
    wr("set sys sleep 0\r\n");
    wr("\r\n"*3); 
    fl();

    
ser = serial.Serial(SERIAL_PORT,SERIAL_SPEED, timeout=1);
cmdmode();
cmdmodetry = False
lineidx = 0;
while lineidx < len(lines):
    line = lines[lineidx]
    wr(line+'\r\n')
    time.sleep(0.01);
    while True:
        l = rd();
        if ( len(l) == 0):
            if cmdmodetry:
                raise Exception("Did not recieve ERR or AOK");
            else:
                cmdmodetry = True
                cmdmode();
                continue;
                
        elif ("ERR" in l):
            raise Exception("ERR recieved");
        elif ("AOK" in l):
            break
    
    cmdmodetry = False
    lineidx += 1
    
## now save and reset module
wr("save\r\n");
time.sleep(0.01)
rd();
wr("reboot\r\n");
fl();