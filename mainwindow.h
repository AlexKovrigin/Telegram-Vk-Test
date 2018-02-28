#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QtNetwork/QSslSocket>
#include <QVector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_butGet_clicked();

private:
    //Set this Path!!!
    QString jsonpath = "D:/p/js.json";

    Ui::MainWindow *ui;
    QByteArray GetTelegramInfo(QString method);
    QString bottoken = "497445504:AAFMbln-GuA9QI-dIzpG6gf3h8_pmXMKLCM";
    bool sendMessage(long long int chat_id, QString text);
    QString compute(QString text, QString fname, QString lname, QString username);
    void modifyJson();
    QVector <long long int> unixs;
    QVector <QString> users;
    QVector <bool> nowplaying;
    QVector <QString> words;
    QVector <QString> masks;
    QVector <QString> usedLetters;
    QVector <QString> wordbank;
    QVector <int> lives;
    QString genWord();
    QString genMask(int len);
    bool updateMask(int i, QString text);

};

#endif // MAINWINDOW_H
