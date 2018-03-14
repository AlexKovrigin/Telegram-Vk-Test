#include "setup.h"
#include "ui_setup.h"

Setup::Setup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Setup)
{
    ui->setupUi(this);
    suc = false;
}

Setup::~Setup()
{
    delete ui;
}

void Setup::on_buttonBox_accepted()
{
    //Accepted
    suc = true;
    bottoken = ui->lineEdit->text();
    close();
}

void Setup::on_buttonBox_rejected()
{
    //Declined
    close();
}
