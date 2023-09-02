#ifndef VACATIONMODEL_H
#define VACATIONMODEL_H

#include "EmploeeModel.h"

#include <QAbstractTableModel>
#include <QDate>
#include <QStyledItemDelegate>
#include <QPainter>

struct EmploeeVacation
{
    QString fio;
    QString type;
    QList<bool> dayList;
    QColor color = Qt::yellow;
};

class VacationModel : public QAbstractTableModel
{
public:
    VacationModel(EmploeeModel *eModel, QComboBox *cb, QObject *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int  rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int  columnCount(const QModelIndex &parent = QModelIndex()) const override;

    void addItem(EmploeeVacation * ev);
    void clear();


//    void fiilData();
     QList<EmploeeVacation *> getEmploeeVacationList() {return emploeeVacationList;};

private:
    EmploeeModel *eModel;
    QList<EmploeeVacation *> emploeeVacationList;
    QComboBox *cb;

    QStringList headers;
};



class VacationDelegate : public QStyledItemDelegate
{
public:
    VacationDelegate(QList<EmploeeVacation *> emploeeVacationList, QObject *parent = nullptr)
        : QStyledItemDelegate(parent)
        , emploeeVacationList(emploeeVacationList){}
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
//        QVariant data = index.data();

        if ( emploeeVacationList[index.row()]->dayList[index.column()] == true )
            painter->fillRect(option.rect, emploeeVacationList[index.row()]->color);
        else return;

    };
private:
    QList<EmploeeVacation *> emploeeVacationList;
};

#endif // VACATIONMODEL_H
