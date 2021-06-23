CC=clang

.PREFIXES:
.SUFFIXES:

waveplayer: main.o
	$(CC) -o $@ -lopenal -g $^

%.o: %.c
	$(CC) -c -o $@ -O2 $<

.PHONY: clean
clean:
	$(RM) main.o