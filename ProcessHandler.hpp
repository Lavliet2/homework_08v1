#ifndef PROCESSHANDLER_HPP
#define PROCESSHANDLER_HPP

#include "EmploeeModel.h"

#include <QObject>
#include <QDebug>

class ProcessHandler : public QObject
{
    Q_OBJECT

public:
    ProcessHandler(Emploee *emploee) : emploee(emploee) {}

signals:
    void processFinished(int index, QString result);
public slots:
    void startProcess()
    {
        QProcess process;
        QString ip = emploee->ip;
        process.start("ping", QStringList() << "-n" << "1" << ip);
        process.waitForFinished(-1);
        QString pingOutput = process.readAllStandardOutput();
        QString result = process.readAll();
                if (pingOutput.contains("TTL="))
                    // IP-адрес доступен
                    emploee->ping = true;
                else
                    // IP-адрес недоступен
                    emploee->ping = false;
        emit processFinished(index, result);
    }
private:
    Emploee *emploee =nullptr;
    int index;

    friend class EmploeeModel;

};

#endif // PROCESSHANDLER_HPP
