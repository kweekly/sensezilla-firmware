def crc(bstr):
    poly = 0x131
    rem = 0
    for b in bstr:
        rem ^= ord(b);
        for bit in range(8):
            if (rem & 0x80) != 0:
                rem = 0xFF & ((rem << 1)^poly)
            else:
                rem = 0xFF & (rem << 1)
                
    return rem

print "%02X"%crc("\x68\x3A")