#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <complex.h>

#define WIDTH 500
#define DELAY 3000

#define DEGREE 3
#define PI 3.14159265358
#define MAX_ITER 1000
#define TOL 1e-6
#define ABS_TOL 1e20f
#define ORG_TOL 1e-6f

// perform newton iteration
static inline complex float newton_iteration(complex float x0, complex float x0i) {
    switch(DEGREE) {
        complex float x1;
        complex float x2;
        case 1:
             return 1.0f;
        case 2:
             return 1.f/2.f*x0 + 1.f/2.f*x0i;
        case 3:
             return 2.f/3.f*x0 + 1.f/3.f*x0i*x0i;
        case 4:
             return 3.f/4.f*x0 + 1.f*4.f*x0i*x0i*x0i;
        case 5:
             x1 = x0i*x0i;
             return 4.f/5.f*x0 + 1.f/5.f*x1*x1;
        case 6:
             x1 = x0i*x0i;
             return 5.f/6.f*x0 + 1.f/6.f*x1*x1*x0i;
        case 7:
             x1 = x0i*x0i;
             return 6.f/7.f*x0 + 1.f/7.f*x1*x1*x1;
        case 8:
             x1 = x0i*x0i;
             return 7.f/8.f*x0 + 1.f/8.f*x1*x1*x1*x0i;
        case 9:
             x1 = x0i*x0i;
             x2 = x1*x1;
             return 8.f/9.f*x0 + 1.f/9.f*x2*x2;
        default:
            printf("invalid degree");
            exit(1);
    }
}

// Fill convergences and attractors matrices with correct values
void calculate(float* x0s, complex float* roots, char* convergences, char* attractors) {
    for (int ix = 0; ix < WIDTH; ix += 1) {
	for (int jx = 0; jx < WIDTH; jx++) {
	    complex float x0 = x0s[jx] + x0s[ix]*I; 
	    short number_of_iters, attr = 9; // default corresponds to extra root
	    for (int conv = 0; conv < MAX_ITER ; ++conv) {
		float re = creal(x0), im = cimag(x0);
		float re_sq = re*re, im_sq = im*im;
		if (re > ABS_TOL || im > ABS_TOL) {
		    attr = DEGREE;
		    number_of_iters = conv;
		    break;
		}
		float abs_sq = re_sq + im_sq;
		if (abs_sq < ORG_TOL) {
		    attr = DEGREE;
		    number_of_iters = conv;
		    break;
		} 
		for (short ind = 0; ind < DEGREE; ++ind) {
		    complex float root = roots[ind];
		    complex float diff = x0-root;
		    float re_diff = creal(diff);
		    float im_diff = cimag(diff);
		    if (re_diff*re_diff + im_diff*im_diff  < TOL) {
		        attr = ind;
			number_of_iters = conv;
			break;
		    }
		    }
		    if (attr != 9) {
		        break;
		    }
		    complex float x0i = (re-im*I) / abs_sq; // z^-1 = conj(z)/|z|^2
		    x0 = newton_iteration(x0, x0i);
	    }
	    attractors[ix*WIDTH+jx] = attr;
	    convergences[ix*WIDTH+jx] = number_of_iters < 50 ? number_of_iters : 49;
	} 
    }
}

// Decide what color a pixel should have given its root and number of iterations needed to reach it
Uint32 get_color(SDL_Surface* surface, char conv, char attr) {
    int r, g, b;
    switch(attr) {
        case 0:
	    r=255;g=0;b=0;
	    break;
        case 1:
	    r=0;g=255;b=0;
	    break;
        case 2:
	    r=0;g=0;b=255;
	    break;
        case 3:
	    r=255;g=255;b=0;
	    break;
        case 4:
	    r=255;g=0;b=255;
	    break;
        case 5:
	    r=0;g=255;b=255;
	    break;
        case 6:
	    r=128;g=128;b=255;
	    break;
        case 7:
	    r=255;g=128;b=0;
	    break;
        case 8:
	    r=255;g=0;b=128;
	    break;
        default:
            printf("invalid degree");
            //exit(1);
    }
    // Create an SDL color value from the RGB values
    Uint32 pixel = SDL_MapRGB(surface->format, r*(conv+10)/55, g*(conv+10)/55, b*(conv+10)/55);
    return pixel;
}

int main (int argc, char** argv) {
    char* attractors = calloc(WIDTH*WIDTH, sizeof(char));
    char* convergences = calloc(WIDTH*WIDTH, sizeof(char));

    // precompute all the roots to x^DEGREE = 1
    complex float* roots = malloc(DEGREE*sizeof(complex float));
    for (short n = 0; n < DEGREE; ++n) {
        float theta = 2*PI*n/DEGREE;
        float re = cos(theta);
        float im = sin(theta);
        roots[n] = re + im*I;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Initialization error\n");
	exit(1);
    }
    SDL_Window * window = SDL_CreateWindow
    (
        "Graphic window!", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        1000,
        1000,
        SDL_WINDOW_SHOWN
    );

    // Setup renderer
    SDL_Renderer* renderer = NULL;
    renderer =  SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);

    // Set render color to red ( background will be rendered in this color )
    SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );

    // Clear winow
    SDL_RenderClear( renderer );
    SDL_Surface* surface1 = SDL_CreateRGBSurface(0, WIDTH, WIDTH, 32, 0, 0, 0, 0);
    SDL_LockSurface(surface1);
    
    float* x0s = malloc(WIDTH*sizeof(float)); // construct x0 matrix 
    int xmouse, ymouse;
    float xstart, xstop;
    for (int ix = 0; ix < 20; ix++){
        Uint32 mouse_state = SDL_GetMouseState(&xmouse, &ymouse);
	if (SDL_PointInRect(&(SDL_Point){xmouse, ymouse}, &(SDL_Rect){1, 1, WIDTH, WIDTH})) {
	    xstart = -xmouse*(20-ix)/20.f;
	    xstop = -xmouse*(20-ix)/20.f;
        } else {
	    xstart = -2.f*(20-ix)/20.f;
	    xstop = 2.f*(20-ix)/20.f;
	}
        // Choose grid size
	// Fill the array with equidistant points
        float jx = xstart + xstop / (float) WIDTH;
        float step = (xstop-xstart) / (float) WIDTH;
	for (int ix = 0; ix < WIDTH; ++ix) {
	    x0s[ix] = jx;
	    jx += step;
	}
	calculate(x0s, roots, convergences, attractors);
        for (int y = 0; y < WIDTH; y++) {
	    for (int x = 0; x < WIDTH; x++) {
	        // Set pixel values for the original surface
		int ix1 = y*WIDTH+x;
	        int conv = convergences[ix1];
	        int attr = attractors[ix1];
	        Uint32 pixel = get_color(surface1, conv, attr);
	        ((Uint32*)surface1->pixels)[ix1] = pixel;
	    }
        }
	SDL_UnlockSurface(surface1);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface1);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
    // Wait for DELAY ms
    SDL_Delay(DELAY);

    SDL_DestroyWindow(window);
    SDL_Quit();

    free(convergences);
    free(attractors);
    return EXIT_SUCCESS;
}
