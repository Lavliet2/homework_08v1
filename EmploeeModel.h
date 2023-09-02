#ifndef EMPLOEEMODEL_H
#define EMPLOEEMODEL_H

#include <QAbstractTableModel>
#include <QComboBox>
#include <QStyledItemDelegate>




#include <QThread>
#include <QProcess>
#include <QMutex>
#include <QObject>

//class ProcessHandler;



///######################################################################
/// @class GenderDelegate для QComboBox для колонки "Gender"
///######################################################################
class GenderDelegate : public QStyledItemDelegate
{
public:
    GenderDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent) {}

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        Q_UNUSED(option); Q_UNUSED(index);
        QComboBox *editor = new QComboBox(parent);
        editor->addItem("М");
        editor->addItem("Ж");
        return editor;
    }
};


struct Emploee
{
    QString famili;
    QString name;
    QString otch;
    QString post;
    QString birthday;
    QString email;
    QString phone;
    QString gender;
    QString ip;
    bool ping = false;
};

///######################################################################
/// @class EmploeeModel модель сотрудников отдела
///######################################################################
class EmploeeModel : public QAbstractTableModel
{

public:
    enum EColumn {
        Famili,
        Name,
        Otch,
        Post,
        Birthday,
        Gender,
        Phone,
        Email,
        IP
    };

    EmploeeModel(QObject *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool     setData(const QModelIndex &index, const QVariant &value, int role) override;
    void     addData(Emploee *emploee);

    int  rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int  columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    void sort(int column, Qt::SortOrder order, bool sortBy = false);//override;

    void ping();

    void startProcessing();

//    QList<Employee *> getEmploeeList();
signals:
    void processResultReady(int index, QString result);
//    void processFinished();
private slots:
    void handeProcessResult(int index, QString result);
//    void startProcess();
//    void upDatePing();

private:
    void fillDataList();
    void sortData(bool sortBy = false);

    int calculateAge(const QDate &birthDate) const;
    static int daysUntilBirthday(const QDate &date);

public:
    Qt::SortOrder sortOrder = Qt::AscendingOrder;        

private:
    QString filePath;
    QStringList dataList;

    QList<Emploee *> emploeeList;
    QStringList headers; // Заголовки столбцов
    QThread *thread;

    int sortColumn = -1;


    QTimer *timer;


};


#endif // EMPLOEEMODEL_H
