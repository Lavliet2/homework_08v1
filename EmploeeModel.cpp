#include "EmploeeModel.h"
#include "ProcessHandler.hpp"

#include <QDebug>
#include <QFileInfo>
#include <QTextStream>
#include <QMessageBox>
#include <QCalendarWidget>
#include <QTextCharFormat>

#include <QThread>
#include <QProcess>
#include <QMutex>
#include <QObject>

#include <QDate>
#include <QStringList>
#include <QTimer>




//class ProcessHandler;
///######################################################################
/// @class EmploeeModel модель сотрудников отдела
///######################################################################
EmploeeModel::EmploeeModel(QObject *parent)
    : QAbstractTableModel(parent)
    , filePath("C:/temp/database.txt")
//    , filePath("O:/06. Данные отделов/НИО ММНМ/Рабочие данные/Собственные программы/Murzin_AV/Base/birthdays.txt")

{
    // Инициализация заголовков столбцов
    headers << "Фамилия" << "Имя" << "Отчество" << " Должность " << " Дата рождения " << "Пол" << "Телефон" << "Почта" << "IP" ;
    fillDataList();

    startProcessing();
    timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, this, &EmploeeModel::startProcessing);
    timer->start(600000);// таймер обновления IP

}


int EmploeeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return emploeeList.count();
}


int EmploeeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return headers.size();
}


QVariant EmploeeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {

        switch (index.column())
        {
        case EColumn::Famili:
            return emploeeList[index.row()]->famili;
        case EColumn::Name:
            return emploeeList[index.row()]->name;
        case EColumn::Otch:
            return emploeeList[index.row()]->otch;
        case EColumn::Post:
            return emploeeList[index.row()]->post;
        case EColumn::Birthday:
        {
            // Преобразование строки в QDate
            QDate birthDate = QDate::fromString(emploeeList[index.row()]->birthday, "dd.MM.yyyy");
            if ( birthDate.isValid() ) return birthDate.toString("dd.MM.yyyy"); // Форматирование QDate обратно в строку для отображения
            else                       return QVariant();                       // Некорректная дата
        }

        case EColumn::Email:
            return emploeeList[index.row()]->email;
        case EColumn::Phone:
            return emploeeList[index.row()]->phone;
        case EColumn::Gender:
            return emploeeList[index.row()]->gender;
        case EColumn::IP:
            return emploeeList[index.row()]->ip;

        }
    }
    if ( role == Qt::ToolTipRole && index.column() == Birthday ) {
        QDate birthDate = QDate::fromString(emploeeList[index.row()]->birthday, "dd.MM.yyyy");
        if ( birthDate.isValid() ) {
            int age = calculateAge(birthDate);
            return "Возраст: " + QString::number(age);
        }
        else return "Invalid birth date";
    }
    return QVariant();
}


QVariant EmploeeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ( orientation == Qt::Vertical && role == Qt::BackgroundRole ) {


        if ( emploeeList[section]->ping ) return QVariant(QColor(0, 255, 0));
        else                    return QVariant(QColor(255, 0, 0));

    }

    if ( role != Qt::DisplayRole )
        return QVariant();

    if ( orientation == Qt::Horizontal && section >= 0 && section < headers.size() ) {
        return headers[section];
    }
    return QVariant();
}


bool EmploeeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if ( index.isValid() && role == Qt::EditRole ) {
        switch (index.column())
        {
        case EColumn::Famili:
            emploeeList[index.row()]->famili  = value.toString().trimmed();
            break;
        case EColumn::Name:
            emploeeList[index.row()]->name = value.toString().trimmed();
            break;
        case EColumn::Otch:
            emploeeList[index.row()]->otch  = value.toString().trimmed();
            break;
        case EColumn::Post:
            emploeeList[index.row()]->post  = value.toString().trimmed();
            break;
        case EColumn::Birthday:
        {
            QString birthday = value.toString();

            QDate date = QDate::fromString(birthday, "dd.MM.yyyy");
            if (date.isValid()) {
                emploeeList[index.row()]->birthday = birthday;
            } else {
                QMessageBox::warning(nullptr, "Предупреждение", "Поле «ДЕНЬ РОЖДЕНИЯ» заполнено некорректно");
                return false;
            }
        }
            break;
        case EColumn::Email:
        {
            QString email = value.toString();
            if ( email.contains("@") && email.contains(".") ) {
                emploeeList[index.row()]->email = email;
            } else {
                QMessageBox::warning(nullptr, "Предупреждение", "Поле «Почта» заполнено некорректно, отсутсвуют специальные символы «@»«.»");
                return false;
            }
        }
            break;
        case EColumn::Phone:
            emploeeList[index.row()]->phone = value.toString().trimmed();
            break;
        case EColumn::Gender:
            emploeeList[index.row()]->gender = value.toString().trimmed();
            break;
        case EColumn::IP:
            emploeeList[index.row()]->ip = value.toString().trimmed();
            break;
        }

        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }

    return false;
}


Qt::ItemFlags EmploeeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}


bool EmploeeModel::removeRow(int row, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    emploeeList.removeAt(row);

    return true;
}

void EmploeeModel::addData(Emploee *emploee)
{
    beginInsertRows(QModelIndex(), emploeeList.size(), emploeeList.size());
    emploeeList.append(emploee);
    endInsertRows();
}

void EmploeeModel::sort(int column, Qt::SortOrder order, bool sortBy)
{
    sortOrder = order;
    sortColumn = column;
    sortData(sortBy);
}

