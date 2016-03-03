#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"
#include "fft.h"
#include "login.h"
#include "password.h"

#include <QtCore>
#include <QFileDialog>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <QTextCodec>
#include <QPainter>
#include <QPaintDevice>
#include <QMouseEvent>
#include <QProcess>
#include <QProgressDialog>
#include <QMap>
#include <QTextStream>
#include <QDataStream>
#include <qmath.h>

//���ݿ����
#include <QSqlDatabase>
#include <QDebug>
#include <QStringList>
#include <QVariant>

/*-----ȫ�ֱ����Ķ�����-----*/
double global_x[SumPot],global_y[SumPot];//ʵ�ʴ��ݸ���ͼ�����ı���������ͼ���ƣ�
double global_x1[SumPot],global_y1[SumPot];//Ƶ�����߻��������ݴ���
//�й�FFT�ı�������
double show1[SumPot]={0};   //ʵ������
double show2[SumPot]={0};   //�鲿����
double show3[SumPot]={0};   //���������ʾʹ�õ�����

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    srand(QDateTime::currentDateTime().toTime_t());
    ui->setupUi(this);

    QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
    setWindowTitle(QObject::tr("��������̽���Ƿ������"));
    resize(1000,500);
    //����������Ϊ������С��Ҫ���û�ͼ����������ͬʱ������С��ֻ��Ҫ������ѡ����Ϊˮƽ��ֱ���ּ���

    //�ź�-�۵�����
    QObject::connect(ui->action_Qt,SIGNAL(triggered()),qApp,SLOT(aboutQt()));
    QObject::connect(ui->action_Website,SIGNAL(triggered()),this,SLOT(aboutWebsiteSlot()));
    QObject::connect(ui->action_about,SIGNAL(triggered()),this,SLOT(aboutSoftwareSlot()));

    ui->customPlot->clearGraphs();
    //ʹ�õ���������л�ͼ
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                    QCP::iSelectLegend | QCP::iSelectPlottables);

//    ui->customPlot->xAxis->setRange(0, SumPot);
//    ui->customPlot->yAxis->setRange(-1, 1);//x��y��ʾ����Χ
//    ui->customPlot->axisRect()->setupFullAxesBox();

//    ui->customPlot->plotLayout()->insertRow(0);
//    ui->customPlot->plotLayout()->addElement(0, 0, new QCPPlotTitle(ui->customPlot, QObject::tr("Ƶ��������ʾ")));

//    ui->customPlot->xAxis->setLabel(QObject::tr("x ��"));
//    ui->customPlot->yAxis->setLabel(QObject::tr("y ��"));
//    ui->customPlot->legend->setVisible(true);

    QFont legendFont = font();
    legendFont.setPointSize(10);//���ϽǱ�ʶ�������ֵĴ�С
    ui->customPlot->legend->setFont(legendFont);
    ui->customPlot->legend->setSelectedFont(legendFont);
    ui->customPlot->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items

    ui->customPlot->plotLayout()->insertRow(0);
    ui->customPlot->plotLayout()->addElement(0, 0, new QCPPlotTitle(ui->customPlot, QObject::tr("Ƶ��������ʾ")));
    //addRandomGraph();
    //addRandomGraph();
    //addRandomGraph();
    //addRandomGraph();
    //��������ʵ����Ҫȷ����ʾ��������

    // connect slot that ties some axis selections together (especially opposite axes):
    connect(ui->customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
    connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(ui->customPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

    // make bottom and left axes transfer their ranges to top and right axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

    // connect some interaction slots:
    connect(ui->customPlot, SIGNAL(titleDoubleClick(QMouseEvent*,QCPPlotTitle*)), this, SLOT(titleDoubleClick(QMouseEvent*,QCPPlotTitle*)));
    connect(ui->customPlot, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)), this, SLOT(axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));
    connect(ui->customPlot, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));

    // connect slot that shows a message in the status bar when a graph is clicked:
    connect(ui->customPlot, SIGNAL(plottableClick(QCPAbstractPlottable*,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*)));

    // setup policy and connect slot for context menu popup:
    ui->customPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->customPlot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));

    int plot_width = ui->customPlot->width();
    int plot_heigh = ui->customPlot->height();//�����ͼ����Ŀ�Ⱥ͸߶�
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_O_triggered()
{
    //�ļ�ѡ��Ի���
    QString fileName = QFileDialog::getOpenFileName(this,QObject::tr("�ļ�ѡ��Ի���"),
                                                          "F:",QObject::tr("�����ļ�(*.wav)"));
    //�ж��ļ��Ƿ�ѡ�񣬲�������ʾ��Ϣ�Ի���
    if(fileName.length()==0)//����ʹ��fileName.isEmpty()
    {
        QMessageBox::information(NULL,QObject::tr("�ļ�ѡ��"),
                                 QObject::tr("��ѡ�����ļ���"));
    }
    else
    {
        QMessageBox::information(NULL,QObject::tr("�ļ�ѡ��"),
                                 QObject::tr("����ѡ�����ļ���")+fileName);
    }
    readFile(fileName);
}

