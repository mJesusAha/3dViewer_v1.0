#include "mainwindow.h"

#include <unistd.h>

#include <QBuffer>
#include <QColorDialog>
#include <QDateTime>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QImage>
#include <QMouseEvent>
#include <QMovie>
#include <QOpenGLWidget>
#include <QPixmap>
#include <QThread>
#include <QTimer>
#include <QWheelEvent>
#include <thread>

#include "QtTest/QTest"
#include "qgifimage.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  connect(ui->radioButton_dashed, &QRadioButton::toggled, this,
          &MainWindow::onRadioButtonDashed);
  connect(ui->radioButton_solid, &QRadioButton::toggled, this,
          &MainWindow::onRadioButtonSolid);
  connect(ui->radioButton_central, &QRadioButton::toggled, this,
          &MainWindow::onRadioButtonCentral);
  connect(ui->radioButton_parallel, &QRadioButton::toggled, this,
          &MainWindow::onRadioButtonParallel);
  read_config();
}

void MainWindow::read_config() {
  QString path = qApp->applicationDirPath() + "/setV1.conf";
  QByteArray ba = path.toLatin1();
  const char *filepath = ba.data();
  FILE *fp = fopen(filepath, "r");
  if (fp != NULL) {
    char *edges_color = (char *)calloc(10, sizeof(char));
    char *vertexes_color = (char *)calloc(10, sizeof(char));
    char *bg_color = (char *)calloc(10, sizeof(char));
    if (edges_color == NULL || vertexes_color == NULL || bg_color == NULL) {
      printf("нет памяти");
      QMessageBox::about(this, "Проблема", "В системе мало места!");
    } else {
      int num_of_args_read = fscanf(
          fp, "%d %d %s %f %s %f %d %s", &(ui->openGLWidget->typeProjection),
          &(ui->openGLWidget->type_line), edges_color,
          &(ui->openGLWidget->lineWidth), vertexes_color,
          &(ui->openGLWidget->pointSize), &(ui->openGLWidget->displayMethod),
          bg_color);
      if (num_of_args_read == 8) {
        set_ui_values(bg_color, edges_color, vertexes_color);  /////// 1
      } else {
        fprintf(stderr, "%s\n", "Oшибка в файле конфигурации");
      }
    }
    fclose(fp);
    free(edges_color);
    free(vertexes_color);
    free(bg_color);
    edges_color = nullptr;
    vertexes_color = nullptr;
    bg_color = nullptr;
  } else {
    fclose(fp);
  }
}

void MainWindow::set_ui_values(char *bg_color, char *edges_color,
                               char *vertexes_color) {  /// 1
  QColor colorB = QColor(bg_color);
  QColor colorE = QColor(edges_color);
  QColor colorV = QColor(vertexes_color);
  ui->openGLWidget->back_color = colorB;
  ui->openGLWidget->edge_color = colorE;
  ui->openGLWidget->vertex_color = colorV;

  if (ui->openGLWidget->typeProjection == 0) {
    ui->radioButton_parallel->setChecked(true);
    ui->radioButton_central->setChecked(false);
    Z = ui->doubleSpinBox_OZ->value();
  } else {
    ui->radioButton_parallel->setChecked(false);
    ui->radioButton_central->setChecked(true);
    central_factor = coordinates.Zmax * 10;
    Z = ui->doubleSpinBox_OZ->value() + 14;
  }
  if (ui->openGLWidget->type_line == 0) {
    ui->radioButton_solid->setChecked(true);
    ui->radioButton_dashed->setChecked(false);
  } else {
    ui->radioButton_solid->setChecked(false);
    ui->radioButton_dashed->setChecked(true);
  }
  ui->doubleSpinBox_thicknessEdges->setValue(ui->openGLWidget->lineWidth);
  ui->doubleSpinBox_sizeVertices->setValue(ui->openGLWidget->pointSize);
  ui->comboBox_displayMethod->setCurrentIndex(ui->openGLWidget->displayMethod);
}

