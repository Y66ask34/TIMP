#include "mytcpserver.h"
#include "database.h"
#include <QDebug>
#include <QCoreApplication>
#include <QString>
#include <QVector>
#include <string>
#include <queue>
#include <stdexcept>
using namespace std;

const float tolerance = 1e-6;

MyTcpServer::~MyTcpServer()
{

    mTcpServer->close();
    //server_status=0;
}

MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent){
    mTcpServer = new QTcpServer(this);

    connect(mTcpServer, &QTcpServer::newConnection,
            this, &MyTcpServer::slotNewConnection);

    if(!mTcpServer->listen(QHostAddress::Any, 33333)){
        qDebug() << "server is not started";
    } else {
        //server_status=1;
        qDebug() << "server is started";
    }
}
void MyTcpServer::slotClientDisconnected() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        int socketDescriptor = socket->socketDescriptor();
        mSocketDescriptors.remove(socketDescriptor);
        socket->deleteLater();
        qDebug() << "Client disconnected:" << socketDescriptor;
    }
}

void MyTcpServer::slotNewConnection(){
    //   if(server_status==1){
    mTcpSocket = mTcpServer->nextPendingConnection();
    // mTcpSocket->write("Hello, World!!! I am echo server!\r\n");
    mSocketDescriptors[mTcpSocket ->socketDescriptor()] = mTcpSocket;
    connect(mTcpSocket, &QTcpSocket::readyRead,this,&MyTcpServer::slotServerRead);
    connect(mTcpSocket,&QTcpSocket::disconnected,this,&MyTcpServer::slotClientDisconnected);
    // }

    qDebug() << mTcpSocket->socketDescriptor();
}

void MyTcpServer::slotServerRead() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) {
        return;
    }

    QString res = "";
    // Чтение всех доступных данных из сокета
    while (socket->bytesAvailable() > 0) {
        QByteArray array = socket->readAll();
        qDebug() << array << "\n";
        res.append(array);
    }

    // Проверка, содержит ли строка символ разделителя '&'
    int keyIndex = res.lastIndexOf('&');
    if (keyIndex == -1) {
        qDebug() << "Error: No key provided (no separator)";
        socket->write("error: no key provided\r\n");
        return;
    }

    // Извлечение ключа и данных
    QString key = res.mid(keyIndex + 1);
    QString encrypted_data = res.left(keyIndex);
    qDebug() << "Key: " << key;
    // Проверка на пустой ключ
    if (key.isEmpty()) {
        qDebug() << "Error: No key provided (empty key)";
        socket->write("error: no key provided\r\n");
        return;
    }


    // Выполняем функциональность с дешифрованными данными
    QByteArray response;
    try {
        response = functionality(encrypted_data, key, socket->socketDescriptor());
    } catch (const std::invalid_argument& e) {
        qDebug() << "Error in functionality:" << e.what();
        response = "error\r\n";
    }

    // Отправка ответа обратно клиенту через сокет
    socket->write(response);
}
double MyTcpServer::findShortestPath(const QVector<QVector<double>>& graph, int startVertex, int endVertex)
{
    int n = graph.size();
    qDebug() << "Graph size:" << n;

    QVector<double> dist(n, std::numeric_limits<double>::max());
    QVector<bool> visited(n, false);

    if (startVertex >= n || endVertex >= n || startVertex < 0 || endVertex < 0) {
        qDebug() << "Invalid vertex indices: startVertex =" << startVertex << ", endVertex =" << endVertex << ", n =" << n;
        throw std::out_of_range("Индекс вершины выходит за допустимые пределы");
    }

    dist[startVertex] = 0;
    std::priority_queue<std::pair<double, int>, QVector<std::pair<double, int>>, std::greater<std::pair<double, int>>> pq;

    pq.push({0, startVertex});

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();
        if (visited[u]) continue;
        visited[u] = true;

        qDebug() << "Visiting vertex:" << u;

        for (int v = 0; v < n; ++v) {
            if (u >= 0 && u < n && v >= 0 && v < graph[u].size() && graph[u][v] && !visited[v] && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                pq.push({dist[v], v});
                qDebug() << "Updating distance for vertex:" << v << " with distance:" << dist[v];
            }
        }
    }

    qDebug() << "Returning shortest path distance:" << dist[endVertex];
    return dist[endVertex];
}

QVector<QVector<double>> MyTcpServer::stringToGraph(const QString& str)
{
    QVector<QVector<double>> graph;
    QStringList rows = str.split(';');
    qDebug() << "Rows count:" << rows.size();
    for (const auto& row : rows) {
        QStringList cols = row.split(',');
        QVector<double> graphRow;
        qDebug() << "Row:" << row;
        for (const auto& col : cols) {
            qDebug() << "Column:" << col;
            bool ok;
            double value = col.toDouble(&ok);
            if (ok) {
                graphRow.push_back(value);
            } else {
                qDebug() << "Invalid value in string:" << col;
                throw std::invalid_argument("Некорректное значение в строке");
            }
        }
        if (!graphRow.isEmpty()) {
            graph.push_back(graphRow);
        }
    }

    // Проверка равенства длин всех строк
    int rowLength = graph[0].size();
    for (const auto& row : graph) {
        if (row.size() != rowLength) {
            qDebug() << "Row size mismatch. Expected:" << rowLength << ", Found:" << row.size();
            throw std::invalid_argument("Некорректная длина строки в графе");
        }
    }

    return graph;
}