bool MainWindow::readFile(QString fileName)
{
    //��ȡ��ȡ���ļ�
    QFile file(fileName);
    wav_struct WAV;
    if(!(file.open(QIODevice::ReadOnly)))
    {
        //QMessageBox::information(this,QObject::tr("���ļ�ʧ�ܣ�"),file.errorString());
        return false;
    }

    QByteArray content = file.readAll();
    WAV.data_size = content.size() - 44;
    WAV.data = new unsigned char[WAV.data_size];
    //qDebug()<<"\n WAV.data:"<<WAV.data;

    file.seek(0x2c);
    file.read((char *)WAV.data, sizeof(char)*WAV.data_size);
    //qDebug()<<"WAV.data_size:"<<WAV.data_size;

    for(int i=0;i<WAV.data_size;i+=2)
    //for(int i=341;i<3414;i++)
    {
        //�ұ�Ϊ���
        unsigned long data_low = WAV.data[i];
        //qDebug()<<"\n data_low:"<<data_low;
        unsigned long data_high = WAV.data[i + 1];
        double data_true = data_high * 256 + data_low;
        //qDebug()<<"\n data_true:"<<data_true;

        long data_complement = 0;
        //ȡ��˵����λ������λ��
        int my_sign = (int)(data_high / 128);
        //printf("%d ", my_sign);
        if(my_sign == 1)
        {
            data_complement = data_true - 65536;
            //ʮ����65536ת��Ϊʮ��������ֵΪ10000����������þ���ȥ������λ
        }
        else
        {
            data_complement = data_true;
        }
        //printf("%d ", data_complement);
        //setprecision(4);
        double float_data = (double)(data_complement/(double)32768);
        //������16bit��ʾһ����������˷�ĸΪ2^16=32768
        //qDebug()<<"float_data"<<float_data*10;
        global_y[i] = float_data;
        show1[i] = float_data;

        //bool equal = global_y[i]==show1[i];
        //qDebug()<<equal;
        //qDebug()<<"\n global_y:"<<global_y*10;
        //addRandomGraph();
    }
    //qDebug()<<"\n global_y:"<<global_y*10;
    if(file.isOpen())
        file.close();

    file.close();
    addWaveGraph();
}

int MainWindow::QByteArrayToInt(QByteArray bytes) {
    int addr = bytes[0] & 0x000000FF;
    addr |= ((bytes[1] << 8) & 0x0000FF00);
    addr |= ((bytes[2] << 16) & 0x00FF0000);
    addr |= ((bytes[3] << 24) & 0xFF000000);
    return addr;
}

void MainWindow::on_action_A_triggered()
{

}

void MainWindow::on_action_P_triggered()
{

}

void MainWindow::on_action_Q_triggered()
{
//    qDebug()<<"quit";
}

void MainWindow::closeEvent(QCloseEvent *event)
{
//    QMessageBox msgBox;
//    msgBox.question(this,QObject::tr("��ʾ"),QObject::tr("ȷ���˳�������?"),QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes);
//    //����жϴ������
}

