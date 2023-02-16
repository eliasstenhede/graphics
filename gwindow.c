#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <complex.h>

#define WIDTH 400
#define WINSIZE 800
#define DELAY 3000

#define DEGREE 6
#define PI 3.14159265358
#define MAX_ITER 1000
#define TOL 1e-6
#define ABS_TOL 1e20f
#define ORG_TOL 1e-6f

// perform newton iteration
static inline complex double newton_iteration(complex double x0, complex double x0i, complex double alpha) {
    switch(DEGREE) {
        complex double x1;
        complex double x2;
        case 1:
             return 1.0f;
        case 2:
	     return x0 - alpha*(x0-1.f)*x0i/2.f;
             //return 1.f/2.f*x0 + 1.f/2.f*x0i;
        case 3:
	     return x0 - alpha*(x0*x0*x0-1.f)/(3.f*x0*x0);
             //return 2.f/3.f*x0 + 1.f/3.f*x0i*x0i;
        case 4:
             return x0 - alpha*(x0*x0*x0*x0-1.f)*x0i*x0i*x0i/4.f;
	     //return 3.f/4.f*x0 + 1.f*4.f*x0i*x0i*x0i;
        case 5:
	     return x0 - alpha*(x0*x0*x0*x0*x0-1.f)*x0i*x0i*x0i*x0i/5.f;
             //x1 = x0i*x0i;
             //return 4.f/5.f*x0 + 1.f/5.f*x1*x1;
        case 6:
	     return x0 - alpha*(x0*x0*x0*x0*x0*x0-1.f)*x0i*x0i*x0i*x0i*x0i/6.f;
             //x1 = x0i*x0i;
             //return 5.f/6.f*x0 + 1.f/6.f*x1*x1*x0i;
        case 7:
	     return x0 - alpha*(x0*x0*x0*x0*x0*x0*x0-1.f)*x0i*x0i*x0i*x0i*x0i*x0i/7.f;
             //x1 = x0i*x0i;
             //return 6.f/7.f*x0 + 1.f/7.f*x1*x1*x1;
        case 8:
	     return x0 - alpha*(x0*x0*x0*x0*x0*x0*x0*x0-1.f)*x0i*x0i*x0i*x0i*x0i*x0i*x0i/8.f;
             //x1 = x0i*x0i;
             //return 7.f/8.f*x0 + 1.f/8.f*x1*x1*x1*x0i;
        case 9:
	     return x0 - alpha*(x0*x0*x0*x0*x0*x0*x0*x0*x0-1.f)*x0i*x0i*x0i*x0i*x0i*x0i*x0i*x0i/9.f;
	     //return x0 - alpha*(x0*x0*x0*x0*x0*x0*x0*-1.f)*x0i*x0i*x0i*x0i*x0i*x0i/7.f;
             //x1 = x0i*x0i;
             //x2 = x1*x1;
             //return 8.f/9.f*x0 + 1.f/9.f*x2*x2;
        default:
            printf("invalid degree");
            exit(1);
    }
}

// Fill convergences and attractors matrices with correct values
void calculate(float* x0s, float* y0s, complex double* roots, char* convergences, char* attractors) {
    for (int ix = 0; ix < WIDTH; ix += 1) {
	for (int jx = 0; jx < WIDTH; jx++) {
	    complex double x0 = x0s[jx] + y0s[ix]*I; 
	    short number_of_iters = 0;
	    short attr = 9; // default corresponds to extra root
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
		    complex double root = roots[ind];
		    complex double diff = x0-root;
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
		    complex double x0i = (re-im*I) / abs_sq; // z^-1 = conj(z)/|z|^2
		    complex double alpha = 0.9 + 0.4*I;
		    x0 = newton_iteration(x0, x0i, alpha);
	    }
	    attractors[ix*WIDTH+jx] = attr;
	    convergences[ix*WIDTH+jx] = number_of_iters < 50 ? number_of_iters : 50;
	} 
    }
}

