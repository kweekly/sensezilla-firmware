import sys,os,time,struct
import numpy as np, serial, math
from numpy import dot,inner,outer
from numpy.linalg import eig,norm,inv

pretend = False

unbuffered = os.fdopen(sys.stdout.fileno(), 'w', 0)
sys.stdout = unbuffered

SETTLE_TIME = 60;

MAX_SENSE_RESISTANCE = 10e-3;
MAX_CURRENT_RMS = 15;

VSENSE_HIGH_R = 470e3 * 2;
VSENSE_LOW_R = 1e3;

max_vdrop_rms = MAX_CURRENT_RMS * MAX_SENSE_RESISTANCE;
max_vsense_rms = 130 * VSENSE_LOW_R / (VSENSE_HIGH_R + VSENSE_LOW_R);

FULL_SCALE_GAIN_SETTING_I = 250e-3;
FULL_SCALE_GAIN_SETTING_V = 250e-3;

def hex_to_float(hstr, sign_bit=False):
    reg = int(hstr,16);
    
    rval = 0;
    i = 0;
    if sign_bit:
        if ( reg & 0x00800000 ):
            rval = 0x80000000;
        reg <<= 1;
        reg &= 0xFFFFFF;

    if ( reg == 0 ):
        return 0.0
    
    while ( 0 == (reg & 0x00800000) ):
        reg <<= 1;
        i += 1;
    
    i += 1;
    rval = rval | ((127-i)<<23) | (reg & 0x7FFFFF);
    binstr = struct.pack("<I",rval);
    (flt,) = struct.unpack("<f", binstr);
    return flt
    
def float_to_hex(flt, sign_bit=False):
    binstr = struct.pack("<f",flt);
    (rval,) = struct.unpack("<I",binstr);
   # print "\n%08X"%rval
    sb = (rval>>31)&0x01;
    exp =  (rval >> 23)&0xFF;
    exp = 127-exp;
    frac = rval & 0x7FFFFF;
    
    if frac==0 and exp==127:
        return "000000"
    
    #print sb,exp,frac
    frac >>= exp - 1;
    
    
    if (sign_bit):
        reg = (sb << 23) | (1<<(23-exp)) | (frac>>1);
    else:
        reg = (1<<(24-exp)) | frac;
    
    return "%06X"%reg
    
def graycode(n):
    a = [0]*n
    retval = []
    for i in range(0,pow(2,n)):
        b = 'X'
        for j in range(0,n):
            if ((i % pow(2,j+1)) - pow(2,j) == 0):
                a[j] = pow(1 - a[j], 2)
                b = j
                break
        retval.append(a[:])
        
    return retval
    
last_code = None;
def set_code(code):
    global last_code
    if last_code == None:
        last_code = [(1-d) for d in code];
    
    for i in range(len(code)):
        if code[i] == 1 and last_code[i] == 0:
            send('+%d'%(i+1));
        elif code[i] == 0 and last_code[i] == 1:
            send('-%d'%(i+1));

    last_code = code;
 
def send(str):
    global ser
    print "> "+str
    tosend = str+"\n";
   # print " ("+' '.join(['%02X'%ord(d) for d in tosend])+")"
    ser.write(tosend);
    
serbuf = '';
def tick(tries=2):
    global ser, serbuf
    i = 0;
    while i < tries:
        timestart = time.time();
        while timestart + SERIAL_TIMEOUT > time.time() and (len(serbuf) == 0 or serbuf[-1] != '\n'):
            while True:
                ch = ser.read();
                if ( ch != None and len(ch)>0 ):
                    serbuf += ch;
                    if ( ch == '\n'):
                        break
                else:
                    break
                
            time.sleep(0.01);
        
        if ( len(serbuf)>0 and serbuf[-1] == '\n' ):
            line = serbuf[:-1];
            serbuf = '';
            print "< "+line
            return line
        i += 1
            
    return None
    
def readhexval():
    send("h");
    while(True):
        line = tick();
        if (line):
            pts = line.split(',')
            if len(pts) != 6:
                continue;
            retval = []
            for pt in pts:
                pts2 = pt.split(' ')
                retval.append([hex_to_float(s) for s in pts2[1:]]);
                    
            return retval
        else:
            print "Error: timeout"
            return None
            
def setgains(iGain,iOffset,vGain,vOffset):
    send("g %s %s %s %s"%(float_to_hex(iGain/4.0),float_to_hex(iOffset,sign_bit=True),float_to_hex(vGain/4.0),float_to_hex(vOffset,sign_bit=True)));

def float_to_str(flt):
    binstr = struct.pack(">f",flt);
    return ''.join(["%02X"%ord(d) for d in binstr]);
    
def str_to_float(st):
    i32 = int(st,16);
    binstr = struct.pack(">I",i32);
    (flt,) = struct.unpack(">f",binstr);
    return flt;
    
def settle():
    for i in range(SETTLE_TIME/5):
        time.sleep(5);
        print ".",
    print " Done."
  
