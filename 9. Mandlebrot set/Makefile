CFLAGS = -O3 -mavx -lSDL2 -lSDL2_ttf -I../common/include

all: avx no-avx
	echo 123
run-avx: avx
	./mandelbrot-avx

clean:
	rm mandelbrot-avx mandelbrot

avx:
	gcc $(CFLAGS) mandelbrot-avx-sdl.c -o mandelbrot-avx

no-avx:
	gcc $(CFLAGS) mandelbrot-sdl.c -o mandelbrot