MainWindow::~MainWindow() {
  write_config();
  if (vertexes != NULL) {
    free(vertexes);
    vertexes = nullptr;
  }
  if (edges != NULL) {
    free(edges);
    edges = nullptr;
  }
  delete ui;
}
void MainWindow::write_config() {
  QString path = qApp->applicationDirPath() + "/setV1.conf";
  QByteArray ba = path.toLatin1();
  const char *filepath = ba.data();
  QByteArray ba1 = ui->openGLWidget->back_color.name().toLatin1();
  const char *bg = ba1.data();
  QByteArray ba2 = ui->openGLWidget->vertex_color.name().toLatin1();
  const char *v_color = ba2.data();
  QByteArray ba3 = ui->openGLWidget->edge_color.name().toLatin1();
  const char *e_color = ba3.data();
  FILE *fp = fopen(filepath, "w");
  fprintf(fp, "%d %d %s %f %s %f %d %s\n", 0, ui->openGLWidget->type_line,
          e_color, ui->openGLWidget->lineWidth, v_color,
          ui->openGLWidget->pointSize, ui->openGLWidget->displayMethod, bg);
  ui->openGLWidget->displayMethod = 0;
  fclose(fp);
}
void MainWindow::on_pushButton_OpenFile_clicked() {
  QString str = QFileDialog::getOpenFileName();
  QByteArray t = str.toLocal8Bit();
  char *str_char = t.data();
  int n = strlen(str_char);
  if (str_char[n - 1] != 'j' && n > 2) {
    QMessageBox::about(this, "Так так", "Ошибка,\n разрешение не то");

  } else {
    file_path = t.data();
    free(vertexes);
    free(edges);
    vertexes = nullptr;
    edges = nullptr;
    ////////////////////////////////////////////////////////////////////////
    int errors =
        parse_obj(file_path, &num_of_vertexes, &num_of_edges, &vertexes, &edges,
                  &coordinates);  // запускаем парсер
    ///////////////////////////////////////////////////////////////////////
    if (errors < 0) {
      free(vertexes);
      free(edges);
      vertexes = nullptr;
      edges = nullptr;
      QMessageBox::about(this, "Так так",
                         "Ошибка, не то, перезагрузи приложение))");
      MainWindow();

    } else {
      prev_scale = 1;
      prev_x_angle = 0;
      prev_y_angle = 0;
      prev_z_angle = 0;

      ui->openGLWidget->coord = coordinates;
      ui->label_V->setText("Вершин: " + QString::number(num_of_vertexes));
      ui->label_G->setText("Граней: " +
                           QString::number((num_of_edges / 6) - 2));
      ui->label->setText("Файл: " + str);
      ui->openGLWidget->num_of_vertexes =
          num_of_vertexes;  // передаём в класс OpenGL все значения
      ui->openGLWidget->num_of_edges = num_of_edges;
      ui->openGLWidget->vertexes = vertexes;
      ui->openGLWidget->edges = edges;
    }
  }
}
///////////////////////////////////Работа с цветом//////////////////////////////

//////////////////Фон///////////////
void MainWindow::pushBackGroundColor() {
  printf("%f  %f   %f\n", (float)r_BackGround / 100, (float)g_BackGround / 100,
         (float)b_BackGround / 100);
  ui->openGLWidget->back_color =
      QColor::fromRgbF((float)r_BackGround / 100, (float)g_BackGround / 100,
                       (float)b_BackGround / 100, 1);
}
void MainWindow::on_verticalSlider_3_valueChanged(int action) {
  r_BackGround = action;
  pushBackGroundColor();
}
void MainWindow::on_verticalSlider_2_valueChanged(int action) {
  g_BackGround = action;
  pushBackGroundColor();
}
void MainWindow::on_verticalSlider_valueChanged(int action) {
  b_BackGround = action;
  pushBackGroundColor();
}
////////////////Линии//////////////////////
void MainWindow::pushVertexes_color() {
  ui->openGLWidget->vertex_color = QColor::fromRgbF(
      (float)r_Vert / 100, (float)g_Vert / 100, (float)b_Vert / 100, 1);
}

