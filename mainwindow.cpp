#include "mainwindow.h"
#include "AddUserForm.h"
#include "ui_mainwindow.h"

//Вывод и данные
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QDate>
#include <QMessageBox>
#include <QTextCharFormat>

//для работы с почтой
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>

// для стилей
#include <QStylePainter>
#include <QCalendarWidget>
#include <QHeaderView>
#include <QStyle>
#include <QPalette>
#include <QStyleFactory>
#include <QTableWidget>

#include <QContextMenuEvent>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , filePath("C:/temp/database.txt")
//    , filePath("O:/06. Данные отделов/НИО ММНМ/Рабочие данные/Собственные программы/Murzin_AV/Base/birthdays.txt")
{
    ui->setupUi(this);
    init();
    readSettings();

    ui->tableView_Vacation->setHidden(!isVacation);
    ui->cb_Vacation->setHidden(!isVacation);
    ui->calendarWidget->setHidden(isVacation || isChat);
    ui->tb_Vacation->setChecked(isVacation);
    ui->tb_Chat->setChecked(isChat);
    ui->chatWidget->setHidden(!isChat);

    on_tb_Style_clicked(isStyle);
    ui->tb_Style->setChecked(isStyle);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::dateOgonki()
{
    QDate currentDate = QDate::currentDate();

    for ( int i = 0; i < eModel->rowCount(); i++ ) {
        QModelIndex index = eModel->index(i, EmploeeModel::EColumn::Birthday);
        QString birthday = eModel->data(index, Qt::EditRole).toString();

        QStringList date = birthday.split('.');
        QDate eventDate(currentDate.year(), date[1].toInt(), date[0].toInt());

        index = eModel->index(i, EmploeeModel::EColumn::Name);
        QString name = eModel->data(index, Qt::EditRole).toString();
        index = eModel->index(i, EmploeeModel::EColumn::Famili);
        QString famili = eModel->data(index, Qt::EditRole).toString();
        index = eModel->index(i, EmploeeModel::EColumn::Otch);
        QString otch = eModel->data(index, Qt::EditRole).toString();

        dateLight(eventDate, currentDate, QString(famili+" "+ name+" "+ otch));
    }
}

void MainWindow::dateLight(const QDate &eventDate, const QDate &currentDate, const QString &toolText, QColor color, bool isAppend)
{
    Q_UNUSED(currentDate);
    QTextCharFormat textCharFormat = ui->calendarWidget->dateTextFormat(eventDate);
    textCharFormat.setToolTip( isAppend ? textCharFormat.toolTip() + toolText : toolText + " 🎂");
    textCharFormat.setForeground(color);

    ui->calendarWidget->setDateTextFormat(eventDate, textCharFormat);
}

void MainWindow::removeLineFromFile(const QString &filename, int lineIndex)
{
    QFile file(filename);
    if ( file.open(QIODevice::ReadWrite) ) {
        QTextStream stream(&file);
        QString line;
        QStringList lines;

        // Читаем все строки из файла
        while ( !stream.atEnd() ) {
            line = stream.readLine();
            lines.append(line);
        }

        // Удаляем строку с указанным индексом
        if ( lineIndex >= 0 && lineIndex < lines.size() ) {
            lines.removeAt(lineIndex);
        }

        // Перезаписываем файл с обновленными строками
        file.resize(0);
        for ( const QString &updatedLine : lines ) {
            stream << updatedLine << Qt::endl;
        }
    }
    file.close();
}

void MainWindow::handleHeaderClicked(int logicalIndex)
{

    Qt::SortOrder newSortOrder = (eModel->sortOrder == Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
    eModel->sort(logicalIndex, newSortOrder);
    eModel->layoutChanged();

}

bool MainWindow::confirmDeletion()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Подтверждение", "Вы уверены, что хотите удалить эту запись?",
                                  QMessageBox::Yes | QMessageBox::No);
    return reply == QMessageBox::Yes;
}

void MainWindow::removeRowFromModel(int row)
{
    eModel->removeRow(row);
    eModel->layoutChanged();
    ui->tableView->setCurrentIndex(QModelIndex());
    ui->tableView->update();
}

