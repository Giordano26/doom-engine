TARGET = graph_test

CC = gcc

LIB_LIST = -lGL -lGLU -lglut -lm

SOURCE_FILES = graph_test.c


$(TARGET): $(SOURCE_FILES)
	$(CC) $^ -o $@ $(LIB_LIST)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)
