#include "login.h"
#include "ui_login.h"

#include <QMessageBox>
#include <QSqlQuery>
#include <QDebug>
#include <QObject>

login::login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    setWindowTitle(QObject::tr("�û���½"));
    ui->passwordEdit->setFocus();
    ui->loginButton->setDefault(true);
}

login::~login()
{
    delete ui;
}

void login::on_loginButton_clicked()
{
    if(ui->passwordEdit->text().isEmpty())
    {
        QMessageBox::information(this, QObject::tr("����������"),QObject::tr("�����������ٵ�¼"), QMessageBox::Ok);
        ui->passwordEdit->setFocus();
    }
    else
    {
        QSqlQuery query;
        query.exec("select pwd from password");
        query.next();
        if(query.value(0).toString() == ui->passwordEdit->text())
        {
            QDialog::accept();//����������
        }
        else
        {
           QMessageBox::warning(this, tr("�������"), tr("��������ȷ������ٵ�¼"),QMessageBox::Ok);
           ui->passwordEdit->clear();
           ui->passwordEdit->setFocus();//��һ�����뽹��
        }
     }
}

void login::on_quitButton_clicked()
{
    QDialog::reject();//���ضԻ��򣬷��;ܾ��ź�
}
