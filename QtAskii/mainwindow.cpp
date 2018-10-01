#include "mainwindow.h"
#include "ui_mainwindow.h"

extern "C" {
    void libInit();
    void libQuit();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    optionSetter::gui_setDefaultOpts();
    libInit();
}

MainWindow::~MainWindow()
{
    libQuit();
    delete ui;
}
