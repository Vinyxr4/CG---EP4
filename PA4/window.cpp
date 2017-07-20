#include <QSlider>
#include <QKeyEvent>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QApplication>
#include <QDesktopWidget>

#include "window.h"
#include "glwidget.h"
#include "mainwindow.h"

Window::Window(MainWindow *mw, int step, int shininess)
    : mainWindow(mw), step_(step), shininess_(shininess) {
  glWidget = new GLWidget(step);

  QTabWidget *colorSlidersTab = createColorSliders();
  QTabWidget *shininessController = createShininessSliders();
  QTabWidget *BumpController = createBumpSlider();
  QTabWidget *DisplacementController = createDisplacementSlider();
  QTabWidget *ControlController = createControlSlider();
  QTabWidget *orderController = createOrderSlider();
  QPushButton *pause = createPauseButton();
  QPushButton *restart = createRestartButton();


  QVBoxLayout *mainLayout = new QVBoxLayout;
  QGridLayout *container = new QGridLayout;

  container->addWidget(glWidget, 0, 0, 11, 1);
  container->addWidget(colorSlidersTab, 1, 1, 2, 7);
  container->addWidget(shininessController, 8, 1, 2, 3);
  container->addWidget(BumpController, 4, 1, 1, 7);
  container->addWidget(DisplacementController, 5, 1, 1, 7);
  container->addWidget(ControlController, 6, 1, 1, 3);
  container->addWidget(orderController, 7, 1, 1, 3);
  container->addWidget(pause, 10, 1);
  container->addWidget(restart, 10, 2);
  container->setColumnStretch(0, 10);
  QWidget *centralWidget = new QWidget;
  centralWidget->setLayout(container);
  mainLayout->addWidget(centralWidget);

  setLayout(mainLayout);
}

QPushButton *Window::createPauseButton () {
    QPushButton *button = new QPushButton ("Pause/Resume", this);
    button->setGeometry(QRect(QPoint(100, 100), QSize(200, 50)));

    connect(button, SIGNAL (clicked(bool)), this, SLOT (handlePauseButton(bool)));

    return button;
}

QPushButton *Window::createRestartButton () {
    QPushButton *button = new QPushButton ("Restart", this);
    button->setGeometry(QRect(QPoint(100, 100), QSize(200, 50)));

    connect(button, SIGNAL (clicked(bool)), this, SLOT (handleRestartButton(bool)));

    return button;
}

void Window::handlePauseButton(bool nv) {
    if (nv == false)
         glWidget->paused = (glWidget->paused + 1) % 2;
}

void Window::handleRestartButton(bool nv) {
    if (nv == false)
         glWidget->restart = true;
}

QTabWidget* Window::createControlSlider() {
  QTabWidget *controlTab = new QTabWidget;
  QWidget *control = createControl();

  controlTab->addTab(control, "Control Points");

  return controlTab;
}

QTabWidget* Window::createOrderSlider() {
  QTabWidget *orderTab = new QTabWidget;
  QWidget *order= createOrder();

  orderTab->addTab(order, "Spline order");

  return orderTab;
}

QTabWidget* Window::createBumpSlider() {
  QTabWidget *bumpTab = new QTabWidget;
  QWidget *bumpness = createBumpness();

  bumpTab->addTab(bumpness, "Snake radius");

  return bumpTab;
}

QTabWidget* Window::createDisplacementSlider() {
  QTabWidget *displacementTab = new QTabWidget;
  QWidget *displacement = createDisplacement();

  displacementTab->addTab(displacement, "Vector size");

  return displacementTab;
}

QTabWidget* Window::createShininessSliders() {
  QTabWidget *shininessTab = new QTabWidget;
  QWidget *shininess = createShininess();

  shininessTab->addTab(shininess, "Shininess");

  return shininessTab;
}

