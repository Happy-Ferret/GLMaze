#include "mesh.h"

#include <stdlib.h>
#include <MathLib.h>

#include <stdio.h>

Mesh* mesh_create_maze(Maze *maze)
{
	Mesh *mesh = malloc(sizeof(Mesh));
	
	mesh->vertex_format = VERTEX_POSITION | VERTEX_TEXCOORD;
	mesh->vertices_count = ((maze->width+1)*(maze->height+1)*2);
	mesh->vertices = malloc(sizeof(float) * mesh->vertices_count * (3+2));
	int x, y, z;
	float *v = mesh->vertices;
	for(y=0; y<2; y++) for(z=0; z<(maze->height+1); z++) for(x=0; x<(maze->width+1); x++)
	{
		//position:
		*v++ = x;
		*v++ = y;
		*v++ = z;
		//texcoord:
		*v++ = x+z;
		*v++ = y;
	}
	
	mesh->indices_count = (maze->width*(maze->height+1)+(maze->width+1)*maze->height) * (2*3);
	mesh->indices_count -= ((maze->width*maze->height) - 1) * (2*3); //passages
	mesh->indices = malloc(sizeof(unsigned int) * mesh->indices_count);
	unsigned int *i = mesh->indices;
	//horizontal walls
	for(y=0; y<maze->height+1; y++) for(x=0; x<maze->width; x++)
	{
		if(y < maze->height)
		{
			Cell *cell = maze_get_cell(maze, x, y);
			if(cell->up) continue;
		}
		
		unsigned int origin = y*(maze->width+1)+x;
		*i++ = origin;
		*i++ = origin + 1;
		*i++ = origin + (maze->height+1) * (maze->width+1);
		
		*i++ = origin + 1;
		*i++ = origin + (maze->height+1) * (maze->width+1) + 1;
		*i++ = origin + (maze->height+1) * (maze->width+1);
	}
	//vertical walls
	for(y=0; y<maze->height; y++) for(x=0; x<maze->width+1; x++)
	{
		if(x < maze->width)
		{
			Cell *cell = maze_get_cell(maze, x, y);
			if(cell->left) continue;
		}
		
		unsigned int origin = y*(maze->width+1)+x;
		*i++ = origin;
		*i++ = origin + (maze->width+1);
		*i++ = origin + (maze->height+1) * (maze->width+1);
		
		*i++ = origin + (maze->width+1);
		*i++ = origin + (maze->height+1) * (maze->width+1) + (maze->width+1);
		*i++ = origin + (maze->height+1) * (maze->width+1);
	}
	
	return mesh;
}

Mesh* mesh_create_quad(float x_scale, float z_scale)
{
	Mesh *mesh = malloc(sizeof(Mesh));
	
	mesh->vertex_format = VERTEX_POSITION | VERTEX_TEXCOORD;
	mesh->vertices_count = 4;
	mesh->vertices = malloc(sizeof(float) * (3+2) * mesh->vertices_count); //position/texcoord
	int x, z;
	float *v = mesh->vertices;
	for(x=0; x<2; x++) for(z=0; z<2; z++)
	{
		//position:
		*v++ = x * x_scale;
		*v++ = 0;
		*v++ = z * z_scale;
		//texcoord:
		*v++ = x * x_scale;
		*v++ = z * z_scale;
	}
	
	mesh->indices_count = 2*3;
	mesh->indices = malloc(sizeof(unsigned int) * mesh->indices_count);
	unsigned int *i = mesh->indices;
	*i++ = 0; *i++ = 1; *i++ = 2;
	*i++ = 1; *i++ = 3; *i++ = 2;
	
	return mesh;
}

Mesh* mesh_create_pyramid(float scale)
{
	Mesh *mesh = malloc(sizeof(Mesh));
	
	mesh->vertex_format = VERTEX_POSITION;
	mesh->vertices_count = 4;
	mesh->vertices = malloc(sizeof(float) * (3) * mesh->vertices_count);
	float *v = mesh->vertices;
	
	#define V(a,b,c) *v++ = a scale; *v++ = b scale; *v++ = c scale;
	//left down front
	V(-,-,+)
	//right down back
	V(+,-,-)
	//left top back
	V(-,+,-)
	//right top front
	V(+,+,+)
	#undef V
	
	mesh->indices_count = 4*3;
	mesh->indices = malloc(sizeof(unsigned int) * mesh->indices_count);
	unsigned int *i = mesh->indices;
	*i++ = 0; *i++ = 1; *i++ = 3;
	*i++ = 0; *i++ = 3; *i++ = 2;
	*i++ = 1; *i++ = 2; *i++ = 3;
	*i++ = 0; *i++ = 2; *i++ = 1;
	
	return mesh;
}

Mesh* mesh_create_screen_square()
{
	Mesh *mesh = malloc(sizeof(Mesh));
	
	mesh->vertex_format = VERTEX_POSITION;
	mesh->vertices_count = 4;
	mesh->vertices = malloc(sizeof(float) * (3) * mesh->vertices_count);
	float *v = mesh->vertices;
	*v++ = -1.0; *v++ = -1.0; *v++ = 0.0;
	*v++ = 1.0; *v++ = -1.0; *v++ = 0.0;
	*v++ = 1.0; *v++ = 1.0; *v++ = 0.0;
	*v++ = -1.0; *v++ = 1.0; *v++ = 0.0;
	
	mesh->indices_count = 2*3;
	mesh->indices = malloc(sizeof(unsigned int) * mesh->indices_count);
	unsigned int *i = mesh->indices;
	*i++ = 0; *i++ = 1; *i++ = 2;
	*i++ = 0; *i++ = 2; *i++ = 3;
	
	return mesh;
}

int mesh_get_vertex_size(Mesh *mesh)
{
	int size = 0;
	if(mesh->vertex_format & VERTEX_POSITION) size += 3;
	if(mesh->vertex_format & VERTEX_NORMAL) size += 3;
	if(mesh->vertex_format & VERTEX_TEXCOORD) size += 2;
	return size;
}

void mesh_save(Mesh *mesh, char *filename)
{
	FILE *file = fopen(filename, "w");
	int vertex_size = mesh_get_vertex_size(mesh);
	int i;
	if(mesh->vertex_format & VERTEX_POSITION)
	{
		for(i=0; i<mesh->vertices_count; i++)
		{
			float *v = &mesh->vertices[i*vertex_size];
			fprintf(file, "v %f %f %f\n", v[0], v[1], v[2]);
		}
	}
	for(i=0; i<(mesh->indices_count/3); i++)
	{
		unsigned int *in = &mesh->indices[i*3];
		fprintf(file, "f %d %d %d\n", in[0]+1, in[1]+1, in[2]+1);
	}
	fclose(file);
}

void mesh_free(Mesh *mesh)
{
	free(mesh->vertices);
	free(mesh->indices);
	free(mesh);
}