//�ڶ����˵����ۺ���
void MainWindow::on_action_zoom_in_triggered()
{

}

void MainWindow::on_action_zoom_out_triggered()
{

}

void MainWindow::on_action_rotate_triggered()
{

}

void MainWindow::on_action_clear_triggered()
{

}

//�������˵����ۺ���
void MainWindow::on_action_2D_triggered()
{

}

void MainWindow::on_action_3D_triggered()
{

}

void MainWindow::on_action_spec_triggered()
{
    ui->customPlot->clearGraphs();

    ui->customPlot->xAxis->setRange(1, 10);
    ui->customPlot->yAxis->setRange(0, 1);//x��y��ʾ����Χ
    ui->customPlot->axisRect()->setupFullAxesBox();

    ui->customPlot->xAxis->setLabel(QObject::tr("Ƶ��"));
    ui->customPlot->yAxis->setLabel(QObject::tr("��һ��������"));
    ui->customPlot->legend->setVisible(true);

    double show2[SumPot]={0};//�鲿����
    QVector<double> x(SumPot), y(SumPot);

    //FFT(show1,show2,n,sign);
    FFT(show1,show2,SumPot,1);

    //-----���㹦��������-----//
    double m=0,n=0;
    double max=1;

    for(int i=341;i<3414;i++)
    //ֻ����1K��10KƵ��;16384*(1/48~10/48)
    {
        m=(show1[i]/16384)*(show1[i]/16384);
        n=(show2[i]/16384)*(show2[i]/16384);

        //qDebug()<<"show2:"<<show2[i];

        show3[i-341]=(m+n)/16384;
        //sqrt(m+n)/;

        //���Խ��յ������ݣ�����������Ƶ��ͼ��ȡ�����ݣ�
        qDebug()<<"show[3]:"<<show3[i-341];
        global_x1[i] = i;
        global_y1[i] = show3[i-341];
        /*-----���Խ������ʾ����δԭ���Σ�δ��FFT�任-----*/

        //x[i] = global_x1[i];
        //y[i] = global_y1[i];

        if(show3[i-341]>max)
        {
            max=show3[i-341];
        }
        //�õ����ֵ
    }
    addSpecGraph();

}

void MainWindow::on_action_wave_triggered()
{

}

void MainWindow::paintEvent(QPaintEvent *event)
{

}

//���ĸ��˵����ۺ���
void MainWindow::aboutSoftwareSlot()
{
    about *dialog = new about;
    dialog->show();//unmodal dialog,����ģ̬�Ի���
    //dialog->exec();//modal dialog,ģ̬�Ի���
}

void MainWindow::aboutWebsiteSlot()
{
    QDesktopServices::openUrl(QUrl("http://www.qt.io/"));
}

//���������ͼ����
void MainWindow::titleDoubleClick(QMouseEvent* event, QCPPlotTitle* title)
{
    Q_UNUSED(event)
    // Set the plot title by double clicking on it
    bool ok;
    QString newTitle = QInputDialog::getText(this, QObject::tr("���ı���"), QObject::tr("�±��⣺"), QLineEdit::Normal, title->text(), &ok);
    if (ok)
    {
      title->setText(newTitle);
      ui->customPlot->replot();
  }
}

void MainWindow::axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part)
{
  // Set an axis label by double clicking on it
  if (part == QCPAxis::spAxisLabel) // only react when the actual axis label is clicked, not tick label or axis backbone
  {
    bool ok;
    QString newLabel = QInputDialog::getText(this, QObject::tr("�޸�������"), QObject::tr("����������:"), QLineEdit::Normal, axis->label(), &ok);
    if (ok)
    {
      axis->setLabel(newLabel);
      ui->customPlot->replot();
    }
  }
}

void MainWindow::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item)
{
  // Rename a graph by double clicking on its legend item
  Q_UNUSED(legend)
  if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
  {
    QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
    bool ok;
    QString newName = QInputDialog::getText(this, QObject::tr("�޸���������"), QObject::tr("����������:"), QLineEdit::Normal, plItem->plottable()->name(), &ok);
    if (ok)
    {
      plItem->plottable()->setName(newName);
      ui->customPlot->replot();
    }
  }
}