void MainWindow::on_verticalSlider_LineR_valueChanged(int value) {
  r_Vert = value;
  pushVertexes_color();
}

void MainWindow::on_verticalSlider_LineG_valueChanged(int value) {
  g_Vert = value;
  pushVertexes_color();
}

void MainWindow::on_verticalSlider_LineB_valueChanged(int value) {
  b_Vert = value;
  pushVertexes_color();
}
//////////////Точки/////////////////////////
void MainWindow::pushEdges_color() {
  ui->openGLWidget->edge_color = QColor::fromRgbF(
      (float)r_Dot / 100, (float)g_Dot / 100, (float)b_Dot / 100, 1);
  // update();
}

void MainWindow::on_verticalSlider_DotR_valueChanged(int value) {
  r_Dot = value;
  pushEdges_color();
}

void MainWindow::on_verticalSlider_DotG_valueChanged(int value) {
  g_Dot = value;
  pushEdges_color();
}

void MainWindow::on_verticalSlider_DotB_valueChanged(int value) {
  b_Dot = value;
  pushEdges_color();
}

/////////////////////Ширина линий/////////////////////////////

void MainWindow::on_doubleSpinBox_thicknessEdges_valueChanged(double arg1) {
  if (arg1 > 12) {
    arg1 = 12;
  }
  if (arg1 < 0) {
    arg1 = 0;
  }
  ui->openGLWidget->lineWidth = arg1;
}
///////////////////////////////Вид линии///////////////////
void MainWindow::onRadioButtonDashed(bool checked) {  // пунктир
  if (checked) {
    ui->openGLWidget->type_line = 1;
  }
}
void MainWindow::onRadioButtonSolid(bool checked) {  //  сплошная
  if (checked) {
    ui->openGLWidget->type_line = 0;
  }
}
//////////////////////////Проекция////////////////////////////////////////////////////

void MainWindow::onRadioButtonCentral(  // 1
    bool checked) {  // нажатие радиокнопки метода отображения Central
  if (checked) {
    ui->openGLWidget->typeProjection = 1;
    central_factor = coordinates.Zmax +
                     10;  // долно быть смещение по оси Z, чтобы полностью вошло
    printf("<%d>", central_factor);
    ui->doubleSpinBox_OX->setValue(0);
    ui->doubleSpinBox_OY->setValue(0);
    ui->doubleSpinBox_OZ->setValue(8.5);
    ui->doubleSpinBox_ROX->setValue(0);
    ui->doubleSpinBox_ROY->setValue(0);
    ui->doubleSpinBox_ROZ->setValue(0);
    translateX(&prev_x_move, ui->doubleSpinBox_OX->value(), &prev_y_move,
               ui->doubleSpinBox_OY->value(), &prev_z_move,
               ui->doubleSpinBox_OZ->value() - central_factor, vertexes,
               num_of_vertexes * 3);
  }
}
void MainWindow::onRadioButtonParallel(  // 0
    bool checked) {  // нажатие радиокнопки метода отображения Parallel
  if (checked) {
    ui->openGLWidget->typeProjection = 0;
    central_factor = 0;
    translateX(&prev_x_move, 0, &prev_y_move, 0, &prev_z_move, 0, vertexes,
               num_of_vertexes * 3);
    ui->doubleSpinBox_OX->setValue(0);
    ui->doubleSpinBox_OY->setValue(0);
    ui->doubleSpinBox_OZ->setValue(0);
    ui->doubleSpinBox_ROX->setValue(0);
    ui->doubleSpinBox_ROY->setValue(0);
    ui->doubleSpinBox_ROZ->setValue(0);
  }
}
//////////////////////////Размер точек//////////////////////////////////////
void MainWindow::on_doubleSpinBox_sizeVertices_valueChanged(
    double arg1)  // изменение размера точек узлов
{
  ui->openGLWidget->pointSize = arg1;
}
//////////////////Вид точек(0 круг, 1 квадрат, 2 нет////////////////////////////
void MainWindow::on_comboBox_displayMethod_currentIndexChanged(int index) {
  ui->openGLWidget->displayMethod = index;
}

