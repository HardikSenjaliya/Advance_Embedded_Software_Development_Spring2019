CC = gcc
INCLUDE_FLAGS = -I .
OPT_FLAGS = -o2 -g
#RT_FLAGS = -lpthread -lrt

CFLAGS = $(INCLUDE_FLAGS) $(OPT_FLAGS)


%.o:%.c
	@echo "compiling" $@ "using" $<
	$(CC) $(CFLAGS) -c $< -o $@

project1: main.o utils.o light.o temperature.o logger.o
	@echo "Building target $@"
	$(CC) $(CFLAGS) $^ -o $@ -lpthread -lrt


clean:
	rm -rf *.o
	rm -rf project1 *.txt