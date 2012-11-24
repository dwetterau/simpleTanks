tanks: tanks.c graphics.c 
	gcc tanks.c graphics.c -lSDL -lm -o tanks

clean:
	rm -f tanks