void MainWindow::removeRowFromFile(int row)
{
    QString fileName = filePath;
    QFile file(fileName);
    if ( file.open(QIODevice::ReadWrite | QIODevice::Text) ) {
        QTextStream in(&file);
        QStringList lines;
        while ( !in.atEnd() ) {
            QString line = in.readLine();
            lines.append(line);
        }
        file.resize(0); // Очищаем файл

        for ( const QString &line : lines ) {
            QStringList words = line.split('\t');
            if ( words.isEmpty() ) continue;

            int currentRow = words[0].toInt();
            if ( currentRow != row ) {
                in << line << Qt::endl;
            }
        }
        file.close();
    }
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QSettings settings("C:\\Temp\\config.cfg", QSettings::IniFormat);
    QMenu menu(this);
    for ( int col = 0; col < eModel->columnCount(); col++ ) {
        QAction *action = new QAction(eModel->headerData(col, Qt::Horizontal).toString(), this);
        action->setCheckable(true);
        if ( settings.value(eModel->headerData(col, Qt::Horizontal).toString()).toString().isEmpty() ) {
            action->setChecked(true);
            ui->tableView->setColumnHidden(col, false);
        }
        else {
            action->setChecked(settings.value(eModel->headerData(col, Qt::Horizontal).toString()).toBool());
            ui->tableView->setColumnHidden(col, !settings.value(eModel->headerData(col, Qt::Horizontal).toString()).toBool());
        }
        menu.addAction(action);

        connect(action, &QAction::toggled, this, [this, col, &settings] (bool checked) {
            settings.setValue(eModel->headerData(col, Qt::Horizontal).toString(), checked);
            ui->tableView->setColumnHidden(col, !checked);
        });
    }
    menu.exec(event->globalPos());
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);

    // Восстановите состояние сплиттера при открытии приложения
    QSettings settings("C:\\Temp\\config.cfg", QSettings::IniFormat);
    QByteArray splitterState = settings.value("splitter/geometry").toByteArray();
    if (!splitterState.isEmpty()) {
        ui->splitter->restoreState(splitterState);
    }
    splitterState = settings.value("splitter/ChatGeometry").toByteArray();
    if (!splitterState.isEmpty()) {
        ui->chatWidget->setSplitter(splitterState);
    }
    // Восстановление размеров окна
    QByteArray windowGeometry = settings.value("window/geometry").toByteArray();
    if (!windowGeometry.isEmpty()) {
        restoreGeometry(windowGeometry);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings("C:\\Temp\\config.cfg", QSettings::IniFormat);
    // Сохранение состояния сплиттера
    settings.setValue("splitter/geometry", ui->splitter->saveState());
    // Сохранение размеров окна
    settings.setValue("window/geometry", saveGeometry());
    // Сохранение состояния сплиттера у ChatWidget
    settings.setValue("splitter/ChatGeometry", ui->chatWidget->getSplitter()->saveState());

    QMainWindow::closeEvent(event);
}

void MainWindow::init()
{
    eModel = new EmploeeModel(this);
    ui->tableView->setModel(eModel);

    vModel = new VacationModel(eModel, ui->cb_Vacation, this);
    vacation(ui->cb_Vacation->currentIndex() + 1);
    QObject::connect(ui->cb_Vacation, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&] (int index) {
        vacation(index + 1);
    });
    ui->tableView_Vacation->setModel(vModel);

    connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionClicked, this, &MainWindow::handleHeaderClicked);

    dateOgonki();
    connect(ui->tableView->model(), &QAbstractTableModel::dataChanged, [&]() {
        dateOgonki();
        if ( senderSignalIndex() == EmploeeModel::EColumn::Birthday )  {
//TODO при любом изменение вызывается dataOgonki (не прально!!!!!)
        }

    });

    ui->cb_Vacation->setCurrentIndex(QDate::currentDate().month()-1);
    ui->tableView_Vacation->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    GenderDelegate *delegate = new GenderDelegate(this);
    ui->tableView->setItemDelegateForColumn(EmploeeModel::Gender, delegate);