QTabWidget* Window::createColorSliders() {
  QTabWidget *colorTab = new QTabWidget;
  QWidget *ambientColors = createAmbientColors();
  QWidget *diffusiveColors = createDiffusiveColors();
  QWidget *specularColors = createSpecularColors();

  colorTab->addTab(ambientColors, "Ambient");
  colorTab->addTab(diffusiveColors, "Diffusive");
  colorTab->addTab(specularColors, "Specular");
  return colorTab;
}


QWidget* Window::createControl() {
  QWidget *controlWidget = new QWidget;
  QGridLayout *controlSliderLayout = new QGridLayout;

  int currentRow = 0;

  controlSlider = createSlider();
  controlLabel = new QLabel(tr("Control Points:"));
  controlSliderLayout->addWidget(controlLabel, currentRow, 0);
  controlSliderLayout->addWidget(controlSlider, currentRow, 1);
  controlSlider->setValue(0);
  glWidget->numPoints = 10;
  connect(controlSlider, SIGNAL(valueChanged(int)), this, SLOT(valueCTRL(int)));
  currentRow++;

  controlWidget->setLayout(controlSliderLayout);
  return controlWidget;
}
QWidget* Window::createOrder() {
  QWidget *orderWidget = new QWidget;
  QGridLayout *orderSliderLayout = new QGridLayout;

  int currentRow = 0;

  orderSlider = createSlider();
  orderLabel = new QLabel(tr("Spline order:"));
  orderSliderLayout->addWidget(orderLabel, currentRow, 0);
  orderSliderLayout->addWidget(orderSlider, currentRow, 1);
  orderSlider->setValue(0);
  glWidget->snakeAttr.curveOrder = 1;
  connect(orderSlider, SIGNAL(valueChanged(int)), this, SLOT(valueORD(int)));
  currentRow++;

  orderWidget->setLayout(orderSliderLayout);
  return orderWidget;
}

QWidget* Window::createBumpness() {
  QWidget *bumpWidget = new QWidget;
  QGridLayout *bumpSliderLayout = new QGridLayout;

  int currentRow = 0;

  bumpSlider = createSlider();
  bumpLabel = new QLabel(tr("Snake radius:"));
  bumpSliderLayout->addWidget(bumpLabel, currentRow, 0);
  bumpSliderLayout->addWidget(bumpSlider, currentRow, 1);
  bumpSlider->setValue(step_/2);
  glWidget->snakeAttr.radius = 0.25;
  connect(bumpSlider, SIGNAL(valueChanged(int)), this, SLOT(valueBMP(int)));
  currentRow++;

  bumpWidget->setLayout(bumpSliderLayout);
  return bumpWidget;
}

QWidget* Window::createDisplacement() {
  QWidget *displacementWidget = new QWidget;
  QGridLayout *displacementSliderLayout = new QGridLayout;

  int currentRow = 0;

  displacementSlider = createSlider();
  displacementLabel = new QLabel(tr("Vector size:"));
  displacementSliderLayout->addWidget(displacementLabel, currentRow, 0);
  displacementSliderLayout->addWidget(displacementSlider, currentRow, 1);
  displacementSlider->setValue(step_/2);
  glWidget->snakeAttr.spacement = 0.5;
  connect(displacementSlider, SIGNAL(valueChanged(int)), this, SLOT(valueDSP(int)));
  currentRow++;

  displacementWidget->setLayout(displacementSliderLayout);
  return displacementWidget;
}

QWidget* Window::createShininess() {
  QWidget *shininessWidget = new QWidget;
  QGridLayout *shininessSliderLayout = new QGridLayout;

  int currentRow = 0;

  shininessSlider = createShininessSlider();
  shininessLabel = new QLabel(tr("Shininess:"));
  shininessSliderLayout->addWidget(shininessLabel, currentRow, 0);
  shininessSliderLayout->addWidget(shininessSlider, currentRow, 1);
  shininessSlider->setValue(shininess_/2);
  glWidget->shininess = 32;
  connect(shininessSlider, SIGNAL(valueChanged(int)), this, SLOT(valueSh(int)));
  currentRow++;

  shininessWidget->setLayout(shininessSliderLayout);
  return shininessWidget;
}

