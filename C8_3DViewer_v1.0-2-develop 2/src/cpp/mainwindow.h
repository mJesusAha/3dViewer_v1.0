#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
// #include <QOpenGLWidget>
#include <QWidget>

#include "opengl.h"

// #include "opengl.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  QString filePath = "";
  //  num_of sums;
  unsigned int num_of_vertexes = 0;
  unsigned int num_of_edges = 0;
  double *vertexes = 0;
  int *edges = 0;
  char *file_path = {0};
  double prev_scale = 1;
  double prev_x_move = 0;
  double prev_y_move = 0;
  double prev_z_move = 0;
  double prev_x_angle = 0;
  double prev_z_angle = 0;
  double prev_y_angle = 0;

 private:
  int r_BackGround, g_BackGround, b_BackGround;
  int r_Vert, g_Vert, b_Vert;
  int r_Dot, g_Dot, b_Dot;

 public slots:
  void create_gif();

 private slots:
  /////////////////Настройки///////////////////////////////////////////////
  void set_ui_values(char *bg_color, char *edges_color, char *vertexes_color);

  ///////////////Конфигурации/////////////////////////////////////////////
  void read_config();
  void write_config();

  ////////////////////Открытие////////////////////////////////////////////
  void on_pushButton_OpenFile_clicked();

  ///////////////////Тип точек/////////////////////////////////////////////
  void onRadioButtonDashed(bool checked);
  void onRadioButtonSolid(bool checked);
  void on_comboBox_displayMethod_currentIndexChanged(int index);

  //////////////////Проекция////////////////////////////////////////////
  void onRadioButtonCentral(bool checked);
  void onRadioButtonParallel(bool checked);

  /////////////////Толщина//////////////////////////////////////////////
  void on_doubleSpinBox_thicknessEdges_valueChanged(double arg1);
  void on_doubleSpinBox_sizeVertices_valueChanged(double arg1);

  ////////////////////Скрин////////////////////////////////////////////////
  void on_pushButton_rebuild_clicked();
  void on_pushButton_screencast_clicked();
  //////////////////Сохранить изображение//////////////////////////////////
  void saveToFile();
  void on_pushButton_clicked();
  ///////////////////////Изменение цвета фона/////////////////////////////
  void pushBackGroundColor();

  void on_verticalSlider_3_valueChanged(int value);
  void on_verticalSlider_2_valueChanged(int value);
  void on_verticalSlider_valueChanged(int value);
  ///////////////////////////Изменение цвета линий///////////////////////
  void pushVertexes_color();

  void on_verticalSlider_LineR_valueChanged(int value);
  void on_verticalSlider_LineG_valueChanged(int value);
  void on_verticalSlider_LineB_valueChanged(int value);
  ///////////////////////////Изменение цвета вершин//////////////////////
  void pushEdges_color();

  void on_verticalSlider_DotR_valueChanged(int value);
  void on_verticalSlider_DotG_valueChanged(int value);
  void on_verticalSlider_DotB_valueChanged(int value);
  ///////////////////////////////////////////////////////////////////////

 private:
  Ui::MainWindow *ui;
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mousePressEvent(QMouseEvent *e);
  virtual void wheelEvent(QWheelEvent *event);
  double X = 0, Y = 0, Z = 0, RX = 0, RY = 0, RZ = 0, Scale = 1, preX = 0,
         preY = 0;
  coord coordinates = {-1, 1, -1, 1, -1, 1};
  void start_second_Thread();
  int central_factor = 0;
};
#endif  // MAINWINDOW_H
