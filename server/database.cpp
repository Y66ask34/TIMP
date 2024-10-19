#include "database.h"

database *database::p_instance = nullptr;
databaseDestroyer database::destroyer;

database::database() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C:\\Users\\Herten\\Desktop\\LizaClSr\\server\\sqlite.db");
    if (!db.open()) {
        qDebug() << "Error opening database: " << db.lastError().text();
    }
}

database* database::getInstance() {
    if (!p_instance) {
        p_instance = new database();
        destroyer.initialize(p_instance);
    }
    return p_instance;
}

QStringList database::sendQuerry(const QString& text, const QVariantList& queryParams)
{
    QSqlQuery query(db);
    query.prepare(text);

   for (int i = 0; i < queryParams.size(); ++i) {
       query.bindValue(i, queryParams.at(i));
    }

    if (!query.exec()) {
        qDebug() << "Error executing query: " << query.lastError().text();
        return QStringList(  );
    }

    QStringList results;
    while (query.next()) {
        QStringList row;
        for (int i = 0; i < query.record().count(); i++) {
            row << query.value(i).toString();
        }
        results << row.join("\t");
    }
    return results;
}

bool database::userDisconnect(int socketDescr) {
    return p_instance->sendQuerry("UPDATE user set id_conn = NULL where id_conn = ?", {socketDescr}).isEmpty();
}

bool database::isUserCorrect(QString log, int socketDescr) {
    return !p_instance -> sendQuerry("SELECT 1 FROM user WHERE login = ? AND id_conn = ?", {log, socketDescr}).isEmpty();
}

QByteArray database::getLogin(int sockDescr) {
    return p_instance -> sendQuerry("select login from user where id_conn = ?", {sockDescr})[0].toUtf8();
}

QByteArray database::reg(QString log, QString hashedPass, QString mail) {
    qDebug() << "Database Debug:" << log << " " << hashedPass << " " << mail;

    QStringList result = p_instance->sendQuerry("SELECT * FROM user WHERE login = ?", {log});

    if (!result.isEmpty()) {
        return "reg-";
    }

    p_instance->sendQuerry(
        "INSERT INTO user (login,pass,mail,stat_task1,stat_task2,id_conn) VALUES (?, ?, ?, 0, 0, NULL) ",
        {log, hashedPass.toUtf8(), mail});

    return ("reg+&" + log).toUtf8();
}


QByteArray database::auth(int socketDescr, QString log, QString hashedPass) {

    // Проверка на существование пользователя
    QStringList result = p_instance->sendQuerry("SELECT * FROM user WHERE login = ?", {log});
    if (result.isEmpty()) {
        return "auth-";
    }

    // Проверка пароля на совпадение
    result = p_instance->sendQuerry("SELECT * FROM user WHERE login = ? AND pass = ?", {log, hashedPass.toUtf8()});
    if (result.isEmpty()) {
        return "auth-";
    }

    p_instance->sendQuerry(
        "UPDATE user set id_conn = ? where login = ?",
        {socketDescr, log}
        );

    return ("auth+&" + log).toUtf8();
}

QByteArray database::stat(int socketDescr, QString log) {
    QStringList results = p_instance -> sendQuerry("SELECT stat_task1, stat_task2 FROM user WHERE login = ? AND id_conn = ?", {log, socketDescr});
    QStringList row = results.at(0).split("\t");
    int statTask1 = row.at(0).toInt();
    int statTask2 = row.at(1).toInt();

    return QString("stat&%1&%2").arg(statTask1).arg(statTask2).toUtf8();
}

