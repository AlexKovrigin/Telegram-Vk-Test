#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSslConfiguration>
#include <QSsl>
#include <QtNetwork>
#include <QSslSocket>
#include <QDebug>
#include <QJsonDocument>
#include <QFile>
#include <QMessageBox>
#include <QTime>
#include <QTableWidgetItem>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //connect(ui->table, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(prepareMenu(QPoint)));
}

MainWindow::~MainWindow()
{
    delete ui;
    emit destructModel();
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    emit destructModel();
}
//Extra function, not currently in use
//void MainWindow::prepareMenu(QPoint pos)
//{
//    QTableWidgetItem * nd = ui->table->itemAt(pos);
//    QMenu menu(this);
//    if(nd != 0)
//    {
//        QAction *openAct = new QAction(QIcon(":/open.ico"), tr("&See user profile"), this);
//        connect(openAct, SIGNAL(triggered()), this, SLOT(openProfile()));
//        menu.addAction(openAct);
//    }
//    menu.exec(table->mapToGlobal(pos));
//}
void MainWindow::on_butGet_clicked()
{
    emit updateModel();
}
//Add row to table
void MainWindow::updateTable(QVector<QString> info)
{
    int row = ui->table->rowCount();
    ui->table->insertRow(row);
    for(int i = 0; i < info.size(); i++)
        ui->table->setItem(row, i, new QTableWidgetItem(info[i]));
}
