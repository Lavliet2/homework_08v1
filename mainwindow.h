#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "EmploeeModel.h"
#include "VacationModel.h"
#include "Chat/ChatWidget.h"


#include <QMainWindow>

//#include <QDate>
//#include <QThreadPool>
//#include <QRunnable>
//#include <QDebug>

/*
class SendEmailTask : public QRunnable
{
public:
    SendEmailTask(const QString& emailAddress, const QString& subject, const QString& body)
        : m_emailAddress(emailAddress), m_subject(subject), m_body(body)
    {
    }

    void run() override
    {
        qDebug() << "Отправка сообщения на адрес" << m_emailAddress;
        qDebug() << "Тема:" << m_subject;
        qDebug() << "Текст:" << m_body;
    }

private:
    QString m_emailAddress;
    QString m_subject;
    QString m_body;
};
*/

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool confirmDeletion();

    void dateOgonki();
    void dateLight(const QDate &eventDate, const QDate &currentDate, const QString &toolText, QColor color = Qt::green, bool isAppend = false);
    void removeRowFromModel(int row);
    void removeRowFromFile (int row);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void showEvent (QShowEvent  *event) override;
    void closeEvent(QCloseEvent *event) override;


private:
    void init();
    void readSettings();
    void vacation(int month);

private slots:
    void on_pb_Add_clicked();
    void on_pb_Del_clicked();
    void on_pb_Save_clicked();
    void on_pb_Sort_clicked();
    void on_pb_Send_clicked();

    void removeLineFromFile(const QString& filename, int lineIndex);
    void handleHeaderClicked(int logicalIndex);

    void on_tb_Style_clicked(bool checked);
    void on_tb_Vacation_clicked(bool checked);


//    void on_pb_Send_Shadow_clicked(); TODO
    void on_tb_Chat_clicked(bool checked);

private:
    Ui::MainWindow *ui;
    EmploeeModel   *eModel     = nullptr;
    VacationModel  *vModel     = nullptr;
//    ChatWidget     *chatWidget = nullptr;

    QString filePath;
    int sortClickCount = 0;

    bool isStyle    = false;
    bool isVacation = false;
    bool isChat     = false;
};
#endif // MAINWINDOW_H
