#include "VacationModel.h"


#include <QDebug>
#include <QFileInfo>
#include <QTextStream>
#include <QDate>

VacationModel::VacationModel(EmploeeModel *eModel, QComboBox *cb, QObject *parent)
    : QAbstractTableModel(parent)
    , eModel(eModel)
    , cb(cb) {}

QVariant VacationModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ( orientation == Qt::Horizontal && role == Qt::BackgroundRole ) {
        QDate date = QDate(QDate::currentDate().year(), cb->currentIndex() + 1, section + 1);
        qDebug() << date << date.weekNumber() << date.dayOfWeek();
        if ( date.dayOfWeek() == 6 || date.dayOfWeek() == 7)
            return QVariant(QColor(165, 38, 10));
    }

    if ( role != Qt::DisplayRole )
        return QVariant();

    if ( orientation == Qt::Horizontal && section >= 0 /*&& section < headers.size()*/ ) {
        return section+1;
    }


    if ( orientation == Qt::Vertical && section >= 0 ) {
            QStringList fioIni = emploeeVacationList[section]->fio.split(" ");
            QString fio = fioIni[0] + " " + fioIni[1][0] + ". " + fioIni[2][0] + ".";

            return fio;
    }

    return QVariant();
}

QVariant VacationModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DecorationRole )
    {
        if ( emploeeVacationList[index.row()]->dayList[index.column()] == true )
            return emploeeVacationList[index.row()]->color;

    }

    return QVariant();
}

int VacationModel::rowCount(const QModelIndex &parent) const
{
    return emploeeVacationList.count();
}

int VacationModel::columnCount(const QModelIndex &parent) const
{
    return emploeeVacationList[0]->dayList.count();
}

void VacationModel::addItem(EmploeeVacation *ev)
{
    beginInsertRows(QModelIndex(), emploeeVacationList.size(), emploeeVacationList.size());
    emploeeVacationList.append(ev);
    endInsertRows();
}

void VacationModel::clear()
{
    emploeeVacationList.clear();
}