///////////////////Установки/////////////////////////
void MainWindow::on_pushButton_rebuild_clicked()  //
{
  X = ui->doubleSpinBox_OX->value();  // сдвиг по осям
  Y = ui->doubleSpinBox_OY->value();
  Z = ui->doubleSpinBox_OZ->value() - central_factor;
  RX = ui->doubleSpinBox_ROX->value();  // вращение по осям
  RY = ui->doubleSpinBox_ROY->value();
  if (ui->openGLWidget->typeProjection == 0) {
    Scale = ui->doubleSpinBox_OZ->value() + 1;
  }
  RZ = ui->doubleSpinBox_ROZ->value();
  if (prev_scale < Scale) {
    scaleX(&prev_scale, &Scale, vertexes, num_of_vertexes * 3);
  }
  translateX(&prev_x_move, X, &prev_y_move, Y, &prev_z_move, Z, vertexes,
             num_of_vertexes * 3);
  rotateX(&prev_x_angle, RX, &prev_y_angle, RY, &prev_z_angle, RZ, vertexes,
          num_of_vertexes * 3);
  if (prev_scale > Scale) {
    scaleX(&prev_scale, &Scale, vertexes, num_of_vertexes * 3);
  }
  update();
}
////////////////////////Ратбота мыши/////////////////////////
void MainWindow::mousePressEvent(QMouseEvent *e) {
  if (e->button() == Qt::LeftButton) {
    if (RX >= 180 && RX <= -180) {
      RX = 0;
    }
    if (RY >= 180 && RX <= -180) {
      RY = 0;
    }
    preX = e->pos().x();  // предыдущие координаты мыши
    preY = e->pos().y();
  }
}

void MainWindow::mouseMoveEvent(
    QMouseEvent *e) {  // отслеживание поворотов мышкой по виджету

  if (e->pos().x() >= 22 && e->pos().x() <= 711 && e->pos().y() >= 22 &&
      e->pos().x() <= 684) {  // для координат виджета openGL
    RX = RX + (e->pos().y() - preY) / 4;
    if (RX >= 180 && RX <= -180) {
      RX = 0;
    }
    ui->doubleSpinBox_ROX->setValue(RX);
    RY = RY + (e->pos().x() - preX) / 4;
    if (RY >= 180 && RX <= -180) {
      RY = 0;
    }
    ui->doubleSpinBox_ROY->setValue(RY);
  }
  preX = e->pos().x();  // предыдущие координаты мыши
  preY = e->pos().y();
  rotateX(&prev_x_angle, RX, &prev_y_angle, RY, &prev_z_angle, RZ, vertexes,
          num_of_vertexes * 3);
  QMainWindow::mouseMoveEvent(e);
}

void MainWindow::wheelEvent(QWheelEvent *event) {
  double scrole_scale = 0;
  if (event->position().x() >= 22 && event->position().x() <= 711 &&
      event->position().y() >= 22 &&
      event->position().x() <= 684) {  // для координат виджета openGL
    if (event->angleDelta().y() > 0 &&
        ui->doubleSpinBox_OZ->value() < 20) {  // если увеличиваем
      scrole_scale = (ui->doubleSpinBox_OZ->value()) + 0.1;
      scaleX(&prev_scale, &scrole_scale, vertexes, num_of_vertexes * 3);
      ui->doubleSpinBox_OZ->setValue(scrole_scale);
    } else if (event->angleDelta().y() < 0 &&
               ui->doubleSpinBox_OZ->value() > 0) {  // если уменьшаем
      scrole_scale = (ui->doubleSpinBox_OZ->value()) - 0.1;
      scaleX(&prev_scale, &scrole_scale, vertexes, num_of_vertexes * 3);
      ui->doubleSpinBox_OZ->setValue(scrole_scale);
    }
  }
  //  update();
}

