#include "databese.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
databese::databese() {}

QByteArray databese::sendQuerry(QString text){
    return "ahdsbaa";
}

bool databese::openDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C:\\Users\\matveybarkanov\\Downloads\\tcpserverDataBase.db");

    if (!db.open()) {
        qDebug() << "Error: connection with database failed";
        return false;
    } else {
        qDebug() << "Database: connection ok";
        return true;
    }
}

bool databese::createTable()
{
    QSqlQuery query;
    bool success = query.exec("CREATE TABLE IF NOT EXISTS users ("
                              "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                              "username VARCHAR(50) UNIQUE, "
                              "password VARCHAR(30))");
    if (!success) {
        qDebug() << "Failed to create table:" << query.lastError();
    }
    return success;
}
bool registerUser(const QString& username, const QString& password) {
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password) VALUES (:username, :password)");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (!query.exec()) {
        qDebug() << "Add user error:" << query.lastError();
        return false;
    }
    return true;
}

