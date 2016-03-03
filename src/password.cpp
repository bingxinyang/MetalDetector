#include "password.h"
#include "ui_password.h"
#include "connection.h"

password::password(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::password)
{
    ui->setupUi(this);
    setWindowTitle(QObject::tr("�޸ĵ�¼����"));
}

password::~password()
{
    delete ui;
}

void password::on_okButton_clicked()
{
    QSqlQuery query;
    query.exec("select pwd from password");
    query.next();
    if(ui->oldPasswordLineEdit->text().isEmpty())
        QMessageBox::warning(this, QObject::tr("����������"), QObject::tr("������������������"), QMessageBox::Ok);
    else if(ui->oldPasswordLineEdit->text() != query.value(0).toString())
    {
        QMessageBox::warning(this, QObject::tr("�������"), QObject::tr("����ľ��������"), QMessageBox::Ok);
        ui->oldPasswordLineEdit->setFocus();
    }
    else
    {
        if(ui->newPasswordLineEdit->text() == ui->surePasswordlineEdit->text())
        {
            QString newPassword = ui->newPasswordLineEdit->text();
            query.exec(QString("update password set pwd=%1").arg(newPassword));//��������
            QMessageBox::information(this, QObject::tr("�޸�����"), QObject::tr("�޸�����ɹ���"), QMessageBox::Ok);
        }
        else
            QMessageBox::warning(this, QObject::tr("�޸�����ʧ��"), QObject::tr("�������������벻һ�£�"), QMessageBox::Ok);
    }
}

void password::on_cancelButton_clicked()
{
    ui->oldPasswordLineEdit->clear();
        ui->oldPasswordLineEdit->setFocus();//�����뽹����ھ����봦��������������
        ui->newPasswordLineEdit->clear();
        ui->surePasswordlineEdit->clear();
}
