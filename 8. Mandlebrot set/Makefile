CFLAGS = -O3 -mavx -lSDL2

all: avx no-avx
	echo 123

clean:
	rm mandelbrot-avx mandelbrot

avx:
	gcc $(CFLAGS) mandelbrot-avx-sdl.c -o mandelbrot-avx

no-avx:
	gcc $(CFLAGS) mandelbrot-sdl.c -o mandelbrot
