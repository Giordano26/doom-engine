TARGET = graph_test

CC = gcc

LIB_LIST = -framework GLUT -framework OpenGL -framework Cocoa

SOURCE_FILES = graph_test.c


$(TARGET): $(SOURCE_FILES)
	$(CC) $^ -o $@ $(LIB_LIST)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)
