#include "transformations.h"

void translate(double x, double y, double z, double *vertexes, int size) {
  translate_projections(x, vertexes, size, 0);
  translate_projections(y, vertexes, size, 1);
  translate_projections(z, vertexes, size, 2);
}

void translate_projections(double x, double *vertexes, int size, int n) {
  if (x != 0) {
    for (int i = n; i < size; i += 3) {
      vertexes[i] += x;
    }
  }
}
void translateX(double *pre_x, double curr_x, double *pre_y, double curr_y,
                double *pre_z, double curr_z, double *vertexes, int size) {
  translate(curr_x - *pre_x, curr_y - *pre_y, curr_z - *pre_z, vertexes, size);
  *pre_x = curr_x;
  *pre_y = curr_y;
  *pre_z = curr_z;
}

void rotate(double x, double y, double z, double *vertexes, int size) {
  double tmp1 = 0, tmp2 = 0, pi = 3.14159265358979323846;
  if (x != 0) {
    x = x * pi / 180;
    double sinx = sin(x);
    double cosx = cos(x);
    for (int i = 0; i < size; i += 3) {
      tmp1 = vertexes[i + 1] * cosx - vertexes[i + 2] * sinx;
      tmp2 = vertexes[i + 1] * sinx + vertexes[i + 2] * cosx;
      vertexes[i + 1] = tmp1;
      vertexes[i + 2] = tmp2;
    }
  }
  if (y != 0) {
    y = y * pi / 180;
    double siny = sin(y);
    double cosy = cos(y);
    for (int i = 0; i < size; i += 3) {
      tmp1 = vertexes[i] * cosy + vertexes[i + 2] * siny;
      tmp2 = vertexes[i + 2] * cosy - vertexes[i] * siny;
      vertexes[i] = tmp1;
      vertexes[i + 2] = tmp2;
    }
  }
  if (z != 0) {
    z = z * pi / 180;
    double sinz = sin(z);
    double cosz = cos(z);
    for (int i = 0; i < size; i += 3) {
      tmp1 = vertexes[i] * cosz - vertexes[i + 1] * sinz;
      tmp2 = vertexes[i] * sinz + vertexes[i + 1] * cosz;
      vertexes[i] = tmp1;
      vertexes[i + 1] = tmp2;
    }
  }
}

void rotateX(double *prev_x, double curr_x, double *prev_y, double curr_y,
             double *prev_z, double curr_z, double *vertexes, int size) {
  rotate(0, 0, -*prev_z, vertexes, size);
  rotate(0, -*prev_y, 0, vertexes, size);
  rotate(-*prev_x, 0, 0, vertexes, size);
  rotate(curr_x, 0, 0, vertexes, size);
  rotate(0, curr_y, 0, vertexes, size);
  rotate(0, 0, curr_z, vertexes, size);
  *prev_x = curr_x;
  *prev_y = curr_y;
  *prev_z = curr_z;
}

void scale(double x, double y, double z, double *vertexes, int size) {
  scale_X(x, vertexes, size);
  scale_X(y, vertexes, size);
  scale_X(z, vertexes, size);
}

void scale_X(double x, double *vertexes, int size) {
  if (x != 0) {
    for (int i = 0; i < size; i += 3) {
      vertexes[i] *= x;
    }
  }
}
void scaleX(double *pre_scale, double *curr_scale, double *vertexes, int size) {
  if ((*pre_scale != *curr_scale) && (*curr_scale != 0)) {
    for (int i = 0; i < size; i++) {
      vertexes[i] = (vertexes[i] / *pre_scale) * *curr_scale;
    }
    *pre_scale = *curr_scale;
  }
}
