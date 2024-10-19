#include "authregform.h"
#include "ui_authregform.h"

#include "functionsforclient.h"
#include <singletonclient.h>


    AuthRegForm::AuthRegForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AuthRegForm)
{
    ui->setupUi(this);
    ui->label_email->setVisible(false);
    ui->lineEdit_email->setVisible(false);
    ui->pushButton_registration->setVisible(false);
    ui->pushButton_adminlogin->setVisible(false);

    ui->lineEdit_login->setPlaceholderText("Username");
    ui->lineEdit_password->setPlaceholderText("Password");
    ui->lineEdit_email->setPlaceholderText("Email");
    ui->lineEdit_password->setEchoMode(QLineEdit::Password);

    SingletonClient* client = SingletonClient::getInstance();
    connect(client, &SingletonClient::message_from_server, this, &AuthRegForm::handleAuthResponse);
}

AuthRegForm::~AuthRegForm()
{
    delete ui;
}

void AuthRegForm::on_pushButton_auth_clicked()
{
    QString username = ui->lineEdit_login->text();
    QString password = ui->lineEdit_password->text();
    SingletonClient* client = SingletonClient::getInstance();
    client->send_msg_to_server(username, password);
    FunctionsForClient::save_login(username);
}

void AuthRegForm::handleAuthResponse(const QString& response)
{
    qDebug() << response;
    QStringList parts = response.split("&");
    if (parts.size() > 0 && parts[0] == "auth+")
        FunctionsForClient::is_auth();
    else if(parts.size() > 0 && parts[0] == "reg+") {
        on_pushButton_auth_clicked();
    }
    else if (parts.size() > 0 && parts[0] == "check+") {
        parts.pop_front();
        FunctionsForClient::write_answer(parts.join(" "));
    }
    else if (parts.size() > 0 && parts[0] == "check-") {
        FunctionsForClient::send_clicked(0);
    }
    else if (parts.size() > 0 && parts[0] == "statistic+")
    {
        FunctionsForClient::statistic_received_user(response);
    }

    else
    {
        ui->lineEdit_login->setText("");
        ui->lineEdit_password->setText("");
    }
}
void AuthRegForm::on_pushButton_registration_clicked()
{
    QString username = ui->lineEdit_login->text();
    QString password = ui->lineEdit_password->text();
    QString email = ui->lineEdit_email->text();
    SingletonClient::getInstance()->send_msg_to_server_reg(username, password, email);
}



void AuthRegForm::on_pushButton_email_clicked()
{
    bool flag = ui->label_email->isVisible();
    if (!flag)
    {
        ui->pushButton_email->setText("Sign in");
    }
    else
    {
        ui->pushButton_email->setText("Sign up");
    }
    ui->pushButton_adminlogin->setVisible(false);
    ui->label_email->setVisible(!flag);
    ui->lineEdit_email->setVisible(!flag);
    ui->pushButton_registration->setVisible(!flag);
    ui->pushButton_auth->setVisible(flag);

}
