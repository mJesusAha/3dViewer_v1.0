#include "Opengl.h"
// #include <QMessageBox>
Opengl::Opengl(QWidget *parent)
    : QOpenGLWidget(parent)

{}

void Opengl::initializeGL() {
  glEnable(GL_DEPTH_TEST);  // буфер глyбины
  glEnable(GL_CULL_FACE);
}

void Opengl::resizeGL(int w, int h) {
  float aspect;
  if (h == 0) {
    aspect = 0;
  } else {
    aspect = w / (float)h;
  }
  glViewport(0, aspect, w, h);
}

void Opengl::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(back_color.redF(), back_color.greenF(), back_color.blueF(),
               back_color.alphaF());
  glMatrixMode(GL_PROJECTION);  // настраиваем отображение области
  glLoadIdentity();
  projections();
  glMatrixMode(GL_MODELVIEW);  // настраиваем отображение объекта
  glLoadIdentity();
  drawFigure();  //
}
void Opengl::projections()  // 0параллельная 1центральная
{
  if (typeProjection == 0) {
    glOrtho(coord.Xmin * 1.5, coord.Xmax * 1.5, coord.Ymin * 1.5,
            coord.Ymax * 1.5, coord.Zmin * 10, coord.Zmax * 10);
  } else if (typeProjection == 1) {
    float heapHeight = coord.Ymax / (2 * tan((60.0 * M_PI / 180) / 2));
    glFrustum(coord.Xmin, coord.Xmax, coord.Ymin, coord.Ymax, heapHeight,
              coord.Zmax * 100);
  }
}
void Opengl::drawFigure() {
  GLfloat *vertexArray =
      (GLfloat *)calloc(num_of_vertexes * 3, sizeof(GLfloat));

  GLint *indexArray = (GLint *)calloc(num_of_edges * 2, sizeof(GLint));
  if (vertexArray == NULL || indexArray == NULL) {
    printf("нет памяти");
    QMessageBox::about(this, "Проблема", "В системе мало места!");
  } else {
    for (unsigned int i = 0; i < num_of_vertexes * 3; i++) {
      vertexArray[i] = (GLfloat)vertexes[i];
    }
    for (unsigned int i = 0; i < num_of_edges * 2; i++) {
      indexArray[i] = (GLint)edges[i];
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertexArray);

    displayMethodSettings();

    lineSettings();
    glDrawElements(GL_LINES, num_of_edges * 2, GL_UNSIGNED_INT,
                   indexArray);  // отрисовка линий

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_LINE_STIPPLE);
    glDisable(GL_POINT_SMOOTH);
  }
  free(vertexArray);

  free(indexArray);

  vertexArray = nullptr;

  indexArray = nullptr;
  update();
}
///////////////точки///////////////
void Opengl::displayMethodSettings() {
  if (displayMethod == 0) {  // режим отображения точек
    glEnable(GL_POINT_SMOOTH);
  }

  if (displayMethod != 2) {  // если метод отображения точек не none
    glColor3f(vertex_color.redF(), vertex_color.greenF(),
              vertex_color.blueF());  // цвет
    glPointSize(pointSize);
    glDrawArrays(GL_POINTS, 0, num_of_vertexes);  // отрисовка точек
  }
}
//////////////////линии/////////////
void Opengl::lineSettings() {
  if (type_line == 1) {  // 1 пунктир
    glLineStipple(10, 0x3333);
    glEnable(GL_LINE_STIPPLE);
  }

  glColor3f(edge_color.redF(), edge_color.greenF(),
            edge_color.blueF());  // цвет
  glLineWidth(lineWidth);         // ширина
}
