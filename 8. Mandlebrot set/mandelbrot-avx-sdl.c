#include <SDL2/SDL.h>
#include <immintrin.h>

const int MAXLOOP = 255;
const double SCROLL_POW = 10.0;

const int WIDTH = 1280;
const int HEIGHT = 720;

const double R2 = 10.0*10.0;

#define CX(x) (x - surf->w/2) * scale + ox
#define CY(y) (surf->h/2 - y) * scale + oy

typedef union {
	__m256d m256d;
	double d[4];
} avx_t;

void draw_mandelbrot(SDL_Surface* surf, double scale, double ox, double oy) {
	Uint32* pixbuf = surf->pixels;
	for (int xi = 0; xi < surf->w; xi += 4) {
		for (int yi = 0; yi < surf->h; yi++) {
			avx_t x, y, x0, y0;
			x0.m256d = _mm256_set_pd(CX(xi + 3), CX(xi + 2), CX(xi + 1), CX(xi));
			y0.m256d = _mm256_set1_pd(CY(yi));
			x = x0;
			y = y0;
	//		printf("(%lf, %lf) ", xn_1, yn_1);

			avx_t two, r2;
			two.m256d = _mm256_set1_pd(2.0);
			r2.m256d = _mm256_set1_pd(R2);

			//int n = 0;
			avx_t n, mask, one;
			n.m256d = _mm256_setzero_pd();
			mask.m256d = _mm256_set1_pd(1337.0);
			one.m256d = _mm256_set1_pd(1.0);
			while ( (mask.d[0] || mask.d[1] || mask.d[2] || mask.d[3]) && (n.d[0] < 256.0 && n.d[1] < 256.0 && n.d[2] < 256.0 && n.d[3] < 256.0)) {
				avx_t x2, xy, y2;
				x2.m256d = _mm256_mul_pd(x.m256d, x.m256d);
				xy.m256d = _mm256_mul_pd(x.m256d, y.m256d);
				y2.m256d = _mm256_mul_pd(y.m256d, y.m256d);

				mask.m256d = _mm256_cmp_pd(_mm256_add_pd(x2.m256d, y2.m256d), r2.m256d, 1);
				n.m256d = _mm256_add_pd(_mm256_and_pd(mask.m256d, one.m256d), n.m256d);

				x.m256d = _mm256_sub_pd(_mm256_add_pd(x2.m256d, x0.m256d), y2.m256d);
				y.m256d = _mm256_add_pd(_mm256_mul_pd(two.m256d, xy.m256d), y0.m256d);
				//double xn = xn_1*xn_1 - yn_1*yn_1 + x0;
				//double yn = 2*xn_1*yn_1 + y0;

				//if (x.d[0]*x.d[0] + y.d[0]*y.d[0] > R2) break;
			}

			for (int i = 0; i < 4; i++) {
				int ni = n.d[i];
				if (ni == MAXLOOP)
					pixbuf[surf->w*yi + xi + i] = SDL_MapRGBA(surf->format, 0, 0, 0, 255);
				else
					pixbuf[surf->w*yi + xi + i] = SDL_MapRGBA(surf->format, (ni << 1) & ni & 255, 16 * ni%16, ni, 255);
			}
		}
	}
}

int main() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
		printf("Init fail\n");
		return 1;
	}

	SDL_Window* win = SDL_CreateWindow("Mandelbrot set (No optimizations)", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (win == NULL) {
		printf("Failed to create window\n");
		return 1;
	}

	double scale = 3.0 / WIDTH; // SAVE AS FLOAT 2.0 NOT 2
	double origin_x = -0.5;
	double origin_y = 0;

	//SDL_Surface* surf_big = SDL_CreateRGBSurface(0,10000,10000,32,0,0,0,0);
	//scale = 3.0 / 10000.0;
	//draw_mandelbrot(surf_big, scale, origin_x, origin_y);
	//SDL_SaveBMP(surf_big, "Mandelbrot.bmp");
	//SDL_FreeSurface(surf_big);
	//goto QUIT;

	while (1) {
		SDL_Surface* surf = SDL_GetWindowSurface(win);
		//printf("Bytes per pixel: %u\n", surf->format->BytesPerPixel);
		SDL_LockSurface(surf);
		draw_mandelbrot(surf, scale, origin_x, origin_y);
		SDL_UnlockSurface(surf);
		SDL_UpdateWindowSurface(win);

		SDL_Delay(33);

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_MOUSEWHEEL:
					//printf("scale: %lf, wheel: %f\n", scale, event.wheel.preciseY);
					scale += -1 * event.wheel.preciseY * (SCROLL_POW/100.0) * scale;
					break;
				case SDL_MOUSEMOTION:
					if (event.motion.state & SDL_BUTTON_LMASK) {
						origin_x += -1 * scale * event.motion.xrel;
						origin_y += scale * event.motion.yrel;
					}
					break;
				case SDL_QUIT:
					goto QUIT;
			}
		}
	}

QUIT:
	SDL_DestroyWindow(win);
	SDL_Quit();
}