//    VacationDelegate *vDelegate = new VacationDelegate(vModel->getEmploeeVacationList(), this);
//    ui->tableView_Vacation->setItemDelegate(vDelegate);
}

void MainWindow::readSettings()
{
    QSettings settings("C:\\Temp\\config.cfg", QSettings::IniFormat);
    for ( int col = 0; col < eModel->columnCount(); col++ ) {
        if ( !settings.value(eModel->headerData(col, Qt::Horizontal).toString()).toString().isEmpty() ) {
            ui->tableView->setColumnHidden(col, !settings.value(eModel->headerData(col, Qt::Horizontal).toString()).toBool());
        }
    }

    isStyle    = settings.value("style").toBool();
    isVacation = settings.value("isVacation").toBool();
    isChat     = settings.value("isChat").toBool();
}

void MainWindow::vacation(int month)
{
    vModel->clear();
    // Открываем файл
//    QFile file("O:/06. Данные отделов/НИО ММНМ/Рабочие данные/Собственные программы/Murzin_AV/Base/Vacation.txt");
    QFile file("C:/temp/Vacation.txt");
    if ( ! file.open(QIODevice::ReadOnly | QIODevice::Text) ) return;

    QStringList familiList;
    for ( int i = 0; i < eModel->rowCount(); i++ ) {
        familiList.push_back(eModel->data(QModelIndex(eModel->index(i, EmploeeModel::Famili))).toString() + " " +
                             eModel->data(QModelIndex(eModel->index(i, EmploeeModel::Name))).toString() + " " +
                             eModel->data(QModelIndex(eModel->index(i, EmploeeModel::Otch))).toString()
                             );
    }


    // Читаем информацию из файла
    QTextStream in(&file);
    while ( !in.atEnd() ) {
        QStringList splitLine = in.readLine().split('\t');

        //TODO первое число не коректно !!!
        int it = 0;
        for ( const QString &word : splitLine ) {
            if ( familiList.contains(word) ) {
                EmploeeVacation *emploeeVacation = new EmploeeVacation;
                emploeeVacation->fio = word;

                QDate firstDay = QDate::fromString(splitLine[it+1], "dd.MM.yyyy");
                QDate lastDay  = QDate::fromString(splitLine[it+2], "dd.MM.yyyy");

                if ( firstDay.isValid() && firstDay.month() == month ) {
                    QString days = splitLine[it+3];
                    emploeeVacation->type = splitLine[it+4];

                    int haveDays = days.toInt();
                    for (int i = 0; i <= firstDay.daysInMonth() -1; i++ ) {
                        if ( firstDay.day() > i )
                            emploeeVacation->dayList.push_back(false);
                        else {
                            if ( haveDays >= 0 ) emploeeVacation->dayList.push_back(true);
                            else                 emploeeVacation->dayList.push_back(false);

                            haveDays--;
                        }
                    }
                    vModel->addItem(emploeeVacation);
                }

                if ( (firstDay.isValid() && firstDay.month() != month) && (lastDay.isValid() && lastDay.month() == month) ) {
                    for ( int i = 0; i <= lastDay.daysInMonth() -1; i++ ) {
                        if ( lastDay.day() > i ) emploeeVacation->dayList.push_back(true);
                        else                     emploeeVacation->dayList.push_back(false);
                    }
                    vModel->addItem(emploeeVacation);
                }

                //TODO перенести покраску (реализовать выбор цвета пользовательски  из приложения)
                if (word.startsWith("Мурзин") ) {
                    emploeeVacation->color = *new QColor(255, 0, 204);
                }
                else if (word.startsWith("Яковлев" ) ) {
                    emploeeVacation->color = *new QColor(61, 208, 154);
                }
                else if (word.startsWith("Малюгин" ) ) {
                    emploeeVacation->color = *new QColor(0,0,255);
                }
                else if (word.startsWith("Жулин" ) ) {
                    emploeeVacation->color = *new QColor(30,33,61);
                }
                else if (word.startsWith("Казначеев" ) ) {
                    emploeeVacation->color = *new QColor(169,29,17);
                }
                else if (word.startsWith("Андреев" ) ) {
                    emploeeVacation->color = *new QColor(169,131,7);
                }
                else if (word.startsWith("Малышева" ) ) {
                    emploeeVacation->color = *new QColor(147,170,0);
                }
                else if (word.startsWith("Белоусова" ) ) {
                    emploeeVacation->color = *new QColor(76,187,23);
                }
                else if (word.startsWith("Кондратьев" ) ) {
                    emploeeVacation->color = *new QColor(0,255,255);
                }
                else if (word.startsWith("Рыбачук" ) ) {
                    emploeeVacation->color = *new QColor(150,186,181);
                }
                else if (word.startsWith("Коняхина" ) ) {
                    emploeeVacation->color = *new QColor(102,0,153);
                }
                else if (word.startsWith("Резанов" ) ) {
                    emploeeVacation->color = *new QColor(19, 10, 143);
                }
                else if (word.startsWith("Карпов" ) ) {
                    emploeeVacation->color = *new QColor(19, 136, 8);
                }
            }
            it++;
        }
    }

    vModel->layoutChanged();
    VacationDelegate *vDelegate = new VacationDelegate(vModel->getEmploeeVacationList(), this);
    ui->tableView_Vacation->setItemDelegate(vDelegate);
}

