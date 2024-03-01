#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

#include <math.h>

void translate(double x, double y, double z, double *vertexes, int size);
void translate_projections(double x, double *vertexes, int size, int n);
void rotate(double x, double y, double z, double *vertexes, int size);
void scale(double x, double y, double z, double *vertexes, int size);
void scale_X(double x, double *vertexes, int size);
void scaleX(double *pre_scale, double *curr_scale, double *vertexes, int size);
void translateX(double *prev_x, double curr_x, double *prev_y, double curr_y,
                double *prev_z, double curr_z, double *vertexes, int size);
void rotateX(double *prev_x, double curr_x, double *prev_y, double curr_y,
             double *prev_z, double curr_z, double *vertexes, int size);
#endif  // TRANSFORMATIONS_H
