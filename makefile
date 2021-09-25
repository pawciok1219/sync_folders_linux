object = function.o main.o
all: demon
demon: $(object)
	gcc $(object) -o demon
.PHONY: clean
clean:
	rm -f *.o demon
