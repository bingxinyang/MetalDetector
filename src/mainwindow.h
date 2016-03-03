#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUrl>
#include <QDesktopServices>//���ڷ�������������
#include "about.h"
#include <QCloseEvent>//��������Ҫ�ر�ʱ����������ݣ����е��¼������ܱ����ĳ�Ա��������protected

#include "qcustomplot.h"
#include <QInputDialog>

#include <QVector>
#include <QWidget>
#include <QMap>

//#include "fft.h"
#include "diagram.h"
#include "connection.h"

#include "login.h"
#include "password.h"

#include <QGLWidget>

#define SINT short int
#define SumPot 16384

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool readFile(QString fileName);    //��ȡ�ļ�����
    int QByteArrayToInt(QByteArray bytes);  //QByteArrayתint
    const char* getString(const QByteArray ba);

protected:
    void closeEvent(QCloseEvent *);
    void paintEvent(QPaintEvent *event);

private slots:
    //��һ���˵����ۺ�������
    void on_action_O_triggered();
    void on_action_A_triggered();
    void on_action_P_triggered();
    void on_action_Q_triggered();

    //�ڶ����˵����ۺ�������
    void on_action_wave_triggered();
    void on_action_spec_triggered();
    void on_action_2D_triggered();
    void on_action_3D_triggered();

    //�������˵����ۺ�������
    void on_action_zoom_in_triggered();
    void on_action_zoom_out_triggered();
    void on_action_rotate_triggered();
    void on_action_clear_triggered();

    //���ĸ��˵����ۺ�������
    void aboutSoftwareSlot();//������������������ӶԻ���
    void aboutWebsiteSlot();//��һ����ַ��ʹ�õ���Ĭ�ϵ������

    //�����������ض���
    void titleDoubleClick(QMouseEvent *event, QCPPlotTitle *title);
    void axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);
    void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
    void selectionChanged();
    void mousePress();
    void mouseWheel();
    void addWaveGraph();
    void addSpecGraph();
    void removeSelectedGraph();
    void removeAllGraphs();
    void contextMenuRequest(QPoint pos);
    void moveLegend();
    void graphClicked(QCPAbstractPlottable *plottable);

    int on_action_result_triggered();
    void on_action_login_triggered();
    void on_action_password_triggered();

private:
    Ui::MainWindow *ui;

    struct wav_struct
    {
        unsigned long file_size;        //�ļ���С
        unsigned short channel;            //ͨ����
        unsigned long frequency;        //����Ƶ��
        unsigned long Bps;                //Byte��
        unsigned short sample_num_bit;    //һ��������λ��
        int data_size;        //���ݴ�С
        unsigned char *data;            //��Ƶ���� ,����Ҫ����ʲô�Ϳ�����λ���ˣ�������ֻ�ǵ����ĸ�������
    };

    QVector<QPointF>  curveMap;//�洢����ͼ�ϵĶ���
};

#endif // MAINWINDOW_H
