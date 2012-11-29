tanks: tanks.c graphics.c 
	rm -f tanks
	gcc tanks.c graphics.c -lSDL -lm -o tanks -g

clean:
	rm -f tanks
