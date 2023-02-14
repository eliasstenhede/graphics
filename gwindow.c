#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <complex.h>

#define WIDTH 400
#define DELAY 5000

#define DEGREE 3
#define PI 3.14159265358
#define MAX_ITER 1000
#define TOL 1e-6
#define ABS_TOL 1e20f
#define ORG_TOL 1e-6f

// perform newton iteration until root is found
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

char* create_pixel_matrix() {
    // Allocate memory for the matrix
    char* matrix = calloc(WIDTH * WIDTH * 4, sizeof(char));
    
    // Center point of the circle
    int cx = WIDTH / 2;
    int cy = WIDTH / 2;
    
    // Radius of the circle
    int r = fmin(WIDTH, WIDTH) / 4;
    
    // Initialize the pixels
    for (int y = 0; y < WIDTH; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int i = (y * WIDTH + x) * 4;
            // Compute the distance from the current pixel to the center of the circle
            double d = sqrt(pow(x - cx, 2) + pow(y - cy, 2));
            if (d <= r) {
                // Inside the circle, set the pixel to red
                matrix[i] = 255;  // red
                matrix[i+3] = 255;  // alpha
            } else {
                // Outside the circle, set the pixel to black
                matrix[i+3] = 0;  // alpha
            }
	}
    }
    
    return matrix;
}

char* conv_attr_to_pixel_matrix(char* convergences, char* attractors) {
    // Allocate memory for the matrix
    char* matrix = calloc(WIDTH * WIDTH * 4, sizeof(char));
    // Initialize the pixels
    for (int y = 0; y < WIDTH; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int i = (y * WIDTH + x)*4;
	    matrix[i] = attractors[i]*5;  // red
	    //matrix[i+3] = 255;  // alpha
        }
    }
    return matrix;
}

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



int main (int argc, char** argv) {
    char* attractors = malloc(WIDTH*WIDTH*sizeof(char));
    char* convergences = malloc(WIDTH*WIDTH*sizeof(char));

    float* x0s = malloc(WIDTH*sizeof(float)); // construct x0 matrix 
    float jx = -2.f + 2.f / (float) WIDTH;
    float step = 4.f / (float) WIDTH;
    for (int ix = 0; ix < WIDTH; ++ix) {
       x0s[ix] = jx;
       jx += step;
    }
    // precompute all the roots to x^DEGREE = 1
    complex float* roots = malloc(DEGREE*sizeof(complex float));
    for (short n = 0; n < DEGREE; ++n) {
        float theta = 2*PI*n/DEGREE;
        float re = cos(theta);
        float im = sin(theta);
        roots[n] = re + im*I;
    }

    calculate(x0s, roots, convergences, attractors);
    //char* mat = create_pixel_matrix();
    char* mat = conv_attr_to_pixel_matrix(convergences, attractors);

    SDL_Window* window = NULL;
    window = SDL_CreateWindow
    (
        "Graphic window!", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        800,
        800,
        SDL_WINDOW_SHOWN
    );

    // Setup renderer
    SDL_Renderer* renderer = NULL;
    renderer =  SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);

    // Set render color to red ( background will be rendered in this color )
    SDL_SetRenderDrawColor( renderer, 0, 255, 0, 255 );

    // Clear winow
    SDL_RenderClear( renderer );

    // Create surface
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
		    mat,            // Pointer to the pixel data buffer
		    WIDTH,          // Width of the image
		    WIDTH,          // Height of the image
		    32,             // Number of bits used to represent each pixel
		    4*WIDTH,        // Number of bytes used to represent each row of pixels
		    0xff000000,     // Red mask (in this case, the most significant byte)
		    0x00ff0000,     // Green mask (in this case, the second most significant byte)
		    0x0000ff00,     // Blue mask (in this case, the third most significant byte)
		    0x000000ff      // Alpha mask (in this case, the least significant byte)
			);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    // Render the texture to the window
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    // Wait for 5 sec
    SDL_Delay( 5000 );

    SDL_DestroyWindow(window);
    SDL_Quit();

    free(mat);
    free(convergences);
    free(attractors);
    return EXIT_SUCCESS;
}