void MainWindow::selectionChanged()
{
  /*
   normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
   the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
   and the axis base line together. However, the axis label shall be selectable individually.

   The selection state of the left and right axes shall be synchronized as well as the state of the
   bottom and top axes.

   Further, we want to synchronize the selection of the graphs with the selection state of the respective
   legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
   or on its legend item.
  */

  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    ui->customPlot->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    ui->customPlot->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    ui->customPlot->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    ui->customPlot->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }

  // synchronize selection of graphs with selection of corresponding legend items:
  for (int i=0; i<ui->customPlot->graphCount(); ++i)
  {
    QCPGraph *graph = ui->customPlot->graph(i);
    QCPPlottableLegendItem *item = ui->customPlot->legend->itemWithPlottable(graph);
    if (item->selected() || graph->selected())
    {
      item->setSelected(true);
      graph->setSelected(true);
    }
  }
}

void MainWindow::mousePress()
{
  // if an axis is selected, only allow the direction of that axis to be dragged
  // if no axis is selected, both directions may be dragged

  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->xAxis->orientation());
  else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->yAxis->orientation());
  else
    ui->customPlot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void MainWindow::mouseWheel()
{
  // if an axis is selected, only allow the direction of that axis to be zoomed
  // if no axis is selected, both directions may be zoomed

  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->xAxis->orientation());
  else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->yAxis->orientation());
  else
    ui->customPlot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void MainWindow::addWaveGraph()
{
    ui->customPlot->clearGraphs();
//    if(ui->customPlot->hasItem(0))
//    {
//        ui->customPlot->removeItem(0);
//    }

    ui->customPlot->xAxis->setRange(0, SumPot);
    ui->customPlot->yAxis->setRange(-1, 1);//x��y��ʾ����Χ
    ui->customPlot->axisRect()->setupFullAxesBox();

//    ui->customPlot->plotLayout()->insertRow(0);
//    ui->customPlot->plotLayout()->addElement(0, 0, new QCPPlotTitle(ui->customPlot, QObject::tr("Ƶ��������ʾ")));

    ui->customPlot->xAxis->setLabel(QObject::tr("x ��"));
    ui->customPlot->yAxis->setLabel(QObject::tr("y ��"));
    ui->customPlot->legend->setVisible(true);

    int n = SumPot;// number of points in graph
    QVector<double> x(n), y(n);
    for (int i=0; i<n; i++)
    //for(int i=341;i<3414;i++)
    {
        x[i] = i;
        //qDebug()<<"\n x[i]:"<<x[i];
        y[i] = global_y[i];
        //qDebug()<<"\n y[i]:"<<y[i];
    }

    ui->customPlot->addGraph();
    ui->customPlot->graph()->setName(QString(QObject::tr("���� %1").arg(ui->customPlot->graphCount()-1)));//��������
    ui->customPlot->graph()->setData(x, y);//��������ֵ

    QPen graphPen;
    graphPen.setColor(QColor(255,0,0));
    graphPen.setWidthF(0.8);
    ui->customPlot->graph()->setPen(graphPen);
    ui->customPlot->replot();
}