QWidget* Window::createAmbientColors() {
  QWidget *colorWidget = new QWidget;
  QGridLayout *colorSliderLayout = new QGridLayout;

  int currentRow = 0;

  redAmbientSlider = createSlider();
  redAmbientLabel = new QLabel(tr("Red:"));
  colorSliderLayout->addWidget(redAmbientLabel, currentRow, 0);
  colorSliderLayout->addWidget(redAmbientSlider, currentRow, 1);
  redAmbientSlider->setValue(step_/2);
  glWidget->ambientColor.setX(0.5);
  connect(redAmbientSlider, SIGNAL(valueChanged(int)), this, SLOT(valueRA(int)));
  currentRow++;

  greenAmbientSlider = createSlider();
  greenAmbientLabel = new QLabel(tr("Green:"));
  colorSliderLayout->addWidget(greenAmbientLabel, currentRow, 0);
  colorSliderLayout->addWidget(greenAmbientSlider, currentRow, 1);
  greenAmbientSlider->setValue(step_/2);
  glWidget->ambientColor.setY(0.5);
  connect(greenAmbientSlider, SIGNAL(valueChanged(int)), this, SLOT(valueGA(int)));
  currentRow++;

  blueAmbientSlider = createSlider();
  blueAmbientLabel = new QLabel(tr("Blue:"));
  colorSliderLayout->addWidget(blueAmbientLabel, currentRow, 0);
  colorSliderLayout->addWidget(blueAmbientSlider, currentRow, 1);
  blueAmbientSlider->setValue(step_/2);
  glWidget->ambientColor.setZ(0.5);
  connect(blueAmbientSlider, SIGNAL(valueChanged(int)), this, SLOT(valueBA(int)));
  currentRow++;

  colorWidget->setLayout(colorSliderLayout);
  return colorWidget;
}

QWidget* Window::createDiffusiveColors() {
  QWidget *colorWidget = new QWidget;
  QGridLayout *colorSliderLayout = new QGridLayout;

  int currentRow = 0;

  redDiffusiveSlider = createSlider();
  redDiffusiveLabel = new QLabel(tr("Red:"));
  colorSliderLayout->addWidget(redDiffusiveLabel, currentRow, 0);
  colorSliderLayout->addWidget(redDiffusiveSlider, currentRow, 1);
  redDiffusiveSlider->setValue(step_/2);
  glWidget->difusiveColor.setX(0.5);
  connect(redDiffusiveSlider, SIGNAL(valueChanged(int)), this, SLOT(valueRD(int)));
  currentRow++;

  greenDiffusiveSlider = createSlider();
  greenDiffusiveLabel = new QLabel(tr("Green:"));
  colorSliderLayout->addWidget(greenDiffusiveLabel, currentRow, 0);
  colorSliderLayout->addWidget(greenDiffusiveSlider, currentRow, 1);
  greenDiffusiveSlider->setValue(step_/2);
  glWidget->difusiveColor.setY(0.5);
  connect(greenDiffusiveSlider, SIGNAL(valueChanged(int)), this, SLOT(valueGD(int)));
  currentRow++;

  blueDiffusiveSlider = createSlider();
  blueDiffusiveLabel = new QLabel(tr("Blue:"));
  colorSliderLayout->addWidget(blueDiffusiveLabel, currentRow, 0);
  colorSliderLayout->addWidget(blueDiffusiveSlider, currentRow, 1);
  blueDiffusiveSlider->setValue(step_/2);
  glWidget->difusiveColor.setZ(0.5);
  connect(blueDiffusiveSlider, SIGNAL(valueChanged(int)), this, SLOT(valueBD(int)));
  currentRow++;

  colorWidget->setLayout(colorSliderLayout);
  return colorWidget;
}

