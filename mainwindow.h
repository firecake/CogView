#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "wiithread.h"
#include <QVector>
#include "qcustomplot.h"

#define WBIN 0.10

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void connected();
    void disconnected();
    void gotEvent (float total, float topLeft, float topRight, float bottomLeft, float bottomRight);

    void displayRate();
    void calculSurface();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_Effacer_clicked();

    void on_pushButton_Charger_clicked();

    void on_pushButton_Sauver_clicked();

    void on_pushButton_Connect_clicked();

private:
    Ui::MainWindow *ui;

    WiiThread *wiiThread;

    QTimer *timer0;
    QDateTime *before;

    int nb_evt;

    float circleR;
    float gx;
    float gy;

    float xmin, xmax, ymin, ymax;

    float surface;

    QCPCurve *circle;

    QVector<QPointF> *mesures;

    QVector<float> *FFTx;
    QVector<float> *FFTy;

    void XYautoscale();
};

#endif // MAINWINDOW_H
