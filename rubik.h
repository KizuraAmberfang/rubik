#ifndef RUBIK_H
# define RUBIK_H

enum color
{
	WHITE;
	BLUE;
	YELLOW;
	ORANGE;
	GREEN;
	RED;
}

typedef struct	s_vertex
{
	color	first;
	color	second;
	color	third;
}			t_vertex;

typedef struct	s_angle
{
	color	first;
	color	second;
}			t_angle;

typedef	struct s_face
{
	color	id;
	t_angle	*e;
	t_angle	*e;
	t_angle	*w;
	t_angle	*w;
	t_vertex	*ne;
	t_vertex	*se;
	t_vertex	*sw;
	t_vertex	*nw; 
	color	e_color;
	color	n_color;
	color	s_color;
	color	w_color;
	color	ne_color;
	color	nw_color;
	color	se_color;
	color	sw_color;
}			t_face;

typedef struct s_cube
{
	t_face	*front;
	t_face	*right;
	t_face	*back;
	t_face	*left;
	t_face	*upper;
	t_face	*down;
}			t_cube;

#endif