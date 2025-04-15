#include <QApplication>
#include <ctime>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    // Initializing the random number generator
    srand(time(nullptr));

    // Creating the QT application
    QApplication app(argc, argv);

    // Creating the main window
    MainWindow mainWindow;
    mainWindow.show();

    // Executing the QT application
    return app.exec();
}
