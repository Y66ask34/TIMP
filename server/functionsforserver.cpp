#include "functionsforserver.h"
#include "database.h"

QByteArray stat(int socketDescr, QString log) {
    database *db = database::getInstance();

    if (!db->isUserCorrect(log, socketDescr)) {
        return "stat-";
    }

    return db -> stat(socketDescr, log);
}

QByteArray check(int socketDescr, QString var, QString userAnsw) {

    return "check\r\n";
}

bool userDisconnect(int socketDescr) {
    database *db = database::getInstance();
    return db ->userDisconnect(socketDescr);
}


QByteArray auth(int socketDescr, QString log, QString pass) {
    if((log == "") || (pass == "")) {
        return "auth-";
    }

    QCryptographicHash hasher(QCryptographicHash::Sha512);
    hasher.addData(pass.toUtf8());
    QByteArray hashedPass = hasher.result().toHex();

    database *db = database::getInstance();
    return db ->auth(socketDescr, log, hashedPass);
}

QByteArray reg(QString log, QString pass, QString mail) {
    if((log == "") || (pass == "") || (mail == "")) {
        return "reg-";
    }

    QCryptographicHash hasher(QCryptographicHash::Sha512);
    hasher.addData(pass.toUtf8());
    QByteArray hashedPass = hasher.result().toHex();

    database *db = database::getInstance();
    return db ->reg(log, hashedPass, mail);
}




float f(float x, float a, float b, float c)
{
    return a * x * x + b * x + c;
}

QString bisection_method(float a, float b, float c, float x_low, float x_high){
    qDebug() << "function called";
    QString res;

    if (f(x_low, a, b, c) * f(x_high, a, b, c) >= 0)
    {
        res = QString::number(NAN);
        return res;
    }

    while ((x_high - x_low) / 2.0 > tolerance2)
    {
        float x_mid = (x_high + x_low) / 2.0;
        if (f(x_mid, a, b, c) == 0)
        {
            res = QString::number(x_mid);
            //qDebug() << res.toFloat();
            return res;
        }
        else if (f(x_low, a, b, c) * f(x_mid, a, b, c) < 0)
        {
            x_high = x_mid;
        }
        else
        {
            x_low = x_mid;
        }
    }
    res = QString::number((x_high + x_low) / 2.0);
    qDebug() << res.toFloat();
    return res;
}

QString check_descr(float a, float b, float c){
    float d = b * b - 4 * a * c;
    if (d < 0)
    {
        return QString("no_roots");
    }
    else if (d == 0)
    {
        return QString("1_root");
    }
    return QString("2_roots");
}


