#include "ChatWidget.h"
#include "ui_ChatWidget.h"

ChatWidget::ChatWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatWidget)
{
    ui->setupUi(this);
}

ChatWidget::~ChatWidget()
{
    delete ui;
}

QSplitter *ChatWidget::getSplitter()
{
    return ui->splitter;
}

void ChatWidget::setSplitter(QByteArray splitterState)
{
    ui->splitter->restoreState(splitterState);
}