void MainWindow::on_pb_Add_clicked()
{
    // Создание и открытие новой формы для добавления данных
    Emploee *emploee = new Emploee;
    AddUserForm *addUserForm = new AddUserForm(*emploee);
    addUserForm->setWindowFlags(addUserForm->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    if ( addUserForm->exec() == QDialog::Accepted ) {
        // Добавление данных в модель
        eModel->addData(emploee);
        dateOgonki();
    }
}

void MainWindow::on_pb_Del_clicked()
{
    int row = ui->tableView->currentIndex().row();
    if ( row >= 0 ) {
        if ( confirmDeletion() ) {
            removeRowFromModel(row);
            removeRowFromFile(row);
        }
    }
}

void MainWindow::on_pb_Save_clicked()
{
    QFile file(filePath);
    if ( ! file.open(QIODevice::WriteOnly | QIODevice::Text) )return;

    QTextStream out(&file);

    for ( int row = 0; row < eModel->rowCount(); row++ ) {
        for ( int col = 0; col < eModel->columnCount(); col++ ) {
            QModelIndex index = eModel->index(row, col);
            out << eModel->data(index).toString() + '\t';
        }
        out << Qt::endl;
    }
    file.close();
}

void MainWindow::on_pb_Sort_clicked()
{
    if ( sortClickCount % 2 == 0) {
        eModel->sort(EmploeeModel::Birthday, Qt::AscendingOrder, true);
        ui->pb_Sort->setIcon(QIcon(":/src/resources/Icon/sort-ascending.png"));
    }
    else {
        eModel->sort(EmploeeModel::Birthday, Qt::DescendingOrder, true);
        ui->pb_Sort->setIcon(QIcon(":/src/resources/Icon/sort-descending.png"));
    }
    sortClickCount++;
    eModel->layoutChanged();
}

void MainWindow::on_pb_Send_clicked()
{
    QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedIndexes();
    QStringList emailAddresses;

    // Получаем почтовые адреса из выделенных ячеек
    for (const QModelIndex& index : selectedIndexes)
    {
        if (index.column() == EmploeeModel::Email)
        {
            QString emailAddress = eModel->data(index, Qt::DisplayRole).toString();
            emailAddresses.append(emailAddress);
        }
    }

    // Проверяем, выбрана ли хотя бы одна электронная почта
    if (emailAddresses.isEmpty())
    {
        QMessageBox::warning(this, "Предупреждение", "Не выбрана электронная почта!");
        return;
    }

    // Формируем строку с почтовыми адресами
    QString emailString = emailAddresses.join(";");

    // Проверяем, является ли стандартный почтовый клиент Outlook
    QSettings settings("HKEY_CURRENT_USER\\Software\\Clients\\Mail", QSettings::NativeFormat);
    QString defaultClient = settings.value("Default").toString();
    if ( defaultClient != "Microsoft Outlook" ) {
        QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                                  "Предупреждение",
                                                                  "Для продолжения работы нужно использовать Microsoft Outlook. "
                                                                  "Хотите сделать его стандартным почтовым клиентом?", QMessageBox::Yes|QMessageBox::No);
        if ( reply == QMessageBox::Yes ) {
            // Устанавливаем Outlook в качестве стандартного почтового клиента
            settings.setValue("Default", "Microsoft Outlook");
        } else return; // Прекращаем отправку почты
    }

    // Получаем данные о будущем имениннике
    QModelIndex index = ui->tableView->currentIndex();
    QString name = eModel->data(index.sibling(index.row(), EmploeeModel::Name), Qt::DisplayRole).toString();
    QString famili = eModel->data(index.sibling(index.row(), EmploeeModel::Famili), Qt::DisplayRole).toString();
    QString otch = eModel->data(index.sibling(index.row(), EmploeeModel::Otch), Qt::DisplayRole).toString();
    QString birthday = eModel->data(index.sibling(index.row(), EmploeeModel::Birthday), Qt::DisplayRole).toString();

    // Формируем тему письма
    QString subject = " &Subject=День рождения у «" + famili + " " + name + " " + otch + "»";

    // Формируем текст письма
    QDate currentDate = QDate::currentDate();
    QStringList dateParts = birthday.split('.');
    //    QDate eventDate2 = QDate::fromString(birthday, "dd.MM.yyyy");
    QDate eventDate(currentDate.year(), dateParts[1].toInt(), dateParts[0].toInt());
    int age = currentDate.year() - QDate::fromString(birthday, "dd.MM.yyyy").year();

    QString body = " &Body=Через " + QString::number(currentDate.daysTo(eventDate)) +
            " дней у «" + famili + " " + name + " " + otch +
            "» будет день рождения, ему исполнится " + QString::number(age) + " 🎂.";

    //    // Открываем почтовый клиент с предварительно заполненными адресами электронной почты
    QDesktopServices::openUrl(QUrl("mailto:" + emailString+ subject +body));
}



