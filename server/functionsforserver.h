#ifndef FUNCTIONSFORSERVER_H
#define FUNCTIONSFORSERVER_H
#include <QString>
#include <QList>
#include <QCryptographicHash>
#include "database.h"
#include <QByteArray>

QByteArray stat(int socketDescr, QString login);
QByteArray check(int socketDescr, QString var, QString userAnsw);
QByteArray auth(int socketDescr, QString log, QString pass);
QByteArray reg(QString log, QString pass, QString mail);


const float tolerance2 = 1e-6;

bool userDisconnect(int socketDescr);


#endif // FUNCTIONSFORSERVER_H
