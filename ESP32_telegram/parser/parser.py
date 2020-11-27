import os
import gzip

def bin_read_gzip(name):
    with open(name, 'rb') as f:
        return gzip.compress(f.read())

def bin_read(name):
    with open(name, 'rb') as f:
        return f.read()

def byte2var(name, data):
    f_esc = ''
    for i, c in enumerate(data):
        f_esc += '0x%02X'%c
        if i < len(data)-1:
            f_esc += ', '
        if ((i+1)%12==0):
            f_esc += '\\\n\t'
    return 'const uint8_t %s[%i] PROGMEM = \\\n{\t%s};\n'%(name,len(data), f_esc)
    
def parse_file(fullname, new_name):
    block = bin_read(fullname)
    return byte2var(new_name, block)

path = './'
fileout = '../libraries/data.h'
c_header = '#ifndef DATA_H\n#define DATA_H\n\n'

if  __name__ == '__main__':
    print(__file__)
    print('Parse webpage %s' % path)
    for filename in os.listdir(path):
        if filename!=__file__: 
            fullname = path+filename
            if os.path.isfile(fullname):
                name, ext = os.path.splitext(filename)
                new_name = ext.replace('.','') + "_" + name.replace('.','_')
                print('\t%s -> %s'%(filename, new_name))
                var = parse_file(fullname, new_name)
                c_header += var + '\n\n'

    c_header += '#endif\n'

    print('Write %s' % fileout)
    with open(fileout, 'w') as of:
        of.write(c_header)
