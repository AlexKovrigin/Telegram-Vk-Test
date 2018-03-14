#include "model.h"
//Our model class
//Basic constructor
Model::Model()
{
    //Check if config file exists
    QFileInfo check_file("config.json");
    if (!check_file.exists() || !check_file.isFile()) {
        if(!firstSetup()) {
            emit close();
            return;
        }
    }
    QJsonObject obj = readJson("config.json");
    bottoken = obj["bottoken"].toString();
    //Read users file
    obj = readJson("users.json");
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
    //Read unixs, unixs are ids of every message (it's so weirdly named because previously as this id unix time stamp was used)
    obj = readJson("unixs.json");
    QJsonArray dunixs = obj["unixs"].toArray();
    for(int i = 0; i < dunixs.size(); i++) {
        QJsonObject t = dunixs[i].toObject();
        unixs.push_back(t["unix"].toInt());
    }
    //Read Wordbank
    obj = readJson("wordbank.json");
    QJsonArray dwordbank = obj["wordbank"].toArray();
    for(int i = 0; i < dwordbank.size(); i++) {
        QJsonObject t = dwordbank[i].toObject();
        wordbank.push_back(t["word"].toString());
    }
}
Model::~Model()
{
    saveJson();
}
// First time installation protocol
bool Model::firstSetup()
{
    bool first = true;
    //Get bottoken
    do {
        if(!first)
            alert("Your bottoken is incorrect!");
        Setup menu;
        menu.exec();
        if(!menu.suc)
            return false;
        bottoken = menu.bottoken;
        first = false;
    } while(!checkToken());
    // We install standard json config files from pastebin
    QJsonObject obj;
    obj["bottoken"] = bottoken;
    writeJson("config.json", obj);
    QByteArray wbnk = makeRequest("https://pastebin.com/raw/xKv0X2Tg");
    writeData("wordbank.json", wbnk);
    QByteArray basicUsers = makeRequest("https://pastebin.com/raw/bWqhfQEA");
    writeData("users.json", basicUsers);
    QByteArray basicUnixs = makeRequest("https://pastebin.com/raw/b7yPH0fv");
    writeData("unixs.json", basicUnixs);
    return true;
}
//Standard check of incoming messages
void Model::update()
{
    //Sends request to get all messages with method getUpdates
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
        long long int unix = raw["update_id"].toInt();
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
            usedLetters.push_back("");
        }
        //If message was already processed, because telegram answers all the messages
        //So based of update id, we identify it
        if(unixs.contains(unix)) {
            continue;
        }
        else
        {
            //Update table in view
            QVector <QString> r;
            r.push_back(text);
            r.push_back(username);
            emit updateTable(r);
        }
        //Reply generator
        QString reply = compute(text, fname, lname, username);
        QJsonObject chat = mes["chat"].toObject();
        int chat_id = chat["id"].toInt();
        sendMessage(chat_id, reply, username); // sending message
        unixs.push_back(unix); // remember the time
    }
}
//Make request to telegram api
QByteArray Model::GetTelegramInfo(QString method)
{
    //------------------------------------------------------------------------------------------------------------------//
    // Attention!!! To run this you need to put ssleay32.dll and libeay32.dll to your build/debug or /release folder!!! //
    //------------------------------------------------------------------------------------------------------------------//
    return makeRequest("https://api.telegram.org/bot" + bottoken + "/" + method);
}
//Get pure html from request
QByteArray Model::makeRequest(QString purl)
{
    QUrl url (purl);
    QNetworkAccessManager *manager = new QNetworkAccessManager();
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
//Send message to exact person with cool keyboard
bool Model::sendMessage(long long chat_id, QString text, QString username)
{
    QString prereq = "sendMessage?chat_id=" + QString::number(chat_id) + "&text=" + text;
    QString keyboard = genKeyboard(username);
    prereq += "&reply_markup=" + keyboard;
    QByteArray responce = GetTelegramInfo(prereq);
    QJsonDocument doc = QJsonDocument::fromJson(responce);
    QJsonObject obj = doc.object();
    return obj["ok"].toBool();
}
//Compute the reply
QString Model::compute(QString text, QString fname, QString lname, QString username)
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
            usedLetters[i].clear();
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
        if(!k && lives[i] > 1)
            reply = "Incorrect :\\. Your word: " + masks[i] + ". You have " + QString::number(--lives[i]) + " lives.";
        else if(!k) {
            reply = "You lost :(. The word was " + words[i] + ". Type /play for revenge.";
            nowplaying[i] = false;
            words[i] = "x";
            masks[i] = "y";
            lives[i] = 10;
            usedLetters[i].clear();
            }
        else if(words[i] == masks[i]) {
            reply = "Congratulations, you won! Type /play to start again.";
            nowplaying[i] = false;
            words[i] = "x";
            masks[i] = "y";
            lives[i] = 10;
            usedLetters[i].clear();
            }
        else
            reply = "Correct :D. Your word: " + masks[i] + ". You have " + QString::number(lives[i]) + " lives.";
    }
    else
        reply = "Hm... I don't understand what you mean, try again or contact our team - /contact";
    return reply;
}
//We save local changes to permanent files at the closing
void Model::saveJson()
{
    QJsonObject obj = readJson("users.json");
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
    writeJson("users.json", obj);

    obj = readJson("unixs.json");
    QJsonArray dunixs;
    for(int i = 0; i < unixs.size(); i++)
    {
        QJsonObject t;
        t["unix"] = unixs[i];
        dunixs.append(t);
    }
    obj["unixs"] = dunixs;
    writeJson("unixs.json", obj);
}
//Generates a word from wordlist
QString Model::genWord()
{
    qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()));
    int id = qrand() % wordbank.size();
    return wordbank[id];
}
//Generates masks based of length
QString Model::genMask(int len)
{
    QString res;
    for(int i = 0; i < len; i++)
        res += '-';
    return res;
}
//Engine of the game, opens lettres
bool Model::updateMask(int i, QString text)
{
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
//Gives user information of something going wrong
void Model::alert(QString message)
{
    QMessageBox msgbx;
    msgbx.setText(message);
    msgbx.exec();
}
//Reads json from file
QJsonObject Model::readJson(QString path)
{
    QFile file(path);
    if(!file.open(QFile::ReadOnly))
    {
        alert("Unable to open config file: " + path);
        emit close();
    }
    QByteArray data = file.readAll(); //JSON данные
    QJsonDocument d = QJsonDocument::fromJson(data);
    QJsonObject obj = d.object();
    if(obj.isEmpty())
    {
        alert("Invalid format of config file: " + path);
        emit close();
    }
    file.close();
    return obj;
}
//Writes json to file
void Model::writeJson(QString path, QJsonObject obj)
{
    QFile file(path);
    if(!file.open(QFile::WriteOnly))
    {
        alert("Unable to open config file: " + path);
        emit close();
    }
    QJsonDocument d = QJsonDocument(obj);
    QByteArray data = d.toJson();
    file.resize(0);
    file.write(data);
    file.close();
}
//Writes data to file
void Model::writeData(QString path, QByteArray data)
{
    QFile file(path);

    if(!file.open(QFile::WriteOnly))
    {
        alert("Unable to open file: " + path);
        emit close();
    }
    file.resize(0);
    file.write(data);
    file.close();
}
//Generates keyboard for user
QString Model::genKeyboard(QString username)
{
    long long int o = users.indexOf(username);
    if(!nowplaying[o]) {
        QJsonObject obj;
        obj["remove_keyboard"] = true;
        QJsonDocument d = QJsonDocument(obj);
        QString req = d.toJson(QJsonDocument::Compact);
        return req;
    }
    QString qwerty = "QWERTYUIOPASDFGHJKLZXCVBNM";
    QString keyboard;
    for(int i = 0; i < qwerty.size(); i++)
    {
        if(usedLetters[o].indexOf(qwerty[i]) == -1)
            keyboard += qwerty[i];
    }
    double c = (double) keyboard.size(); - (double) usedLetters[o].size();
    int l = ceil(c / 3);
    int h = c - 2 * l;
    QVector <QString> keys;
    keys.push_back(keyboard.left(l));
    keys.push_back(keyboard.mid(l, l));
    keys.push_back(keyboard.right(h));
    QJsonArray k;
    for(int i = 0; i < 3; i++){
        QJsonArray t;
        for(int q = 0; q < keys[i].size(); q++)
            t.append(QString(keys[i][q]));
        k.append(t);
    }
    QJsonObject obj;
    obj["keyboard"] = k;
    QJsonDocument d = QJsonDocument(obj);
    QString req = d.toJson(QJsonDocument::Compact);
    return req;
}
//Checks if bottoken is valid
bool Model::checkToken()
{
    QByteArray data = GetTelegramInfo("getMe");
    QJsonObject obj = (QJsonDocument::fromJson(data)).object();
    bool ok = obj["ok"].toBool();
    if(!ok)
        return false;
    QString name = (obj["result"].toObject())["first_name"].toString();
    emit setWindowName(name);
    return true;
}
