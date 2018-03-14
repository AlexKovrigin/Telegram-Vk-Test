#include "controller.h"
/*
 * Controller, the whole project is using MVC pattern
 * This controller just connects view and model layers
 * Project made by Alex Kovrigin
 * http://www.alex.unaux.com
 * Contact me:
 * Telegram: @alex_kovrigin
 * Email: a.kovrigin0@gmail.com
*/
//Constructor connecting features beetween layers
Controller::Controller()
{
    MainWindow * b = new MainWindow(); //View layer
    Model * a = new Model(); // Model layer
    b->show();

    //Signals between a and b
    connect(a, SIGNAL(updateTable(QVector<QString>)), b, SLOT(updateTable(QVector<QString>))); // Add info to table
    connect(b, SIGNAL(updateModel()), a, SLOT(update())); // Update button
    connect(a, SIGNAL(close()), this, SLOT(closeModel())); // Automatic Close via Model
    connect(b, SIGNAL(destructModel()), a, SLOT(saveJson())); // Save when closing
    connect(a, SIGNAL(setWindowName(QString)), b, SLOT(setWindowTitle(QString))); // Change window title
    a->checkToken();
    //Timer for auto-update
    QTimer * l = new QTimer();
    connect(l, SIGNAL(timeout()), a, SLOT(update()));
    l->start(250);
}
Controller::~Controller()
{
    closeModel();
}

void Controller::closeModel()
{
    // Close everything
    a->saveJson();
    delete a;
    delete b->ui;
    delete b;
    delete this;
}