// Decide what color a pixel should have given its root and number of iterations needed to reach it
Uint32 get_color(SDL_Surface* surface, char conv, char attr) {
    int r, g, b;
    switch(attr) {
        case 0:
	    r=101;g=116;b=205;
	    break;
        case 1:
	    r=255;g=128;b=0;
	    break;
        case 2:
	    r=246;g=109;b=155;
	    break;
        case 3:
	    r=52;g=144;b=220;
	    break;
        case 4:
	    r=77;g=192;b=181;
	    break;
        case 5:
	    r=56;g=193;b=114;
	    break;
        case 6:
	    r=255;g=237;b=74;
	    break;
        case 7:
	    r=149;g=97;b=226;
	    break;
        case 8:
	    r=255;g=0;b=128;
	    break;
        default:
            printf("invalid degree");
            exit(1);
    }
    // Create an SDL color value from the RGB values
    int darkness_param = 50;
    Uint32 pixel = SDL_MapRGB(surface->format, r*(conv+1)/darkness_param, g*(conv+1)/darkness_param, b*(conv+1)/darkness_param);
    return pixel;
}

int main (int argc, char** argv) {
    char* attractors = calloc(WIDTH*WIDTH, sizeof(char));
    char* convergences = calloc(WIDTH*WIDTH, sizeof(char));

    // precompute all the roots to x^DEGREE = 1
    complex double* roots = malloc(DEGREE*sizeof(complex double));
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
        WINSIZE,
        WINSIZE,
        SDL_WINDOW_SHOWN
    );
    SDL_Event event;

    // Setup renderer
    SDL_Renderer* renderer = NULL;
    renderer =  SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);

    // Set render color to red ( background will be rendered in this color )
    SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );

    // Clear winow
    SDL_RenderClear( renderer );
    SDL_Surface* surface1 = SDL_CreateRGBSurface(0, WIDTH, WIDTH, 32, 0, 0, 0, 0);
    SDL_LockSurface(surface1);
    
    float* x0s = malloc(WIDTH*sizeof(float)); // construct x0 vector 
    float* y0s = malloc(WIDTH*sizeof(float)); // construct y0 vector 
    float xstart, xmiddle, xstop, jx;
    float ystart, ymiddle, ystop, jy;
    float stepsize;
    float complex_span = 10.f;
    int xmouse, ymouse;
    int extra_zoom = 0;
    while (1) {
	//Check if user is trying to close the window?
	while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    SDL_DestroyWindow(window);
                    SDL_Quit();
                    return 0;
                    break;
		case SDL_MOUSEBUTTONDOWN:
		    if (event.button.button == SDL_BUTTON_LEFT) {
                        extra_zoom = 1;
                    }
                    break;
                default:
                    break;
            }
        }
	// Get position of cursor
	SDL_PumpEvents();
        SDL_GetMouseState(&xmouse, &ymouse);
        
	// Calculate the surface with the cursor as middle point and zoom one step
	complex_span *= 0.995;
	if (extra_zoom) {
	    complex_span *= 0.8;
	    extra_zoom = 0;
	}
	xmiddle = x0s[xmouse*WIDTH/WINSIZE]/1.2f;
	ymiddle = y0s[ymouse*WIDTH/WINSIZE]/1.2f;
	xstart = xmiddle - complex_span/2.f;
	xstop  = xmiddle + complex_span/2.f;
	ystart = ymiddle - complex_span/2.f;
	ystop  = ymiddle + complex_span/2.f;
	// Fill the array with equidistant points
        jx = xstart + xstop/(float)WIDTH;
        jy = ystart + ystop/(float)WIDTH;
        stepsize = complex_span/(float)WIDTH;
	for (int ix = 0; ix < WIDTH; ++ix) {
	    x0s[ix] = jx;
	    y0s[ix] = jy;
	    jx += stepsize;
	    jy += stepsize;
	}
	calculate(x0s, y0s, roots, convergences, attractors);
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
    // SDL_Delay(DELAY);

    SDL_DestroyWindow(window);
    SDL_Quit();

    free(x0s);
    free(y0s);
    free(roots);
    free(convergences);
    free(attractors);
    return EXIT_SUCCESS;
}
