PROGRAM =testmulthreads
CC = gcc
CFLAGS = -g -std=c99 

${PROGRAM}:	${PROGRAM}.o alphabetcountmulthreads.o
		${CC} -o ${PROGRAM} ${PROGRAM}.o alphabetcountmulthreads.o -lpthread -lm

alphabetcountmulthreads.o ${PROGRAM}.o:	count.h


clean:
		rm -f *.o ${PROGRAM}		
