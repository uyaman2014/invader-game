inbader: kbhit.o inbader.o
	gcc -o inbader kbhit.o inbader.o

kbhit.o: kbhit.h kbhit.c
	gcc -c kbhit.c

main.o: inbader.c
	gcc -c inbader.c

clean:
	rm -f *.o