void EmploeeModel::ping()
{

}

void EmploeeModel::startProcessing()
{
    qDebug() << "startProcessing;";
    for ( int i = 0; i< emploeeList.count(); i++ ) {
        Emploee *emploee = emploeeList.at(i);
        ProcessHandler *handler = new ProcessHandler(emploee);
        handler->index = i;

        thread = new QThread;
        handler->moveToThread(thread);

        QObject::connect(thread,  &QThread::started, handler, &ProcessHandler::startProcess);
        QObject::connect(handler, &ProcessHandler::processFinished, this, &EmploeeModel::handeProcessResult);
        QObject::connect(thread,  &QThread::finished, thread, &QThread::deleteLater);

        thread->start();
    }

}

void EmploeeModel::processResultReady(int index, QString result){}
void EmploeeModel::handeProcessResult(int index, QString result)
{
    emit processResultReady(index, result);
    layoutChanged();
}


void EmploeeModel::fillDataList()
{
    // Открываем файл
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Не удалось открыть файл";
        return;
    }
    // Читаем информацию из файла
    QTextStream in(&file);
    while (!in.atEnd())
    {
        QStringList splitLine = in.readLine().split('\t');

        Emploee *empl = new Emploee;

        empl->famili   = splitLine[Famili];
        empl->name     = splitLine[Name];
        empl->otch     = splitLine[Otch];
        empl->post     = splitLine[Post];
        empl->birthday = splitLine[Birthday];
        empl->email    = splitLine[Email];
        empl->phone    = splitLine[Phone];
        empl->gender   = splitLine[Gender];
        empl->ip       = splitLine[IP];


        emploeeList.append(empl);
    }
    // Закрываем файл
    file.close();
}


int EmploeeModel::daysUntilBirthday(const QDate &date)
{
    QDate currentDate = QDate::currentDate();
    QDate nextBirthday(currentDate.year(), date.month(), date.day());

    if (nextBirthday < currentDate)
        nextBirthday = nextBirthday.addYears(1);

    return currentDate.daysTo(nextBirthday);
}

void EmploeeModel::sortData(bool sortBy)
{
    if ( sortBy ) {
        std::sort(emploeeList.begin(), emploeeList.end(), [this](const Emploee *a, const Emploee *b) {
            QDate dateA = QDate::fromString(a->birthday, "dd.MM.yyyy");
            QDate dateB = QDate::fromString(b->birthday, "dd.MM.yyyy");

            if (!dateA.isValid() || !dateB.isValid())
                return false;

            int daysUntilBirthdayA = daysUntilBirthday(dateA);
            int daysUntilBirthdayB = daysUntilBirthday(dateB);

            if (sortOrder == Qt::AscendingOrder)
                return daysUntilBirthdayA < daysUntilBirthdayB;
            else
                return daysUntilBirthdayA > daysUntilBirthdayB;
        });
    }
    else {
        switch ( sortColumn )
        {
        case Famili: case Name: case Otch: case Post: case Email: case Phone: case Gender: case IP:
            std::sort(emploeeList.begin(), emploeeList.end(), [this](const Emploee *a, const Emploee *b) {
                //            return compareNames(a, b);
                if (sortOrder == Qt::AscendingOrder) {
                    if      ( sortColumn == Famili)
                        return a->famili < b->famili;
                    else if ( sortColumn == Name)
                        return a->name < b->name;
                    else if ( sortColumn == Otch)
                        return a->otch < b->otch;
                    else if ( sortColumn == Post)
                        return a->post < b->post;
                    else if ( sortColumn == Email)
                        return a->email < b->email;

                    else if ( sortColumn == Phone)
                        return a->phone < b->phone;
                    else if ( sortColumn == Gender)
                        return a->gender < b->gender;
                    else if ( sortColumn == IP)                        
                        return a->ping < b->ping;

                }
                else
                    if      ( sortColumn == Famili)
                        return a->famili > b->famili;
                    else if ( sortColumn == Name)
                        return a->name > b->name;
                    else if ( sortColumn == Otch)
                        return a->otch > b->otch;
                    else if ( sortColumn == Post)
                        return a->post > b->post;
                    else if ( sortColumn == Email)
                        return a->email > b->email;

                    else if ( sortColumn == Phone)
                        return a->phone > b->phone;
                    else if ( sortColumn == Gender)
                        return a->gender > b->gender;
                    else if ( sortColumn == IP)
                        return a->ping > b->ping;

                return  a->famili > b->famili; //хз
            });

            break;
        case Birthday:
        {
            std::sort(emploeeList.begin(), emploeeList.end(), [this](const Emploee *a, const Emploee *b) {
                QDate dateA = QDate::fromString(a->birthday, "dd.MM.yyyy");
                QDate dateB = QDate::fromString(b->birthday, "dd.MM.yyyy");

                if (!dateA.isValid() || !dateB.isValid())
                    return false;

                if (sortOrder == Qt::AscendingOrder)
                    return dateA < dateB;
                else
                    return dateA > dateB;
            });

        }
        default:
            break;
        }
    }

    emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
}

int EmploeeModel::calculateAge(const QDate &birthDate) const
{
    QDate currentDate = QDate::currentDate();
    int age = currentDate.year() - birthDate.year();

    if (birthDate.month() > currentDate.month() ||
            (birthDate.month() == currentDate.month() && birthDate.day() > currentDate.day())) {
        age--;
    }

    return age;
}

