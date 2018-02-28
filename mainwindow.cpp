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
/*
 * Telegram simple Hangman Bot, here is link to standard json file configuration:
 * https://pastebin.com/ED8ZBwnE
 * Wordbank is already there, it's ~2800 words
 * For configuration you only need bottoken and put path to this file in the first line of the constructor:
*/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //Here
    //Pls here
    jsonpath = "D:/p/js.json"; // <----- Change it!
    //Not here, but there ^
    //                    \


    QFile newfile(jsonpath); // Открытие файла
    if(!newfile.open(QFile::ReadOnly))
    { }
    QByteArray data = newfile.readAll(); //JSON данные
    QJsonDocument d = QJsonDocument::fromJson(data);
    QJsonObject obj = d.object();
    QString temp = obj["bottoken"].toString();
    if(temp.isEmpty()) {
        QMessageBox msgbx;
        msgbx.setText("Error! Json not configured");
        msgbx.exec();
        QTimer::singleShot(0, this, SLOT(close()));
    } // Check if json is configured

    //Updating current data
    QJsonArray dusers = obj["users"].toArray();
    for(int i = 0; i < dusers.size(); i++) {
        QJsonObject t = dusers[i].toObject();
        users.push_back(t["username"].toString());
        nowplaying.push_back(t["nowplaying"].toBool());
        words.push_back(t["word"].toString());
        masks.push_back(t["mask"].toString());
        lives.push_back(t["lives"].toInt());
        usedLetters.push_back(t["usedLetters"].toString());
    }
    QJsonArray dunixs = obj["unixs"].toArray();
    for(int i = 0; i < dunixs.size(); i++) {
        QJsonObject t = dunixs[i].toObject();
        unixs.push_back(t["unix"].toInt());
    }
    QJsonArray dwordbank = obj["wordbank"].toArray();
    for(int i = 0; i < dwordbank.size(); i++) {
        QJsonObject t = dwordbank[i].toObject();
        wordbank.push_back(t["word"].toString());
    }

    bottoken = temp;
    newfile.close();
    //Auto Timer for checking messages
    QTimer * l = new QTimer();
    connect(l, SIGNAL(timeout()), this, SLOT(on_butGet_clicked()));
    l->start(500);
    ui->setupUi(this);
    //modifyJson();
}

void MainWindow::modifyJson()
{
    //Open
    QFile newfile(jsonpath); // Открытие файла
    if(!newfile.open(QFile::ReadWrite))
    { }
    //Prepare for writing
    QByteArray data = newfile.readAll(); //JSON данные
    QJsonDocument d = QJsonDocument::fromJson(data);
    QJsonObject obj = d.object();
    QJsonArray dusers;
    for(int i = 0; i < users.size(); i++)
    {
        QJsonObject t;
        t["username"] = users[i];
        t["nowplaying"] = nowplaying[i];
        t["word"] = words[i];
        t["mask"] = masks[i];
        t["lives"] = lives[i];
        t["usedLetters"] = usedLetters[i];
        dusers.append(t);
    }
    obj["users"] = dusers;

    QJsonArray dunixs;
    for(int i = 0; i < unixs.size(); i++)
    {
        QJsonObject t;
        t["unix"] = unixs[i];
        dunixs.append(t);
    }
    obj["unixs"] = dunixs;
    //Writing
    d = QJsonDocument(obj);
    data = d.toJson();
    newfile.resize(0);
    newfile.write(data);
    newfile.close();
}

MainWindow::~MainWindow()
{
    modifyJson();
    delete ui;
}
QString MainWindow::genMask(int len)
{
    //Generates masks based of length
    QString res;
    for(int i = 0; i < len; i++)
        res += '-';
    return res;
}

bool MainWindow::updateMask(int i, QString text)
{
    //Engine of the game, opens lettres
    QString prev = masks[i];
    for(int q = 0; q < words[i].size(); q++)
    {
        if(words[i][q] == text[0])
            masks[i][q] = text[0];
    }
    if(prev == masks[i])
        return false;
    else
        return true;
}
QString MainWindow::genWord()
{
    //Generates random word from wordbank in json
    if(true)
    {
        qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()));
        int id = qrand() % wordbank.size();
        return wordbank[id];
    }
    else
        return "SKYFISH"; // Just Debug extra feature, don't bother
}

