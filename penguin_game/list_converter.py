#!/usr/bin/python3
from functools import reduce


vertString = input("give me the list of vertices: ")
vertList = vertString.strip('{').strip('}').split('}, {')
xList = list(map(lambda vert: abs(int(vert.split(',')[0])),vertList))
yList = list(map(lambda vert: int(vert.split(',')[1]),vertList))
absYlist = []
for i in range(0,len(yList)):
    absYlist.append(sum(yList[0:i+1])+30)
print('X',xList)
print('Xlen',len(xList))
print('Y',absYlist)
print('Ylen',len(yList))