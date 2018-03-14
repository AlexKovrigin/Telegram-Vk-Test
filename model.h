#ifndef MODEL_H
#define MODEL_H
#include <QSslConfiguration>
#include <QSsl>
#include <QtNetwork>
#include <QSslSocket>
#include <QDebug>
#include <QJsonDocument>
#include <QFile>
#include <QMessageBox>
#include <QTime>
#include <QMainWindow>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QVector>
#include <setup.h>
#include <QFileInfo>

class Model : public QObject
{
    Q_OBJECT
public:
    Model();
    ~Model();
public slots:
    void update();
    void saveJson();
    bool checkToken();
signals:
    void updateTable(QVector <QString> info);
    void close();
    void setWindowName(QString name);
private:
    bool firstSetup();
    QByteArray GetTelegramInfo(QString method);
    QByteArray makeRequest(QString purl);
    QString bottoken;
    bool sendMessage(long long int chat_id, QString text, QString username);
    QString compute(QString text, QString fname, QString lname, QString username);
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
    void alert(QString message);
    QJsonObject readJson(QString path);
    void writeJson(QString path, QJsonObject obj);
    void writeData(QString path, QByteArray data);
    QString genKeyboard(QString username);

};

#endif // MODEL_H
