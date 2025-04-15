#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Configurer la taille relative des widgets dans le splitter
    QList<int> sizes;
    sizes << (width() * 0.8) << (width() * 0.2); // 80% pour glWidget, 20% pour cameraHandler
    ui->splitter->setSizes(sizes);

    // Connecter le signal de quitter si nécessaire
    connect(ui->actionExit, &QAction::triggered, this, &QMainWindow::close);
}

MainWindow::~MainWindow()
{
    delete ui;
}
