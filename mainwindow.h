#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Ui::MainWindow *ui;
private slots:
    void on_butGet_clicked();
    void updateTable(QVector <QString> info);
    void closeEvent(QCloseEvent *event);
    //void prepareMenu(QPoint pos);
    //void openProfile();
signals:
    void updateModel();
    void destructModel();
    void getProfile();
private:
};

#endif // MAINWINDOW_H
