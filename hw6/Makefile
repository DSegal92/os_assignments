GS = -Wall -Werror -std=c99 -D_XOPEN_SOURCE=600
SOURCE = baboons.c
OUTPUT = baboons

all: $(SOURCE)
	$(CC) $(FLAGS) -o $(OUTPUT) $(SOURCE) -pthread
	chmod u+x $(OUTPUT)

clean:
	@rm -f $(OUTPUT)

