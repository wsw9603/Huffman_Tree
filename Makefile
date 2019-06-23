huffman : main.c huffman.c huffman.h
	cc -o $@ $^

.PHONY : clean
clean :
	-rm huffman

.PHONY : run
run :
	./huffman

