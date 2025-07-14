<img width="100%" alt="cover_imgc" src="https://i.imgur.com/oGHIEMe.png" />

![](https://img.shields.io/github/stars/d-neto/imgc.svg?style=flat) ![](https://img.shields.io/github/forks/d-neto/imgc.svg?style=flat) ![](https://img.shields.io/github/issues/d-neto/imgc.svg?style=flat) ![](https://img.shields.io/github/downloads/d-neto/imgc/total?style=flat&color=4AF) ![](https://img.shields.io/github/license/d-neto/imgc?style=flat&color=4AF)

my personal implementations of digital image processing tools

## Introduction
This is a simple library that I made to study c, digital image processing techniques and algorithms. Some of the implementations are naive and simple, as this isn't intended to be a serious library, just for recreational purposes.

Some image operations like load and write are being done using <a href="https://github.com/nothings/stb">stbimage.h</a>.

> [!WARNING]
> Display images are not fully implemented, only available on Linux distros with X11, and may cause errors, be careful.

## Intallation
To compile this library, you can just build using Makefile

Fisrt clone the repository:
```bash
git clone https://github.com/d-neto/imgc.git
cd ./imgc
```

Then build the project with the command bellow and it will generate `libimgc.a`.
```bash
make
```

If you want to see some examples, you can run:
```bash
make examples
```

## Usage
To use the it, you can link your program with `libimgc.a`, the static library generated from compilation.

Example Program:
```c
#include "imgc.h"

int main(){

    img_t my_image = img_load("path/to/my_image.ext");

    img_t new_image = img_bright(my_image, 40.f);

    img_write(new_image, "my_new_image.png");

    return 0;
}
```

Compile with:
```bash
gcc -I ./path/to/imgc/includes -L ./path/to/imgc/lib program.c -limgc -lm
```