QString MainWindow::compute(QString text, QString fname, QString lname, QString username)
{
    //Reply processment, add if to make another case
    QString reply;
    long long int i = users.indexOf(username);
    if(text == "/start")
        reply = "Hi, " + fname + "! Welcome to my Hangman Game. Type /play to start. Type /contact to contact our team.";
    else if(text == "/contact")
        reply = "So, to contact our team email us at a.kovrigin0@gmail.com or via telegram @alex_kovrigin. :D";
    else if(text == "/cancel") {
        if(nowplaying[i]) {
            nowplaying[i] = false;
            reply = "Roger-Dodger! Your current game is cancelled, word was: " + words[i] + ".";
            words[i] = "x";
            masks[i] = "y";
            lives[i] = 10;
            usedLetters.clear();
        }
        else
            reply = "Hm... My systems show that you're not in game currently. Type /play to start.";
    }
    else if(text == "/play")
    {
        words[i] = genWord();
        masks[i] = genMask(words[i].size());
        nowplaying[i] = true;
        reply = "Let's Play! Your word: " + masks[i] + ". You have 10 lives.";
    }
    else if(nowplaying[i] && text[0].isLetter() && text.size() == 1)
    {
        text = text.toUpper();
        if(usedLetters[i].indexOf(text[0]) != -1) {
            reply = "Sorry, you've already used this letter.";
            return reply;
        }
        usedLetters[i] += text;
        bool k = updateMask(i, text);
        if(!k && lives[i] > 0)
            reply = "Incorrect :\\. Your word: " + masks[i] + ". You have " + QString::number(--lives[i]) + " lives.";
        else if(!k) {
            reply = "You lost :(. The word was " + words[i] + ". Type /play for revenge.";
            nowplaying[i] = false;
            words[i] = "x";
            masks[i] = "y";
            lives[i] = 10;
            usedLetters.clear();
            }
        else if(words[i] == masks[i]) {
            reply = "Congratulations, you won! Type /play to start again.";
            nowplaying[i] = false;
            words[i] = "x";
            masks[i] = "y";
            lives[i] = 10;
            usedLetters.clear();
            }
        else
            reply = "Correct :D. Your word: " + masks[i] + ". You have " + QString::number(lives[i]) + " lives.";
    }
    else
        reply = "Hm... I don't understand what you mean, try again or contact our team - /contact";
    return reply;
}

void MainWindow::on_butGet_clicked()
{
    //Sends request with method getUpdates
    QByteArray rep = GetTelegramInfo("getUpdates");
    QJsonDocument d = QJsonDocument::fromJson(rep);
    QJsonObject ma = d.object();
    QJsonArray arr = ma["result"].toArray();
    for(int i = 0; i < arr.size(); i++)
    {
        //Process every message
        QJsonObject raw = arr[i].toObject();
        QJsonObject mes = raw["message"].toObject();
        QString text = mes["text"].toString();
        ui->txt->setPlainText(ui->txt->toPlainText() + "\n" + text);
        long long int unix = mes["date"].toInt();
        QJsonObject from = mes["from"].toObject();
        QString username = from["username"].toString();
        QString fname = from["first_name"].toString();
        QString lname = from["last_name"].toString();
        //If user not registered
        if(!users.contains(username)) {
            users.push_back(username);
            nowplaying.push_back(false);
            words.push_back("x");
            masks.push_back("y");
            lives.push_back(10);
        }
        //If message was already processed, because telegram answers all the messages
        //So based of the unix time when the message was sent, we identify it
        if(unixs.contains(unix)) {
            continue;
        }
        else
            ui->txt->setPlainText(ui->txt->toPlainText() + " -----> Sent"); // Debug
        //Reply generator
        QString reply = compute(text, fname, lname, username);
        QJsonObject chat = mes["chat"].toObject();
        int chat_id = chat["id"].toInt();
        sendMessage(chat_id, reply); // sending message
        unixs.push_back(unix); // remember the time
    }
}

QByteArray MainWindow::GetTelegramInfo(QString method)
{
    //------------------------------------------------------------------------------------------------------------------//
    // Attention!!! To run this you need to put ssleay32.dll and libeay32.dll to your build/debug or /release folder!!! //
    //------------------------------------------------------------------------------------------------------------------//
    QUrl url ("https://api.telegram.org/bot" + bottoken + "/" + method);
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request(url);
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1_2);
    request.setSslConfiguration(config);
    QNetworkReply *reply = manager->get(QNetworkRequest(request));

    QEventLoop eventLoop;
    QObject::connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    return reply->readAll();
}

bool MainWindow::sendMessage(long long chat_id, QString text)
{
    QByteArray responce = GetTelegramInfo("sendMessage?chat_id=" + QString::number(chat_id) + "&text=" + text);
    QJsonDocument doc = QJsonDocument::fromJson(responce);
    QJsonObject obj = doc.object();
    if(obj["ok"].toBool())
        return true;
    else
        return false;
}