////////////////////////Cкриншот////////////////////////////
void MainWindow::on_pushButton_clicked() { saveToFile(); }
void MainWindow::saveToFile() {
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save"), "",
                                                  tr("(*.bmp);;(*.jpeg)"));
  ui->openGLWidget->grab().save(fileName);  // сохранение скрина
}
////////////////////////gif////////////////////////////////
void MainWindow::on_pushButton_screencast_clicked() { create_gif(); }

void MainWindow::create_gif() {
  QGifImage gif(QSize(640, 480));
  gif.setDefaultDelay(100);
  QPixmap screenshot;
  QImage image;
  int xOffset = 0;
  int yOffset = 0;
  double sscale = 1, x_angle = 0, y_angle = 0, z_angle = 0, x_move = 0,
         y_move = 0, z_move = 0;
  QString save_file_path = QFileDialog::getSaveFileName(
      this, "Open file", "../../new.gif", "GIF Files (*.gif);");
  if (prev_scale < Scale) {
    scaleX(&prev_scale, &sscale, vertexes, num_of_vertexes * 3);
  }
  translateX(&prev_x_move, 0, &prev_y_move, 0, &prev_z_move, 0, vertexes,
             num_of_vertexes * 3);
  rotateX(&prev_x_angle, 0, &prev_y_angle, 0, &prev_z_angle, 0, vertexes,
          num_of_vertexes * 3);
  if (prev_scale > Scale) {
    scaleX(&prev_scale, &sscale, vertexes, num_of_vertexes * 3);
  }
  for (int i = 0; i < 50; i += 1) {
    screenshot = ui->openGLWidget->grab();  // скриншот
    screenshot =
        screenshot.scaled(screenshot.width() / 2, screenshot.height() / 2,
                          Qt::KeepAspectRatio, Qt::SmoothTransformation);
    xOffset = (screenshot.width() - 640) /
              2;  // расчёт точки начала для обрезки скриншота по центру
    yOffset = (screenshot.height() - 480) / 2;
    screenshot = screenshot.copy(xOffset, yOffset, 640, 480);  // обрезка
    image = screenshot.toImage();
    gif.addFrame(image, QPoint(0, 0));  // добавление скриншота в gif
    translateX(&prev_x_move, x_move + ui->doubleSpinBox_OX->value() / 50,
               &prev_y_move, y_move + ui->doubleSpinBox_OY->value() / 50,
               &prev_z_move,
               z_move + (ui->doubleSpinBox_OZ->value() - central_factor) / 50,
               vertexes, num_of_vertexes * 3);
    rotateX(&prev_x_angle, x_angle + ui->doubleSpinBox_ROX->value() / 50,
            &prev_y_angle, y_angle + ui->doubleSpinBox_ROY->value() / 50,
            &prev_z_angle, z_angle + ui->doubleSpinBox_ROZ->value() / 50,
            vertexes, num_of_vertexes * 3);
    x_move += ui->doubleSpinBox_OX->value() / 50;
    y_move += ui->doubleSpinBox_OY->value() / 50;
    z_move += ui->doubleSpinBox_OZ->value() / 50;
    x_angle += ui->doubleSpinBox_ROX->value() / 50;
    y_angle += ui->doubleSpinBox_ROY->value() / 50;
    z_angle += ui->doubleSpinBox_ROZ->value() / 50;
    if (ui->doubleSpinBox_OZ->value() > 1) {
      sscale += (ui->doubleSpinBox_OZ->value() - 1) / 50;
      scaleX(&prev_scale, &sscale, vertexes, num_of_vertexes * 3);

    } else if (ui->doubleSpinBox_OZ->value() < 1) {
      sscale += -(ui->doubleSpinBox_OZ->value()) / 50;
      scaleX(&prev_scale, &sscale, vertexes, num_of_vertexes * 3);
    }
  }

  gif.save(save_file_path);
}
