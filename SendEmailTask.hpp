#ifndef SENDEMAILTASK_HPP
#define SENDEMAILTASK_HPP

#include <QThreadPool>
#include <QRunnable>
#include <QDebug>
#include <QPlainTextEdit>
#include <QtNetwork/QTcpSocket>
#include <QCoreApplication>
#include <QProcess>

class SendEmailTask : public QRunnable
{
public:
    SendEmailTask(const QString& emailAddress, const QString& subject, const QString& body)
        : m_emailAddress(emailAddress), m_subject(subject), m_body(body)
    {
    }

    void run() override
    {
        QPlainTextEdit *t = new QPlainTextEdit();
        t->setPlainText(QString("Отправка сообщения на адрес %1").arg( m_emailAddress));
        t->show();

        QString email = "Murzin_AV@surgutneftegas.ru";
        QString subject = "Subject";
        QString body = "Body";




        QStringList emails;
        emails.push_back("Karpov_DO1@surgutneftegas.ru");
        emails.push_back("murzin_av@surguteneftegas.ru");


        QStringList argum;
        argum << "mailto:" + email + "?subject=" + subject + "&body=" + body;

        QProcess process;
        process.startDetached("cmd", argum);
        process.waitForFinished(-1);
        t->appendPlainText(QString("Email code %1").arg(process.exitCode()));
        t->appendPlainText(QString("Error %1").arg(process.errorString()));

//        if ( process.waitForFinished() ) {
//            t->appendPlainText("Email send");
//        } else {
//            t->appendPlainText("Failed to send email.");
//}


                /*
        QPlainTextEdit *t = new QPlainTextEdit();
        t->setPlainText(QString("Отправка сообщения на адрес %1").arg( m_emailAddress));
        t->show();


        QString serverAddress = "10.124.13.34"; //"Tonipi-service@surgutneftegas.ru";
        int port = 25;

        QTcpSocket socket;
        socket.connectToHost("10.124.13.34", 25);
        if ( socket.waitForConnected() ) {
            t->setPlainText(QString("Ae connect").arg(serverAddress) + '\n');

            socket.write("EHLO localhost\r\n");
            socket.write("ENAIL FROM: <Tonipi-service@surgutneftegas.ru>\r\n");
            socket.write("RCPT TO: \r\n");
            socket.write("DATA\r\n");
            socket.write("Subject: Test email\r\n");
            socket.write("Hello this is a test email\r\n");
            socket.write(".\r\n");
            socket.write("EQUIT\r\n");
            socket.flush();

            if ( socket.waitForBytesWritten() && socket.waitForReadyRead() ) {
                t->appendPlainText("Email send" + socket.readAll() + '\n');
            }
            else {
                t->appendPlainText("Failed to send message \n");
            }
            socket.close();
        }

        else {
            t->appendPlainText("Connect to SMPT service failed");
        }
*/


        qDebug() << "Отправка сообщения на адрес" << m_emailAddress;
        qDebug() << "Тема:" << m_subject;
        qDebug() << "Текст:" << m_body;
    }

private:
    QString m_emailAddress;
    QString m_subject;
    QString m_body;
};


#endif // SENDEMAILTASK_HPP
