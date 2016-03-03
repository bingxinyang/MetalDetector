#ifndef CONNECTION_H
#define CONNECTION_H
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtSql>
#include <QtXml/QDomDocument>

static bool createConnection()
{
    //SqlDatabaseΪʵ�����ݿ����ӵ���
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","connection1");//����һ�����ݿ⣬������ݿ�����
    db.setDatabaseName("analyse.db");//�������ݿ������
    if(! db.open())
    {
        QMessageBox::critical(0,QObject::tr("�޷��������ݿ�"),
             QObject::tr("�޷�������Ч�����ݿ�����"),QMessageBox::Cancel);
        return false;
    }
    QSqlQuery query(db);//�½�һ����ѯ��
    query.exec("create table result (id varchar primary key,percentage varchar(10),analyse varchar(20))");
    query.exec("insert into result values('0','10%','No damage')");
    query.exec("insert into result values('1','20%','Top damage')");
    query.exec("insert into result values('2','15%','Bottom damage')");
    return true;
}

bool static createXml()
{
    QFile file("data.xml");//����һ��xml�ļ�
    if(file.exists())
        return true;
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))//��ֻд��ʽ�򿪣���������ǰ����Ϣ
        return false;
    QDomDocument doc;//�½�һ��QDomDocument�����������һ��xml�ļ�
    QDomProcessingInstruction instruction;//��Ӵ���ָ��
    instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);//xml�ļ��汾����Ϣ
    QDomElement root = doc.createElement(QString("�ж������"));
    doc.appendChild(root);//���Ӹ�Ŀ¼
    QTextStream out(&file);//ָ���ı���
    doc.save(out, 4);//��xml�ĵ����浽�ļ�data.xml�ļ��У�4��ʾ��Ԫ�������ַ���
    file.close();

    return true;
}

#endif // CONNECTION_H