QString MyTcpServer::graphToString(const QVector<QVector<double>>& graph)
{
    QStringList rows;
    for (const auto& row : graph) {
        QStringList cols;
        for (double value : row) {
            cols.push_back(QString::number(value));
        }
        rows.push_back(cols.join(','));
    }
    return rows.join(';');
}

int MyTcpServer::binary_search(QString &arr, QString item) {
    int low = 0;
    int high = arr.size() - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        QChar guess = arr[mid];
        if (guess == item) {
            return mid;
        } else if (guess > item) {
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }
    return -1; // Возвращаем -1, если элемент не найден
}


QString MyTcpServer::Shi_Vishener(const QString& data_from_the_user, const QString& key) {
    if (data_from_the_user.isEmpty()) {
        throw std::invalid_argument("The string is empty");
    }
    if (key.isEmpty()) {
        throw std::invalid_argument("No key");
    }

    QString table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789&|,;-^+_";
    QVector<int> key_value;
    QVector<int> message_value;
    QVector<int> before_final;
    QString encrypted;

    // Преобразование входных данных в индексы таблицы
    for (const QChar& value : data_from_the_user) {
        int index = table.indexOf(value);
        if (index == -1) {
            qDebug() << "Invalid character in input string:" << value;
            throw std::invalid_argument("Invalid character in input string");
        }
        message_value.push_back(index);
    }

    // Преобразование ключа в индексы таблицы
    for (int i = 0; i < data_from_the_user.size(); ++i) {
        QChar k = key[i % key.size()];
        int index = table.indexOf(k);
        if (index == -1) {
            qDebug() << "Invalid character in key:" << k;
            throw std::invalid_argument("Invalid character in key");
        }
        key_value.push_back(index);
    }

    // Шифрование сообщения
    for (int i = 0; i < data_from_the_user.size(); ++i) {
        int encrypted_index = (message_value[i] + key_value[i]) % table.size();
        before_final.push_back(encrypted_index);
    }

    // Преобразование зашифрованных индексов обратно в символы
    for (int index : before_final) {
        encrypted.append(table[index]);
    }

    return data_from_the_user;
}



QString MyTcpServer::Deshi_Vishener(const QString& data_from_the_user, const QString& key) {
    if (data_from_the_user.isEmpty()) {
        throw std::invalid_argument("The string is empty");
    }
    if (key.isEmpty()) {
        throw std::invalid_argument("No key");
    }

    QString table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789&|,;-^+_";
    QVector<int> deshi_data;
    QVector<int> key_value;

    // Преобразование входных данных в индексы таблицы
    for (const QChar& ch : data_from_the_user) {
        int index = table.indexOf(ch);
        if (index == -1) {
            qDebug() << "Invalid character in input string:" << ch;
            throw std::invalid_argument("Invalid character in input string");
        }
        deshi_data.push_back(index);
    }

    // Преобразование ключа в индексы таблицы
    for (int i = 0; i < deshi_data.size(); ++i) {
        QChar k = key[i % key.size()];
        int index = table.indexOf(k);
        if (index == -1) {
            qDebug() << "Invalid character in key:" << k;
            throw std::invalid_argument("Invalid character in key");
        }
        key_value.push_back(index);
    }

    QString result;
    int tableSize = table.size();
    for (int i = 0; i < deshi_data.size(); ++i) {
        // Декодирование символа
        int decodedIndex = (deshi_data[i] - key_value[i] + tableSize) % tableSize;
        result.append(table[decodedIndex]);
    }

    return data_from_the_user;
}


float MyTcpServer::f(float x, float a, float b, float c)
{
    return a * x * x + b * x + c;
}