void MainWindow::on_tb_Style_clicked(bool checked)
{
    QSettings settings("C:\\Temp\\config.cfg", QSettings::IniFormat);
    settings.setValue("style", checked);

    QApplication::setStyle("Fusion");
    ui->calendarWidget->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    QString style = " ";
    QString styleCalend = " ";
    QPalette palette = QApplication::style()->standardPalette();

    if ( !checked ) {
        // Настройка темы для тёмного стиля
        palette.setColor(QPalette::Window, QColor(53, 53, 53));
        palette.setColor(QPalette::WindowText, Qt::white);
        palette.setColor(QPalette::Base, QColor(25, 25, 25));
        palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
        palette.setColor(QPalette::ToolTipBase, Qt::white);
        palette.setColor(QPalette::ToolTipText, Qt::white);
        palette.setColor(QPalette::Text, Qt::white);
        palette.setColor(QPalette::Button, QColor(53, 53, 53));
        palette.setColor(QPalette::ButtonText, Qt::white);
        palette.setColor(QPalette::BrightText, Qt::red);
        palette.setColor(QPalette::Link, QColor(42, 130, 218));
        //        QApplication::setPalette(palette);

        QFile styleFile(":/src/resources/Styles/calendar_style.qss");
        if ( styleFile.open(QIODevice::ReadOnly | QIODevice::Text) ) {
            styleCalend = styleFile.readAll();
            styleFile.close();
            //            ui->calendarWidget->setStyleSheet(style);
        }

        style = "QTableView:!active {"
                "  selection-background-color: #191919;"
                "  selection-color: white;"
                "}"
                "QTableView:active {"
                "  selection-background-color: #353535;"
                "  selection-color: white;"
                "}";

        //        ui->tableView->setStyleSheet(style);
        ui->tb_Style->setIcon(QIcon(":/src/resources/Icon/sun.png"));
    }
    else {
        ui->tb_Style->setIcon(QIcon(":/src/resources/Icon/sunBlack.png"));
    }

    QApplication::setPalette(palette);
    ui->calendarWidget->setStyleSheet(styleCalend);
    ui->tableView->setStyleSheet(style);
    style = "text-align:left; padding-left: 3px; padding-right: 3px";
    ui->pb_Add->setStyleSheet(style);
    ui->pb_Del->setStyleSheet(style);
    ui->pb_Save->setStyleSheet(style);
    ui->pb_Sort->setStyleSheet(style);
    ui->pb_Send->setStyleSheet(style);

}

