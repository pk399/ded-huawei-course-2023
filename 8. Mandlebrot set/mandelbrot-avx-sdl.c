#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <immintrin.h>
#include <stdint.h>

#include "gettime.h"

//const unsigned TARGET_FPS = 30;
const long unsigned CPU_FREQ = 2.3e9; // Intel Core i7-3610QM

const int MAXLOOP = 255;
const double SCROLL_POW = 10.0;

const int FONTSIZE = 32;
const SDL_Color TEXTCOL = {0, 255, 32};

const int WIDTH = 1280;
const int HEIGHT = 720;

const double R2 = 10.0*10.0;

#define CX(x) ((x - WIDTH/2) * scale + origin_x)
#define CY(y) ((HEIGHT/2 - y) * scale + origin_y)

void draw_mandelbrot(SDL_Surface* surf, double scale, double origin_x, double origin_y) {
	Uint32* pixbuf = surf->pixels;
	for (int xi = 0; xi < surf->w - 3; xi += 4) {
		for (int yi = 0; yi < surf->h; yi++) {
			__m256d x, y, x0, y0;
			x0 = _mm256_set_pd(CX(xi + 3), CX(xi + 2), CX(xi + 1), CX(xi));
			y0 = _mm256_set1_pd(CY(yi));
			x = x0;
			y = y0;

			__m256d one, two, r2;
			one = _mm256_set1_pd(1.0);
			two = _mm256_set1_pd(2.0);
			r2 = _mm256_set1_pd(R2);

			__m256d n, mask;
			n = _mm256_setzero_pd();
			mask = _mm256_set1_pd(1337.0); // a non-zero number
			for ( int i = 0; i < MAXLOOP && !_mm256_testz_si256(_mm256_castpd_si256(mask), _mm256_castpd_si256(mask)); i++ ) {
				__m256d x2, xy, y2;
				x2 = _mm256_mul_pd(x, x);
				xy = _mm256_mul_pd(x, y);
				y2 = _mm256_mul_pd(y, y);

				mask = _mm256_cmp_pd(_mm256_add_pd(x2, y2), r2, 1);
				n = _mm256_add_pd(_mm256_and_pd(mask, one), n);

				x = _mm256_sub_pd(_mm256_add_pd(x2, x0), y2);
				y = _mm256_add_pd(_mm256_mul_pd(two, xy), y0);
			}

			double* nd = (double*) &n;
			for (int i = 0; i < 4; i++) {
				unsigned ni = nd[i];
				if (ni == MAXLOOP)
					pixbuf[surf->w*yi + xi + i] = SDL_MapRGBA(surf->format, 0, 0, 0, 255);
				else
					pixbuf[surf->w*yi + xi + i] = SDL_MapRGBA(surf->format, (ni << 1) & ni & 255, 16 * ni%16, ni, 255);
			}
		}
	}
}

int main() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) || TTF_Init()) {
		printf("Init fail\n");
		return 1;
	}

	SDL_Window* win = SDL_CreateWindow("Mandelbrot set (AVX)", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (win == NULL) {
		printf("Failed to create window\n");
		return 1;
	}

	TTF_Font* mc_font = TTF_OpenFont("minecraft.ttf", FONTSIZE);
	if (mc_font == NULL) {
		printf("Failed to open font\n");
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
	uint64_t time = gettime();
	while (1) {
		SDL_Surface* surf = SDL_GetWindowSurface(win);
		//printf("Bytes per pixel: %u\n", surf->format->BytesPerPixel);
		SDL_LockSurface(surf);

		draw_mandelbrot(surf, scale, origin_x, origin_y);

		uint64_t new_time = gettime();
		char fps_text[1024] = "";
		unsigned cur_fps = CPU_FREQ/(new_time - time);
		sprintf(fps_text, "~%u FPS", cur_fps);
		time = new_time;

		SDL_Surface* text_surf = TTF_RenderText_Solid(mc_font, fps_text, TEXTCOL);
		
		SDL_Rect dst_rect = {0, 0, text_surf->w, text_surf->h};
		SDL_UnlockSurface(surf);
		SDL_BlitSurface(text_surf, NULL, surf, &dst_rect);

		SDL_UpdateWindowSurface(win);

		//if (cur_fps > TARGET_FPS) {
		//	SDL_Delay(1000/TARGET_FPS - 1000/cur_fps);printf("HI");
		//}

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_MOUSEWHEEL:
					double old_scale = scale;

					scale += -1 * event.wheel.preciseY * (SCROLL_POW/100.0) * scale;

					origin_x -= (scale - old_scale)*(event.wheel.mouseX - WIDTH/2);
					origin_y += (scale - old_scale)*(event.wheel.mouseY - HEIGHT/2);

					//printf("mx: %d; f0: %lf; f1: %lf\n", event.wheel.mouseX, fx0, fx1);
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
