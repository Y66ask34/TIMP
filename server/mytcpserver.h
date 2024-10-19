#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QString>
#include <stdexcept>
#include "functionsforserver.h"

class MyTcpServer : public QObject
{
    Q_OBJECT
public:
    explicit MyTcpServer(QObject *parent = nullptr);
    ~MyTcpServer();

    static float f(float x, float a, float b, float c);
    static QString bisection_method(float a, float b, float c, float x_low, float x_high);
    static QString check_descr(float a, float b, float c);

public slots:
    void slotNewConnection();//!Метод для соединение с сервером
    void slotServerRead();//!Метод для чтения данных приходящих на сервер
    void slotClientDisconnected();//!Метод для рассоединеня с сервером

public:

    QTcpServer *mTcpServer;//!Поле которое слушает TCP соединение
    QTcpSocket *mTcpSocket;//!Поле которое слушает TCP соединение

    double findShortestPath(const QVector<QVector<double>> &graph, int startVertex, int endVertex);//!Метод для нахождения кратчайшего пути между вершинами графа
    QVector<QVector<double>> stringToGraph(const QString &str);//!Метод для перевода строки в граф для функции нахождения кратчайшего пути между вершинами графа
    QString graphToString(const QVector<QVector<double>> &graph);//!Метод для перевода графа в строку для отправки строки на клиент
    int binary_search(QString& arr, QString item);//! Метод биарного поиска (метод деления пополам)
    QString Shi_Vishener(const QString& data_from_the_user, const QString& key);//!Шифрование Виженера
    QString Deshi_Vishener(const QString& data_from_the_user, const QString& key);//!Дешифрока Виженера
    QByteArray functionality(QString data_from_client1, QString key, int socketDescr);//!Главный метод для вызова необходимых методов и отладочная информация


    database *db = database::getInstance();

    QMap<int, QTcpSocket*> mSocketDescriptors;
};

#endif // MYTCPSERVER_H
