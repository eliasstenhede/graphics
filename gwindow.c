#include <SDL2/SDL.h>
#include <math.h>
#define HEIGHT 512
#define WIDTH 1028
#define DELAY 3000

char* create_pixel_matrix() {
    // Allocate memory for the matrix
    char* matrix = calloc(HEIGHT * WIDTH * 4, sizeof(char));
    
    // Center point of the circle
    int cx = WIDTH / 2;
    int cy = HEIGHT / 2;
    
    // Radius of the circle
    int r = fmin(WIDTH, HEIGHT) / 4;
    
    // Initialize the pixels
    for (int y = 0; y < HEIGHT; y++) {
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

int main (int argc, char** argv)
{
    char* mat = create_pixel_matrix();
    SDL_Window* window = NULL;
    window = SDL_CreateWindow
    (
        "Graphic window!", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        SDL_WINDOW_SHOWN
    );

    // Setup renderer
    SDL_Renderer* renderer = NULL;
    renderer =  SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);

    // Set render color to red ( background will be rendered in this color )
    SDL_SetRenderDrawColor( renderer, 0, 255, 0, 255 );

    // Clear winow
    SDL_RenderClear( renderer );


    /////////////////////
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
		    mat,            // Pointer to the pixel data buffer
		    WIDTH,          // Width of the image
		    HEIGHT,         // Height of the image
		    32, // Number of bits used to represent each pixel
		    4*WIDTH,        // Number of bytes used to represent each row of pixels
		    0xff000000,     // Red mask (in this case, the most significant byte)
		    0x00ff0000,     // Green mask (in this case, the second most significant byte)
		    0x0000ff00,     // Blue mask (in this case, the third most significant byte)
		    0x000000ff      // Alpha mask (in this case, the least significant byte)
			);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    // Render the texture to the window
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    
    /////////////////////
    // Creat a rect at pos ( 50, 50 ) that's 50 pixels wide and 50 pixels high.
    SDL_Rect r;
    r.x = 50;
    r.y = 50;
    r.w = 50;
    r.h = 50;

    // Set render color to blue ( rect will be rendered in this color )
    SDL_SetRenderDrawColor( renderer, 0, 0, 255, 255 );

    // Render rect
    SDL_RenderFillRect( renderer, &r );

    // Render the rect to the screen
    SDL_RenderPresent(renderer);

    // Wait for 5 sec
    SDL_Delay( 5000 );

    SDL_DestroyWindow(window);
    SDL_Quit();

    free(mat);
    return EXIT_SUCCESS;
}
