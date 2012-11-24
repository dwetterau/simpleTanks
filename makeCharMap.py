#! /usr/bin/python2

fontFile = open('ter-u12n.bdf', 'r');
fillingChar = False
characters = []
character = []
for line in fontFile:
    if 'BITMAP' in line:
        fillingChar = True
        continue
    if fillingChar:
        if 'ENDCHAR' in line:
            fillingChar = False
            characters.append(character)
            character = []
            continue
        character.append(line.strip())
finalMat = []
hex2binary = {'0' : [0,0,0,0],
              '1' : [0,0,0,1],
              '2' : [0,0,1,0],
              '3' : [0,0,1,1],
              '4' : [0,1,0,0],
              '5' : [0,1,0,1],
              '6' : [0,1,1,0],
              '7' : [0,1,1,1],
              '8' : [1,0,0,0],
              '9' : [1,0,0,1],
              'A' : [1,0,1,0],
              'B' : [1,0,1,1],
              'C' : [1,1,0,0],
              'D' : [1,1,0,1],
              'E' : [1,1,1,0],
              'F' : [1,1,1,1]}
for i in range(256): #only want the first 256 chars
    newChar = []
    for row in range(len(characters[i])):
        newRow = []
        r = characters[i][row]
        for j in range(len(r)):
            newRow.extend(hex2binary[r[j]])
        newChar.append(newRow)
    finalMat.append(newChar)
#Print it out in C syntax
print 'static charMap[256][12][8] = '
print '{'
for i in range(len(finalMat)):
    print '{'
    for j in range(len(finalMat[i])):
        line = '{ ';
        for k in range(len(finalMat[i][j])):
            line += str(finalMat[i][j][k])
            if k != len(finalMat[i][j]) - 1:
                line += ','
        if j != len(finalMat[i]) - 1:
            line += '},'    
        else:
            line += '}'
        print line
    if i != len(finalMat) - 1:
         print '},'    
    else:
        print  '}'
print '};'
