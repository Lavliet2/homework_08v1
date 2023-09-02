#ifndef ADDUSERFORM_H
#define ADDUSERFORM_H

#include <QDialog>
#include "QDate"

struct Emploee;

namespace Ui {
class AddUserForm;
}

class AddUserForm : public QDialog
{
    Q_OBJECT

public:
    explicit AddUserForm(Emploee &emploee, QWidget *parent = nullptr);
    virtual void done(int result)override;
    ~AddUserForm();

private slots:
    void on_tb_getIp_clicked();

private:

    Ui::AddUserForm *ui;
    Emploee &emploee;

};

#endif // ADDUSERFORM_H
