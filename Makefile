CC=gcc
CFLAGS=-Wall

SRC_PATH=./src
DIST_PATH=./dist
BIN_PATH=./bin

IMAGE_OBJ=$(DIST_PATH)/images.o  $(DIST_PATH)/intensity.o $(DIST_PATH)/convolve.o $(DIST_PATH)/blend.o $(DIST_PATH)/transform.o $(DIST_PATH)/histogram.o $(DIST_PATH)/thresh.o
LIBIMGC_OBJ=$(DIST_PATH)/alloc.o $(IMAGE_OBJ) $(DIST_PATH)/matrix.o $(DIST_PATH)/geom.o 

all: example ./lib/libimagec.a

example: ./examples/main.c ./lib/libimagec.a
	$(CC) $(CFLAGS) -I./includes -L./lib -o $(BIN_PATH)/$@ ./examples/main.c -limagec -lm

./lib/libimagec.a: $(LIBIMGC_OBJ)
	ar rcs $@ $^ -lm

$(DIST_PATH)/images.o: $(SRC_PATH)/image/images.c
	$(CC) $(CFLAGS) -I./includes -c -o $@ $^
$(DIST_PATH)/histogram.o: $(SRC_PATH)/image/histogram.c
	$(CC) $(CFLAGS) -I./includes -c -o $@ $^
$(DIST_PATH)/thresh.o: $(SRC_PATH)/image/thresh.c
	$(CC) $(CFLAGS) -I./includes -c -o $@ $^
$(DIST_PATH)/transform.o: $(SRC_PATH)/image/transform.c
	$(CC) $(CFLAGS) -I./includes -c -o $@ $^
$(DIST_PATH)/intensity.o: $(SRC_PATH)/image/intensity.c
	$(CC) $(CFLAGS) -I./includes -c -o $@ $^
$(DIST_PATH)/convolve.o: $(SRC_PATH)/image/convolve.c
	$(CC) $(CFLAGS) -I./includes -c -o $@ $^
$(DIST_PATH)/blend.o: $(SRC_PATH)/image/blend.c
	$(CC) $(CFLAGS) -I./includes -c -o $@ $^

$(DIST_PATH)/matrix.o: $(SRC_PATH)/matrix.c
	$(CC) $(CFLAGS) -I./includes -c -o $@ $^

$(DIST_PATH)/geom.o: $(SRC_PATH)/geom.c
	$(CC) $(CFLAGS) -I./includes -c -o $@ $^

$(DIST_PATH)/alloc.o: $(SRC_PATH)/alloc.c
	$(CC) $(CFLAGS) -I./includes -c -o $@ $^

$(BIN_PATH)/test_imagec: ./tests/test_imagec.c $(LIBIMGC_OBJ)
	$(CC) $(CFLAGS) -I./includes -o $@ $^ -lm
