#include "mainwindow.h"
#include "window.h"

#include <QMenu>
#include <QDebug>
#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>
#include <QVector3D>
#include <vector>
#include <cmath>

MainWindow::MainWindow() {
  QMenuBar *menuBar = new QMenuBar;

  setMenuBar(menuBar);

  window_ = new Window(this, 100, 64);
  setCentralWidget(window_);
  setWindowTitle("PA2");
}
