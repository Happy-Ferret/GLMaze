#include "noise.h"

#include <stdlib.h>
#include <math.h>

static float interpolate(float x, float a, float b)
{
	x = cos(x*M_PI) * -0.5 + 0.5;
	return a + x * (b-a);
}

void noise_generate_texture2d_channel(int gridsize, int width, int height, int stride, float *data)
{
	if(stride == 0) stride = 1;
	float noise_grid[gridsize][gridsize];
	int i, j;
	for(i=0; i<gridsize; i++) for(j=0; j<gridsize; j++) noise_grid[i][j] = rand()/(float)RAND_MAX;
	
	for(j=0; j<height; j++) for(i=0; i<width; i++)
	{
		int x1 = (int)floor(i/(width/gridsize)) % gridsize;
		int x2 = (x1+1) % gridsize;
		int y1 = (int)floor(j/(height/gridsize)) % gridsize;
		int y2 = (y1+1) % gridsize;
		float xfac = (i%(width/gridsize))/((float)(width/gridsize));
		float yfac = (j%(height/gridsize))/((float)(height/gridsize));
		float v1 = interpolate(xfac, noise_grid[x1][y1], noise_grid[x2][y1]);
		float v2 = interpolate(xfac, noise_grid[x1][y2], noise_grid[x2][y2]);
		float value = interpolate(yfac, v1, v2);
		data[(i+j*width)*stride] = value;
	}
}