void MainWindow::addSpecGraph()
{
    ui->customPlot->clearGraphs();

    ui->customPlot->xAxis->setRange(341, 3414);
    ui->customPlot->yAxis->setRange(0, 1);//x��y��ʾ����Χ
    ui->customPlot->axisRect()->setupFullAxesBox();

//    ui->customPlot->plotLayout()->insertRow(0);
//    ui->customPlot->plotLayout()->addElement(0, 0, new QCPPlotTitle(ui->customPlot, QObject::tr("Ƶ��������ʾ")));

    ui->customPlot->xAxis->setLabel(QObject::tr("Ƶ��"));
    ui->customPlot->yAxis->setLabel(QObject::tr("��һ������������"));
    ui->customPlot->legend->setVisible(true);

    int n = SumPot;// number of points in graph
    QVector<double> x(n), y(n);
    for(int i=341;i<3414;i++)
    {
        x[i] = i;
        //qDebug()<<"\n x[i]:"<<x[i];
        //y[i] = global_y1[i];
        y[i] = global_y1[i]*10000000000;//��ֵ̫С��ʾ����������Ҫ�ʵ�����
        //qDebug()<<"\n y[i]:"<<y[i];
    }

    ui->customPlot->addGraph();
    ui->customPlot->graph()->setName(QString(QObject::tr("���� %1").arg(ui->customPlot->graphCount()-1)));//��������
    ui->customPlot->graph()->setData(x, y);//��������ֵ

    QPen graphPen;
    graphPen.setColor(QColor(255,0,0));
    graphPen.setWidthF(0.8);
    ui->customPlot->graph()->setPen(graphPen);
    ui->customPlot->replot();
}

void MainWindow::removeSelectedGraph()
{
  if (ui->customPlot->selectedGraphs().size() > 0)
  {
    ui->customPlot->removeGraph(ui->customPlot->selectedGraphs().first());
    //ui->customPlot->replot();
  }
}

void MainWindow::removeAllGraphs()
{
  ui->customPlot->clearGraphs();
  ui->customPlot->replot();
}

void MainWindow::contextMenuRequest(QPoint pos)
{
  QMenu *menu = new QMenu(this);
  menu->setAttribute(Qt::WA_DeleteOnClose);

  if (ui->customPlot->legend->selectTest(pos, false) >= 0) // context menu on legend requested
  {
    menu->addAction(QObject::tr("������ʾ"), this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignLeft));
    menu->addAction(QObject::tr("������ʾ"), this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignHCenter));
    menu->addAction(QObject::tr("������ʾ"), this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignRight));
    menu->addAction(QObject::tr("������ʾ"), this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignRight));
    menu->addAction(QObject::tr("������ʾ"), this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignLeft));
  } else  // general context menu on graphs requested
  {
    //menu->addAction("Add random graph", this, SLOT(addRandomGraph()));
    if (ui->customPlot->selectedGraphs().size() > 0)
      menu->addAction(QObject::tr("ɾ����ѡ����"), this, SLOT(removeSelectedGraph()));
    if (ui->customPlot->graphCount() > 0)
      menu->addAction(QObject::tr("ɾ����������"), this, SLOT(removeAllGraphs()));
  }

  menu->popup(ui->customPlot->mapToGlobal(pos));
}

void MainWindow::moveLegend()
{
  if (QAction* contextAction = qobject_cast<QAction*>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
  {
    bool ok;
    int dataInt = contextAction->data().toInt(&ok);
    if (ok)
    {
      ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(0, (Qt::Alignment)dataInt);
      ui->customPlot->replot();
    }
  }
}

void MainWindow::graphClicked(QCPAbstractPlottable *plottable)
{
  ui->statusBar->showMessage(QString(QObject::tr("��ѡ������ '%1'.").arg(plottable->name())), 1000);
}

int MainWindow::on_action_result_triggered()
{
    diagram *analyse_diagram = new diagram;
    analyse_diagram->show();
    // �������ݿ�����
    if (!createConnection())
        return 1;

    //ʹ��QSqlQuery��ѯ����1�����ű���Ҫʹ����������ȡ������
    QSqlDatabase db = QSqlDatabase::database("connection1");
    // ʹ��QSqlQuery��ѯ���ű�
    QSqlQuery query(db);
    query.exec("select * from result");
    while(query.next())
    {
        qDebug() << query.value(0).toInt() << query.value(1).toString();
    }
}

//����Ӹ���֮ǰ��Ҫ��mainwindow.h�������źŲ�ӳ���ϵ����������login.h����
//���򲻻�ʶ������࣬����δ�����ʶ���Ĵ���
void MainWindow::on_action_login_triggered()
{
    login *customer = new login;
    customer->show();
}

void MainWindow::on_action_password_triggered()
{
    password *changepass = new password;
    changepass->show();
}