QWidget* Window::createSpecularColors() {
  QWidget *colorWidget = new QWidget;
  QGridLayout *colorSliderLayout = new QGridLayout;

  int currentRow = 0;

  redSpecularSlider = createSlider();
  redSpecularLabel = new QLabel(tr("Red:"));
  colorSliderLayout->addWidget(redSpecularLabel, currentRow, 0);
  colorSliderLayout->addWidget(redSpecularSlider, currentRow, 1);
  redSpecularSlider->setValue(step_/2);
  glWidget->specularColor.setX(0.5);
  connect(redSpecularSlider, SIGNAL(valueChanged(int)), this, SLOT(valueRS(int)));
  currentRow++;

  greenSpecularSlider = createSlider();
  greenSpecularLabel = new QLabel(tr("Green:"));
  colorSliderLayout->addWidget(greenSpecularLabel, currentRow, 0);
  colorSliderLayout->addWidget(greenSpecularSlider, currentRow, 1);
  greenSpecularSlider->setValue(step_/2);
  glWidget->specularColor.setY(0.5);
  connect(greenSpecularSlider, SIGNAL(valueChanged(int)), this, SLOT(valueGS(int)));
  currentRow++;

  blueSpecularSlider = createSlider();
  blueSpecularLabel = new QLabel(tr("Blue:"));
  colorSliderLayout->addWidget(blueSpecularLabel, currentRow, 0);
  colorSliderLayout->addWidget(blueSpecularSlider, currentRow, 1);
  blueSpecularSlider->setValue(step_/2);
  glWidget->specularColor.setZ(0.5);
  connect(blueSpecularSlider, SIGNAL(valueChanged(int)), this, SLOT(valueBS(int)));
  currentRow++;

  colorWidget->setLayout(colorSliderLayout);
  return colorWidget;
}

void Window::disconnectWidgetUpdate() {
  glWidget->disconnectUpdate();
}

void Window::connectWidgetUpdate() {
  glWidget->connectUpdate();
}

QSlider *Window::createSlider() {
  QSlider *slider = new QSlider(Qt::Horizontal);
  slider->setRange(0, step_);
  slider->setSingleStep(10);
  slider->setTickPosition(QSlider::TicksRight);
  return slider;
}

QSlider *Window::createShininessSlider() {
  QSlider *slider = new QSlider(Qt::Horizontal);
  slider->setRange(0, shininess_);
  slider->setSingleStep(8);
  slider->setTickPosition(QSlider::TicksRight);
  return slider;
}

void Window::valueBMP(int nv) {
    glWidget->snakeAttr.radius = ((float) nv)/(2*step_);
}

void Window::valueCTRL(int nv) {
    glWidget->numPoints = (int) (10 + 990 * (float) (nv)/step_);
    glWidget->restart = true;
}

void Window::valueORD(int nv) {
    glWidget->snakeAttr.curveOrder = (int) (1 + 9 * (float) (nv)/step_);
    glWidget->restart = true;
}

void Window::valueDSP(int nv) {
    glWidget->snakeAttr.spacement = ((float) nv)/step_;
}

// Ambient light slider controller
void Window::valueRA(int nv) {
    glWidget->ambientColor.setX(float(nv)/step_);
}
void Window::valueGA(int nv) {
    glWidget->ambientColor.setY(float(nv)/step_);
}
void Window::valueBA(int nv) {
    glWidget->ambientColor.setZ(float(nv)/step_);
}

// Difusive light slider controller
void Window::valueRD(int nv) {
    glWidget->difusiveColor.setX(float(nv)/step_);
}
void Window::valueGD(int nv) {
    glWidget->difusiveColor.setY(float(nv)/step_);
}
void Window::valueBD(int nv) {
    glWidget->difusiveColor.setZ(float(nv)/step_);
}

// Specular light slider controller
void Window::valueRS(int nv) {
    glWidget->specularColor.setX(float(nv)/step_);
}
void Window::valueGS(int nv) {
    glWidget->specularColor.setY(float(nv)/step_);
}
void Window::valueBS(int nv) {
    glWidget->specularColor.setZ(float(nv)/step_);
}

// Shininess - Albedo - Roughness slider controller
void Window::valueSh(int nv) {
    glWidget->shininess = float(nv);
}
