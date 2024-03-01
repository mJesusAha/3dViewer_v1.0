#include "parse_obj.h"
#define _GNU_SOURSE
#include <stdio.h>
#include <stdlib.h>
int counter_V_F(FILE *fp, num_of *sums) {
  int error = 0;
  size_t len = 0;
  char *line = NULL;
  while (!feof(fp)) {
    getline(&line, &len, fp);
    if ((line[0] == 'v') && (line[1] == ' ')) {
      sums->vertexes += 1;
    }
    if ((line[0] == 'f') && (line[1] == ' ')) {
      if (line[2] == '-') {
        printf("%s", line);
        error = error - 1111111111;
      }
      sums->edges += 1;
      int i = 1;
      while (line[i] != '\n') {
        int temp = (((line[i + 1]) >= 48) && ((line[i + 1]) <= 57));
        if ((line[i] == ' ') && (temp == 1)) {
          sums->edges += 1;
        }
        i++;
      }
    }
  }
  free(line);
  line = NULL;
  return error;
}

void read_data(FILE *fp, double *vertexes, int *facets, coord *coord_v) {
  int v_ind = 0, f_ind = 0;
  size_t len = 0;
  char *str = NULL;
  while (!feof(fp)) {
    getline(&str, &len, fp);
    if ((str[0] == 'v') && (str[1] != 'n')) {
      read_vertex(vertexes, str, &v_ind, coord_v);
    }
    if (str[0] == 'f') {
      vertex_set(facets, str, &f_ind);
    }
  }
  free(str);
  str = NULL;
}
int read_vertex(double *vertexes, char *str, int *index, coord *coord_v) {
  int error = 0;
  double x = 0, y = 0, z = 0;
  int num_of_digits_read = sscanf(str, "v %lf %lf %lf", &x, &y, &z);
  if (num_of_digits_read == 3) {
    vertexes[*index] = x;
    coord_v->Xmin = (x < coord_v->Xmin) ? x : coord_v->Xmin;
    coord_v->Xmax = (x > coord_v->Xmax) ? x : coord_v->Xmax;
    vertexes[*index + 1] = y;
    coord_v->Ymin = (y < coord_v->Ymin) ? y : coord_v->Ymin;
    coord_v->Ymax = (y > coord_v->Ymax) ? y : coord_v->Ymax;
    vertexes[*index + 2] = z;
    coord_v->Zmin = (z < coord_v->Zmin) ? z : coord_v->Zmin;
    coord_v->Zmax = (z > coord_v->Zmax) ? z : coord_v->Zmax;
    *index += 3;
  } else {
    error = 1;
  }
  return error;
}

void vertex_set(int *facets, char *str, int *f_ind) {
  int j = *f_ind, pre_v = 0, curr_v = 0, i = 1, len_str = strlen(str);
  sscanf(str, "f %d", &pre_v);
  pre_v--;
  int first_v = pre_v;
  while ((i + 1 < len_str) && ((str[i + 1] < 48) || (str[i + 1] > 57))) {
    i++;
  }
  i++;
  while (i < (int)strlen(str)) {
    if ((str[i] == ' ') && (str[i + 1] >= 48 && str[i + 1] <= 57)) {
      sscanf(&str[i + 1], "%d", &curr_v);
      curr_v--;
      facets[j] = pre_v;
      facets[j + 1] = curr_v;
      j = j + 2;
      pre_v = curr_v;
    }
    i++;
  }
  facets[j] = pre_v;
  facets[j + 1] = first_v;
  *f_ind = j + 2;
}
int parse_obj(char *filename, unsigned int *num_of_vertexes,
              unsigned int *num_of_edges, double **vertexes, int **facets,
              coord *coord_v) {
  int error = 0;
  num_of sums;
  sums.vertexes = 0;
  sums.edges = 0;
  coord_v->Xmax = 1;
  coord_v->Xmin = -1;
  coord_v->Ymax = 1;
  coord_v->Ymin = -1;
  coord_v->Zmax = 1;
  coord_v->Zmin = -1;
  FILE *fp = fopen(filename, "r");
  if (fp != NULL) {
    if (counter_V_F(fp, &sums) >= 0) {
      *num_of_vertexes = sums.vertexes;
      *num_of_edges = sums.edges;
      fseek(fp, 0, SEEK_SET);
      *vertexes = calloc(sums.vertexes * 3, sizeof(double));
      *facets = calloc(sums.edges * 2, sizeof(int));
      if (vertexes == NULL || facets == NULL) {
        fprintf(stderr, "Памяти не хватило\n");
        error = error - 1;
      } else {
        read_data(fp, *vertexes, *facets, coord_v);
      }
      fclose(fp);
    } else {
      fprintf(stderr, "Не открылся\n");
      fclose(fp);
      error = error - 1;
    }
  }

  return error;
}
