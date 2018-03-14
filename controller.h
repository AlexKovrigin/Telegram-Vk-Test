#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <mainwindow.h>
#include <model.h>
#include <QObject>
#include <QTimer>
class Controller : public QObject
{
    Q_OBJECT
public:
    //explicit Controller(QWidget *parent = 0);
    Controller();
    ~Controller();
public slots:
    void closeModel();
private:
    Model * a;
    MainWindow * b;
};

#endif // CONTROLLER_H
