#include "maze.h"
#include "mesh.h"
#include "camera.h"
#include "walker.h"

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

float t = 0.0;

void camera_update_pos(float pos[3])
{
	pos[1] = 0.5;
	camera_set_position(pos);
}

int main()
{
	srand(time(NULL));
	Maze *maze = maze_generate(10, 10);
	maze_print(maze);
	Mesh *mesh = mesh_create_maze(maze);
	Mesh *plane = mesh_create_quad((float)maze->width, (float)maze->height);
	Mesh *pyramid = mesh_create_pyramid(0.2);
	int start[2] = {5, 5};
	Walker *walker = walker_create(maze, start, UP, camera_update_pos, camera_set_rotation);
	
	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetVideoMode(500, 500, 32, SDL_OPENGL);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	gluPerspective(60.0, 1.0, 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glPointSize(10.0);
	
	glEnable(GL_LIGHT0);
	GLfloat light_pos[] = {0.0, 1.0, 0.0, 0.0};
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	GLfloat light_ambient[] = {1.0, 1.0, 1.0, 1.0};
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	GLfloat mat_ambient[] = {0.3, 0.3, 0.3, 1.0};
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	
	texture_init();
	GLuint wall_texture = texture_create("wall.jpg");
	GLuint ceiling_texture = texture_create("ceiling.jpg");
	GLuint floor_texture = texture_create("floor.jpg");
	
	char quit = 0;
	while(!quit)
	{
		SDL_Event ev;
		while(SDL_PollEvent(&ev))
		{
			if(ev.type == SDL_QUIT) quit = 1;
		}
		
		walker_step(walker, 0.02);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		float m[16];
		camera_get_matrix(m);
		glLoadMatrixf(m);
		
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, floor_texture);
		mesh_draw(plane);
		glTranslatef(0.0, 1.0, 0.0);
		glBindTexture(GL_TEXTURE_2D, ceiling_texture);
		mesh_draw(plane);
		glPopMatrix();
		
		glColor3f(1.0, 1.0, 1.0);
		glBindTexture(GL_TEXTURE_2D, wall_texture);
		mesh_draw(mesh);
		
		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		int x, y;
		for(y=0; y<maze->height; y++) for(x=0; x<maze->width; x++)
		{
			Cell *cell = maze_get_cell(maze, x, y);
			if(cell->object == OBJ_TWISTER)
			{
				glPushMatrix();
				glTranslatef(cell->x+0.5, 0.5, cell->y+0.5);
				glRotatef(t, 0.0, 1.0, 0.0);
				glRotatef(t*0.7, 1.0, 0.0, 0.0);
				mesh_draw(pyramid);
				glPopMatrix();
			}
		}
		glPopAttrib();
		
		GLuint error = glGetError();
		if(error) printf("OpenGL Error: %s\n", gluErrorString(error));
		
		SDL_GL_SwapBuffers();
		SDL_Delay(20);
		t++;
	}
	
	SDL_Quit();
	return 0;
}
