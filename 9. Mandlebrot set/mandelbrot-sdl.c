#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdint.h>

#include "gettime.h"

const long unsigned CPU_FREQ = 2.3e9; // Intel Core i7-3610QM

const int MAXLOOP = 255;
const double SCROLL_POW = 10.0;

const int FONTSIZE = 32;
const SDL_Color TEXTCOL = {0, 255, 32};

const int WIDTH = 1280;
const int HEIGHT = 720;

const double R2 = 10.0*10.0;

#define CX(x) ((x - WIDTH/2) * scale + ox)
#define CY(y) ((HEIGHT/2 - y) * scale + oy)

void draw_mandelbrot(SDL_Surface* surf, double scale, double ox, double oy) {
	Uint32* pixbuf = surf->pixels;
	for (int x = 0; x < surf->w; x++) {
		for (int y = 0; y < surf->h; y++) {
			double x0 = CX(x);
			double y0 = CY(y);
			double xn_1 = x0, yn_1 = y0;
	//		printf("(%lf, %lf) ", xn_1, yn_1);

			int n = 0;
			for ( ; n < MAXLOOP; n++) {
				double xn = xn_1*xn_1 - yn_1*yn_1 + x0;
				double yn = 2*xn_1*yn_1 + y0;

				if (xn*xn + yn*yn > R2) break;
				
				xn_1 = xn;
				yn_1 = yn;
			}

			if (n == MAXLOOP)
				pixbuf[surf->w*y + x] = SDL_MapRGBA(surf->format, 0, 0, 0, 255);
			else
				pixbuf[surf->w*y + x] = SDL_MapRGBA(surf->format, (n << 1) & n & 255, 16 * n%16, n, 255);
		}
	}
}

int main() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) || TTF_Init()) {
		printf("Init fail\n");
		return 1;
	}

	SDL_Window* win = SDL_CreateWindow("Mandelbrot set (No optimizations)", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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
		SDL_UnlockSurface(surf);

		uint64_t new_time = gettime();
		char fps_text[1024];
		unsigned cur_fps = CPU_FREQ/(new_time - time);
		time = new_time;
		sprintf(fps_text, "~%u FPS", cur_fps);

		SDL_Surface* text_surf = TTF_RenderText_Solid(mc_font, fps_text, TEXTCOL);
		SDL_Rect dst_rect = {0, 0, text_surf->w, text_surf->h};
		SDL_BlitSurface(text_surf, NULL, surf, &dst_rect);

		SDL_UpdateWindowSurface(win);

		//SDL_Delay(33);

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_MOUSEWHEEL:
					//printf("scale: %lf, wheel: %f\n", scale, event.wheel.preciseY);
					double old_scale = scale;

					scale += -1 * event.wheel.preciseY * (SCROLL_POW/100.0) * scale;

					origin_x -= (scale - old_scale)*(event.wheel.mouseX - WIDTH/2);
					origin_y += (scale - old_scale)*(event.wheel.mouseY - HEIGHT/2);

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
