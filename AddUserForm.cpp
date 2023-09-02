#include <ws2tcpip.h>
#include <winsock.h>

#include "AddUserForm.h"
#include "ui_AddUserForm.h"
#include <QDebug>
#include <QMessageBox>

#include "EmploeeModel.h"
//#include "PingTask.hpp"
#include <QProcess>


#include <QCoreApplication>


AddUserForm::AddUserForm(Emploee &employer, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddUserForm)
    , emploee(employer)
{
    ui->setupUi(this);
}


void AddUserForm::done(int result)
{
    if ( result != QDialog::Accepted ) {
        QDialog::done(result);
    }
    else {

        QStringList errorList;

        emploee.name     = ui->editline_Name->text();
        emploee.famili   = ui->editline_Famili->text();
        emploee.otch     = ui->lineEdit_Otch->text();
        emploee.post     = ui->lineEdit_Post->text();
        emploee.birthday = ui->dateEdit->date().toString("dd.MM.yyyy");
        emploee.email    = ui->lineEdit_Email->text();
        emploee.phone    = ui->lineEdit_Phone->text();
        emploee.gender   = ui->cb_Gender->currentText();        

        emploee.ip       = ui->lineEdit_IP->text();


        if ( emploee.name.isEmpty()     )  errorList << "Поле «Имя» пустое";
        if ( emploee.famili.isEmpty()   )  errorList << "Поле «Фамилия» пустое";
        if ( emploee.otch.isEmpty()     )  errorList << "Поле «Отчество» пустое";
        if ( emploee.post.isEmpty()     )  errorList << "Поле «Должность» пустое";
        if ( emploee.birthday.isEmpty() )  errorList << "Поле «Дата рождения» пустое";
        if ( emploee.email.isEmpty()    )  errorList << "Поле «Почта» пустое";
        if (!emploee.email.contains("@") || !emploee.email.contains(".")  )
            errorList    << "Поле «Почта» заполнено некорректно, отсутствуют специальные символы «@»«.»";

        if ( !errorList.isEmpty() ) {

            QMessageBox::warning(nullptr, "Предупреждение", errorList.join(";\n"));
            return;
        }

        QDialog::done(result);
    }
}


AddUserForm::~AddUserForm()
{
    delete ui;
}

void AddUserForm::on_tb_getIp_clicked()
{
    WSADATA wsaData;
    if ( WSAStartup(MAKEWORD(2, 2), &wsaData) != 0 ) {
        qDebug() << "ping WSAStartup " ;
        return;
    }
    char hostName[256];
    if ( gethostname(hostName, sizeof(hostName)) != 0 ) {
        qDebug() << "ping  gethostname" ;
        WSACleanup();
        return;
    }

    struct addrinfo *result = nullptr;
    if ( getaddrinfo(hostName, nullptr, nullptr, &result) != 0 ) {
        qDebug() << "ping  getaddrinfo" ;
        WSACleanup();
        return;
    }

    struct sockaddr_in *sockadd_ipv4 = reinterpret_cast<struct sockaddr_in *>(result->ai_addr);
    qDebug() << "Ip address" << inet_ntoa(sockadd_ipv4->sin_addr);

    freeaddrinfo(result);
    WSACleanup();

    ui->lineEdit_IP->setText(inet_ntoa(sockadd_ipv4->sin_addr));

}
