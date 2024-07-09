GCC = gcc
CFLAGS = -Wall -Wshadow -pthread
OBJS = ex5.o bmp.o filter.o

ex5: $(OBJS)
	$(GCC) $(CFLAGS) $(OBJS) -o $@

.c.o: 
	$(GCC) $(CFLAGS) -g -c $*.c 

clean:
	rm *.o ex5

test: ex5
	./ex5 testcases/test.bmp outputs/test_out.bmp
	diff outputs/test_out.bmp testcases/test_sol.bmp
	
testmem: ex5
	valgrind --track-origins=yes --tool=memcheck --leak-check=summary ./ex5 testcases/test.bmp outputs/test_out.bmp	
