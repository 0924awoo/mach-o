ml5: load.o ml.o
	gcc load.o ml.o -o ml5

load.o: load.c ml.h
	gcc -c load.c

ml.o: ml.c ml.h
	gcc -c ml.c

clean:
	rm load.o ml.o ml5
