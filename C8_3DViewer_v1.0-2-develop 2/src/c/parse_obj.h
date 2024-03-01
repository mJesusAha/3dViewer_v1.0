#ifndef PARSE_OBJ_H
#define PARSE_OBJ_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _GNU_SOURSE

typedef struct {
  unsigned int vertexes;
  unsigned int edges;
  int v_ind;
  int f_ind;
} num_of;
typedef struct {
  double Xmin;
  double Xmax;
  double Ymin;
  double Ymax;
  double Zmin;
  double Zmax;
} coord;

int parse_obj(char *filename, unsigned int *num_of_vertexes,
              unsigned int *num_of_facets, double **vertexes, int **facets,
              coord *coord_v);
int counter_V_F(FILE *fp, num_of *sums);
void read_data(FILE *fp, double *vertexes, int *facets, coord *coord_v);
int read_vertex(double *vertexes, char *str, int *v_ind, coord *coord_V);
void vertex_set(int *facets, char *str, int *f_ind);

#endif  // PARSE_OBJ_H
