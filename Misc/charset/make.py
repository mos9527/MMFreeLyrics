import sys,glob
if len(sys.argv) != 2:
    print('usage : make.py <glob pattern/file name>')    
    sys.exit(1)
txt = ''
for path in glob.glob(sys.argv[1]):
    content = open(path,'r',encoding='utf-8').read()
    txt += content
    print('Appending %s (%d chars)' % (path,len(content)))
tbl = dict()
for t in txt:     
    tbl.setdefault(t,0)
    tbl[t] += 1
txt2 = ''.join([k for k,v in sorted(tbl.items(),key=lambda v:v[1]) if not k in '\t\n '])
with open('charset.txt','w',encoding='utf-8') as f:
    f.write('## This file is automatically generated by MMFreeLyrics/Misc/charset/make.py. The Characters shown are sorted by their frequencies.\n')
    f.write(txt2)
print('%d chars in. %d chars out.' % (len(txt),len(txt2)))