#ifndef SETUP_H
#define SETUP_H

#include <QDialog>

namespace Ui {
class Setup;
}

class Setup : public QDialog
{
    Q_OBJECT

public:
    explicit Setup(QWidget *parent = 0);
    ~Setup();
    bool suc = false;
    QString bottoken;

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::Setup *ui;
};

#endif // SETUP_H