QString MyTcpServer::bisection_method(float a, float b, float c, float x_low, float x_high){
    qDebug() << "function called";
    QString res;

    if (f(x_low, a, b, c) * f(x_high, a, b, c) >= 0)
    {
        res = QString::number(NAN);
        return res;
    }

    while ((x_high - x_low) / 2.0 > tolerance)
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

QString MyTcpServer::check_descr(float a, float b, float c){
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



//findShortestPath&0&4&0,2,0,6,0;2,0,3,8,5;0,3,0,0,7;6,8,0,0,9;0,5,7,9,0&Qword
//1GO6oFPE^CTGl-U&QYlRQYnPSYnTSYoPSYnQSgnSTYnQSYnNSfoTSgnNSYnWTYnSSfnWSYltCMS6
QByteArray MyTcpServer::functionality(QString data_from_client, QString key, int socketDescr) {
    qDebug() << "Received data:" << data_from_client;


    QString decrypted_data;
    try {
        decrypted_data = Deshi_Vishener(data_from_client, key);
    } catch (const std::invalid_argument& e) {
        qDebug() << "Error in Deshi_Vishener:" << e.what();
        return "error\r\n";
    }
    qDebug() << "Data unencrypted: " << decrypted_data;

    for (QChar &c : decrypted_data) {
        if (c == '\n') {
            c = '_'; // заменяем символ новой строки
        } else if (c == '\r') {
            c = '^'; // заменяем символ возврата каретки
        }
    }

    QStringList data_from_client_list = decrypted_data.split(QLatin1Char('&'));

    qDebug() << "Number of parameters received:" << data_from_client_list.size();

    if (data_from_client_list.size() < 2) {
        qDebug() << "Error: not enough parameters";
        return "error\r\n";
    }

    QString func_name = data_from_client_list.first();
    data_from_client_list.removeFirst();
    qDebug() << "Function name:" << func_name;
    qDebug() << "Parameters:" << data_from_client_list;

    if (func_name == "auth") {
        if (data_from_client_list.size() != 2) {
            qDebug() << "Error: auth expects exactly 2 parameters";
            return "error\r\n";
        }
        return Shi_Vishener(auth(socketDescr, data_from_client_list.at(0), data_from_client_list.at(1).trimmed()), data_from_client_list.at(0).toUtf8()).toUtf8();
    } else if (func_name == "reg") {
        if (data_from_client_list.size() != 3) {
            qDebug() << "Error: reg expects exactly 3 parameters";
            return "error\r\n";
        }
        return Shi_Vishener(reg(data_from_client_list.at(0), data_from_client_list.at(1), data_from_client_list.at(2).trimmed()), data_from_client_list.at(0).toUtf8()).toUtf8();
    } else if (func_name == "binary_search") {
        if (data_from_client_list.size() != 2) {
            qDebug() << "Error: binary_search expects exactly 2 parameters";
            return "error\r\n";
        }
        QString array = data_from_client_list.at(0);
        QString item = data_from_client_list.at(1);
        int index = binary_search(array, item);
        QString login = db -> getLogin(socketDescr);
        qDebug() << "binary_search result:" << index;
        return Shi_Vishener(QString::number(index).toUtf8(), login).toUtf8();

    } else if(func_name == "bisection_method") {
        QString temp1, temp2, d;
        d = check_descr((data_from_client_list.at(0)).toFloat(), (data_from_client_list.at(1)).toFloat(),
                        (data_from_client_list.at(2)).toFloat());
        if (d == "no_roots"){
            return QString("0").toUtf8() + "&aa";
        }
        else if (d == "1_root"){
            return (QString("check+&x = ") + QString::number(-((data_from_client_list.at(1)).toFloat()) / (2 * (data_from_client_list.at(0)).toFloat()))).toUtf8() + "&ff";
        }
        else {
            temp1 = (bisection_method((data_from_client_list.at(0)).toFloat(), (data_from_client_list.at(1)).toFloat(),
                                      (data_from_client_list.at(2)).toFloat(), -1000, 0));
            temp2 = (bisection_method((data_from_client_list.at(0)).toFloat(), (data_from_client_list.at(1)).toFloat(),
                                                (data_from_client_list.at(2)).toFloat(), 0, 1000));
            if(temp1 == "nan" || temp2 == "nan"){
                return QString("check+&0&dd").toUtf8();
            }
            else{
                return QString("check+&" + temp1 + "&" + temp2).toUtf8();
            }
        }
    }

    else if (func_name == "findShortestPath") {
        qDebug() << "Parameters received:" << data_from_client_list;

        if (data_from_client_list.size() != 3) {
            qDebug() << "Error: findShortestPath expects exactly 3 parameters";
            return "error\r\n";
        }

        bool ok1, ok2;
        int startVertex = data_from_client_list.at(0).toInt(&ok1);
        int endVertex = data_from_client_list.at(1).toInt(&ok2);
        if (!ok1 || !ok2) {
            qDebug() << "Error: Invalid vertex indices";
            return "Invalid vertex index\r\n";
        }
        qDebug() << "Start vertex:" << startVertex << ", End vertex:" << endVertex;

        // Преобразование строки в граф
        QVector<QVector<double>> graph;
        try {
            graph = stringToGraph(data_from_client_list.at(2));
        } catch (const std::invalid_argument& e) {
            qDebug() << "Error in stringToGraph:" << e.what();
            return "error\r\n";
        }

        qDebug() << "Graph converted from string:";
        for (const auto &row : graph) {
            QString rowOutput;
            for (double val : row) {
                rowOutput.append(QString::number(val) + " ");
            }
            qDebug() << rowOutput;
        }

        double shortestPath;
        try {
            shortestPath = findShortestPath(graph, startVertex, endVertex);
            qDebug() << shortestPath;
        } catch (const std::out_of_range& e) {
            qDebug() << "Error in findShortestPath:" << e.what();
            return "error\r\n";
        }

        qDebug() << "Shortest path result:" << shortestPath;
        return Shi_Vishener(QString::number(shortestPath), key).toUtf8() + "\r\n";
    }

    qDebug() << "Error: Unknown function";
    return "Unknown function\r\n";
}