void MainWindow::on_tb_Vacation_clicked(bool checked)
{
    ui->calendarWidget->setHidden(checked);
    ui->tableView_Vacation->setHidden(!checked);
    ui->cb_Vacation->setHidden(!checked);
    ui->chatWidget->setHidden(true);
    ui->tb_Chat->setChecked(false);

    QSettings settings("C:\\Temp\\config.cfg", QSettings::IniFormat);
    settings.setValue("isVacation", checked);
    settings.setValue("isChat", false);

}


void MainWindow::on_tb_Chat_clicked(bool checked)
{
    ui->calendarWidget->setHidden(checked);
    ui->tableView_Vacation->setHidden(true);
    ui->cb_Vacation->setHidden(true);
    ui->chatWidget->setHidden(!checked);
    ui->tb_Vacation->setChecked(false);

    isVacation = false;

    QSettings settings("C:\\Temp\\config.cfg", QSettings::IniFormat);
    settings.setValue("isChat", checked);
    settings.setValue("isVacation", false);
}


















//TODO
/*
void MainWindow::on_pb_Send_Shadow_clicked()
{
    QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedIndexes();
    QStringList emailAddresses;

    // Получаем почтовые адреса из выделенных ячеек
    for (const QModelIndex& index : selectedIndexes)
    {
        if (index.column() == EmploeeModel::Email)
        {
            QString emailAddress = eModel->data(index, Qt::DisplayRole).toString();
            emailAddresses.append(emailAddress);
        }
    }

    // Проверяем, выбрана ли хотя бы одна электронная почта
    if (emailAddresses.isEmpty())
    {
        QMessageBox::warning(this, "Предупреждение", "Не выбрана электронная почта!");
        return;
    }

    // Формируем строку с почтовыми адресами
    QString emailString = emailAddresses.join(";");

    // Формируем текст письма
    QString body = "Текст Вашего сообщения";
    QString subject = "Тема Вашего сообщения";

    // Создаем задачу для отправки почты
    SendEmailTask* task = new SendEmailTask(emailString, subject, body);

    // Отправляем задачу в пул потоков
    QThreadPool::globalInstance()->start(task);
}

//               рабочий метод!!!!
//void MainWindow::on_pb_Send_Shadow_clicked()
//{
//    QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedIndexes();
//    QStringList emailAddresses;

//    // Получаем почтовые адреса из выделенных ячеек
//    for (const QModelIndex& index : selectedIndexes)
//    {
//        if (index.column() == EmploeeModel::Email)
//        {
//            QString emailAddress = eModel->data(index, Qt::DisplayRole).toString();
//            emailAddresses.append(emailAddress);
//        }
//    }

//    // Проверяем, выбрана ли хотя бы одна электронная почта
//    if (emailAddresses.isEmpty())
//    {
//        QMessageBox::warning(this, "Предупреждение", "Не выбрана электронная почта!");
//        return;
//    }

//    // Формируем строку с почтовыми адресами
//    QString emailString = emailAddresses.join(";");

//    // Формируем текст письма
//    QString body = "Текст вашего сообщения";
//    QString subject = "Тема вашего сообщения";

//    // Открыть почтовый клиент по умолчанию и заполнить поля
//    QDesktopServices::openUrl(QUrl("mailto:" + emailString + "?subject=" + subject + "&body=" + body));
//}

*/