SERIAL_PORT = sys.argv[1];
SERIAL_TIMEOUT = 1;
  
  
if  len(sys.argv) > 2:
    pMeas = np.empty((6,2**6));
    iMeas = np.empty((6,2**6));
    vMeas = np.empty((6,2**6));

    fin = open(sys.argv[2],'r');
    for line in fin:
        line = line.rstrip();
        if '@C' in line:
            code = line[2:line.index(' ')][::-1]
            codei = int(code,2)
            pts = line[line.index(' ')+1:].split(';');
            pts2 = [p.split(',') for p in pts];
            pMeas[:,codei] = [str_to_float(a[0]) for a in pts2];
            iMeas[:,codei] = [str_to_float(a[1]) for a in pts2];
            vMeas[:,codei] = [str_to_float(a[2]) for a in pts2];
        elif '@LOAD_MEASUREMENTS=' in line:
            line = line[line.index('=')+1:];
            pts = line.split(';');
            pts2 = [p.split(',') for p in pts];
            pGTmeas = np.array([str_to_float(a[0]) for a in pts2]).T * 1000.0; # in kW
            iGTmeas = np.array([str_to_float(a[1]) for a in pts2]).T;
            vGTmeas = np.array([str_to_float(a[2]) for a in pts2]).T;

    fin.close();
    
    pGTmeas_exp = np.zeros((6,2**6));
    iGTmeas_exp = np.zeros((6,2**6));
    
    for i in range(2**6):
        for b in range(6):
            if ( (i & (1<<b)) != 0 ):
                pGTmeas_exp[b,i] = pGTmeas[b];
                iGTmeas_exp[b,i] = iGTmeas[b];
    
    pAinv_t,pResid,pRank,pSing = np.linalg.lstsq(pMeas.T, pGTmeas_exp.T, rcond=1e-10);
    pAinv = pAinv_t.T
    iAinv_t,iResid,iRank,iSing = np.linalg.lstsq(iMeas.T, iGTmeas_exp.T, rcond=1e-10);
    iAinv = iAinv_t.T
    
    print ""
    for i in range(6):
        print '%d '%i + ', '.join(['%6.2f'%(f) for f in iAinv[i,:]])
    
    print ""
    for i in range(6):
        print '%d '%i + ', '.join(['%6.2f'%(f) for f in iGTmeas_exp[:,i]])
        
    print ""
    for i in range(6):
        print '%d '%i + ', '.join(['%6.2f'%(f) for f in dot(iAinv,iMeas)[:,i]])
    
else:    
    if not pretend:  
        print "Opening Serial port: ",SERIAL_PORT," 115200 baud"
        ser = serial.Serial(SERIAL_PORT, 115200, timeout=0, rtscts=0);

        if (not readhexval()):
            if (not readhexval()):
                print "Cannot establish communication";
                sys.exit(1);

    print "\nSetting Gain and Offsets:"
    vdrop_pk = max_vdrop_rms * math.sqrt(2);
    iGain = FULL_SCALE_GAIN_SETTING_I / vdrop_pk;
    print "\tCurrent Max Vdrop_pk=%.2fmV, set gain=%.2f"%(1e3*vdrop_pk,iGain)
    vsense_pk = max_vsense_rms * math.sqrt(2);
    vGain = FULL_SCALE_GAIN_SETTING_V / vsense_pk;
    print "\tCurrent Max Vsense_pk=%.2fmV, set gain=%.2f"%(1e3*vsense_pk,vGain)

    if not pretend:
        setgains(iGain,0,vGain,0);  
        
    print "\nRunning Calibration:"
    codes = graycode(6);
    for code in codes:
        print "\tCode: "+','.join(['%d'%d for d in code])+" Settling.";
        
        if not pretend:
            set_code(code);
            settle();
            readhexval(); # flush bad stuff?
            time.sleep(1);
            vals = readhexval();
            print "@C"+''.join(['%d'%d for d in code])+" "+';'.join([','.join([float_to_str(chv) for chv in cha]) for cha in vals])
        
    if not pretend:
        set_code([0]*6);

    raw_input("Ready to perform load power measurement?");
        
    print "\nDetermining load power measurements:"
    LOAD_MEASUREMENTS = []
    for ch in range(6):
        print "\tCHANNEL %d: Settling."%(ch+1),
        code = [0]*6;
        code[ch] = 1;
        if not pretend:
            set_code(code);
            settle();
            
        Pstr = raw_input("\t\tPower  (kW): ");
        print Pstr
        Istr = raw_input("\t\tCurrent (A): ");
        print Istr
        Vstr = raw_input("\t\tVoltage (V): ");
        print Vstr
        LOAD_MEASUREMENTS.append((float(Pstr),float(Istr),float(Vstr)));
        
        
    if not pretend:
        set_code([0]*6);
        
        
    #print LOAD_MEASUREMENTS
    print "@LOAD_MEASUREMENTS="+';'.join([','.join([float_to_str(chv) for chv in cha]) for cha in LOAD_MEASUREMENTS])+""
    if not pretend:
        ser.close()
    
    