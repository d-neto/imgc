CC=gcc
CFLAGS=-Wall -Wextra
OPTIMIZE=-O3

SRC_PATH=./src
DIST_PATH=./dist
BIN_PATH=./bin

IMAGE_OBJ=$(DIST_PATH)/images.o $(DIST_PATH)/text.o $(DIST_PATH)/intensity.o $(DIST_PATH)/convolve.o $(DIST_PATH)/blend.o $(DIST_PATH)/morph.o $(DIST_PATH)/transform.o $(DIST_PATH)/histogram.o $(DIST_PATH)/thresh.o $(DISPLAY_FILES_TARGET)
LIBIMGC_OBJ=$(IMAGE_OBJ) $(DIST_PATH)/matrix.o $(DIST_PATH)/geom.o 

OS := $(shell uname)
DISPLAY_FILES_TARGET := $(DIST_PATH)/display.o

# Linux -> X11
ifeq ($(OS), Linux)
    DISPLAY_FILES_TARGET += $(DIST_PATH)/display_x11.o
endif

all: ./lib/libimgc.a

./lib/libimgc.a: $(LIBIMGC_OBJ)
	ar rcs $@ $^ -lm

$(DIST_PATH)/images.o: $(SRC_PATH)/image/images.c
	$(CC) $(CFLAGS) -I./includes $(OPTIMIZE) -c -o $@ $^
$(DIST_PATH)/histogram.o: $(SRC_PATH)/image/histogram.c
	$(CC) $(CFLAGS) -I./includes $(OPTIMIZE) -c -o $@ $^
$(DIST_PATH)/thresh.o: $(SRC_PATH)/image/thresh.c
	$(CC) $(CFLAGS) -I./includes $(OPTIMIZE) -c -o $@ $^
$(DIST_PATH)/transform.o: $(SRC_PATH)/image/transform.c
	$(CC) $(CFLAGS) -I./includes $(OPTIMIZE) -c -o $@ $^
$(DIST_PATH)/intensity.o: $(SRC_PATH)/image/intensity.c
	$(CC) $(CFLAGS) -I./includes $(OPTIMIZE) -c -o $@ $^
$(DIST_PATH)/convolve.o: $(SRC_PATH)/image/convolve.c
	$(CC) $(CFLAGS) -I./includes $(OPTIMIZE) -c -o $@ $^
$(DIST_PATH)/blend.o: $(SRC_PATH)/image/blend.c
	$(CC) $(CFLAGS) -I./includes $(OPTIMIZE) -c -o $@ $^
$(DIST_PATH)/morph.o: $(SRC_PATH)/image/morph.c
	$(CC) $(CFLAGS) -I./includes $(OPTIMIZE) -c -o $@ $^
$(DIST_PATH)/text.o: $(SRC_PATH)/image/text.c
	$(CC) $(CFLAGS) -I./includes -c -o $@ $^


$(DIST_PATH)/display.o: $(SRC_PATH)/display/display.c
	$(CC) $(CFLAGS) -I./includes -c -o $@ $^

$(DIST_PATH)/display_x11.o: $(SRC_PATH)/display/x11.c
	$(CC) $(CFLAGS) -I./includes -c -o $@ $^ -lX11 -lpthreads

$(DIST_PATH)/matrix.o: $(SRC_PATH)/mat/matrix.c
	$(CC) $(CFLAGS) -I./includes $(OPTIMIZE) -c -o $@ $^

$(DIST_PATH)/geom.o: $(SRC_PATH)/geom/geom.c
	$(CC) $(CFLAGS) -I./includes $(OPTIMIZE) -c -o $@ $^


# Examples

EXAMPLES_SRC = $(wildcard examples/*.c)
EXAMPLES = $(patsubst examples/%.c, bin/examples/%, $(EXAMPLES_SRC))

examples: $(EXAMPLES)

bin/examples/%: examples/%.c ./lib/libimgc.a
	$(CC) $(CFLAGS) -fopenmp -I./includes -L./lib -o $@ $< -limgc -lm -lX11

clean:
	rm -r $(DIST_PATH)/*.o $(EXAMPLES) ./lib/libimgc.a ./samples/out/*.png