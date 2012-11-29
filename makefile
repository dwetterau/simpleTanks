tanks: tanks.c graphics.c 
	./makeCharMap.py > charMap.h
	gcc tanks.c graphics.c -lSDL -lm -o tanks -g

clean:
	rm -f tanks charMap.h
