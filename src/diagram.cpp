#include "diagram.h"
#include "ui_diagram.h"
#include <QObject>
#include <QEvent>

diagram::diagram(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::diagram)
{
    ui->setupUi(this);
    setWindowTitle(QObject::tr("���η��������ʾ"));
}

diagram::~diagram()
{
    delete ui;
}


