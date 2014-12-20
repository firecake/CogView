#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <qdebug.h>

// -------------------------------------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    wiiThread = new WiiThread();

    mesures = new QVector<QPointF>();

    connect(wiiThread, &WiiThread::connected,    this, &MainWindow::connected);
    connect(wiiThread, &WiiThread::disconnected, this, &MainWindow::disconnected);
    connect(wiiThread, &WiiThread::gotEvent, this, &MainWindow::gotEvent);

    xmin=100.0;
    xmax=-100.0;

    ymin=100.0;
    ymax=-100.0;

    ui->XY->xAxis->setRange(-25.0,  25.0);
    ui->XY->yAxis->setRange(-25.0,  25.0);
    ui->XY->addGraph();
    ui->XY->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->XY->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));

    ui->XY->addGraph();
    ui->XY->graph(1)->setPen(QPen(Qt::red));
    ui->XY->graph(1)->setLineStyle(QCPGraph::lsNone);
    ui->XY->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 10));

    circle = new QCPCurve(ui->XY->xAxis, ui->XY->yAxis);
    circle->setPen(QPen(Qt::black));
    ui->XY->addPlottable(circle);

    QTimer *timer0 = new QTimer(this);
    connect(timer0, SIGNAL(timeout()), this, SLOT(displayRate()));

    nb_evt = 0;
    before = new QDateTime(QDateTime::currentDateTime());

    timer0->start(1000);

}

// -------------------------------------------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete ui;
}

// -------------------------------------------------------------------------------------------------------------------
void MainWindow::connected()
{
    ui->label->setText("Connected");
}

// -------------------------------------------------------------------------------------------------------------------
void MainWindow::disconnected()
{
    ui->label->setText("Disconnected");
}

// -------------------------------------------------------------------------------------------------------------------
void MainWindow::gotEvent (float total, float topLeft, float topRight, float bottomLeft, float bottomRight)
{

    QPointF *m;

    m = new QPointF( -(topLeft+bottomLeft)+(topRight+bottomRight), (topLeft+topRight)-(bottomLeft+bottomRight));
    mesures->append(*m);

    ui->XY->graph(0)->addData( m->x(), m->y() );
    ui->XY->replot();
    nb_evt++;
}

// -------------------------------------------------------------------------------------------------------------------
void MainWindow::on_pushButton_Effacer_clicked()
{

    xmin=100.0;
    xmax=-100.0;

    ymin=100.0;
    ymax=-100.0;

    mesures->clear();

    ui->XY->graph(0)->clearData();
    ui->XY->graph(1)->clearData();

    ui->XY->xAxis->setRange(-25.0,  25.0);
    ui->XY->yAxis->setRange(-25.0,  25.0);


}

// -------------------------------------------------------------------------------------------------------------------
void MainWindow::displayRate()
{

  // Update rate

  QDateTime now = QDateTime::currentDateTime();
  ui->label->setText(QString::number(nb_evt)+" "+QString::number(before->msecsTo(now)));
  before->setTime(now.time());

  nb_evt = 0;

  XYautoscale();
  calculSurface();

  ui->XY->replot();

}

// -------------------------------------------------------------------------------------------------------------------
void MainWindow::calculSurface()
{

    QList<float> dg;

    // -----------------  Calcul du centre de gravité -------------------------------

    gx = 0.0;
    gy = 0.0;

    for(int i=0;i<mesures->size();i++)
    {
      gx = gx + mesures->at(i).x();
      gy = gy + mesures->at(i).y();
    }

    gx = gx / mesures->size();
    gy = gy / mesures->size();

    ui->XY->graph(1)->clearData();
    ui->XY->graph(1)->addData(gx,gy);

    // -----------------  Calcul du cercle contenant 80% des points les plus proches du centre de gravité -------------------------------

    for(int i=0;i<mesures->size();i++)
      dg.append(sqrt( ( mesures->at(i).x() - gx ) * ( mesures->at(i).x() - gx ) + ( mesures->at(i).y() - gy ) * ( mesures->at(i).y() - gy  ) ));

    qSort(dg.begin(), dg.end());

    circleR = dg.at( dg.size() * 0.85 );

    int pointCount = 100;
    QVector<double> x1(pointCount), y1(pointCount);
    for (int i=0; i<pointCount; ++i)
    {
      double phi = (i/(double)(pointCount-1))*2*M_PI;
      x1[i] = gx + circleR * cos(phi);
      y1[i] = gy + circleR * sin(phi);
    }
    // pass the data to the curves:
    circle->clearData();
    circle->setData(x1, y1);
}

// -------------------------------------------------------------------------------------------------------------------
void MainWindow::on_pushButton_Charger_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
         tr("Chargement"), ".", tr("Fichiers texte (*.cog)"));

    QSettings settings(fileName ,QSettings::IniFormat);

    int size = settings.beginReadArray("Points");

        for (int i=0; i < size;i++)
        {
            settings.setArrayIndex(i);
            mesures->append(settings.value("point").toPointF());
        }

        settings.endArray();

        for(int i=0;i<mesures->size();i++)
        {
            ui->XY->graph(0)->addData( mesures->at(i).x(), mesures->at(i).y() );
        }

        XYautoscale();

        ui->XY->replot();

        calculSurface();

}

// -------------------------------------------------------------------------------------------------------------------
void MainWindow::on_pushButton_Sauver_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Sauvegarde"),
                               "./untitled.txt",
                               tr("Fichiers texte (*.cog)"));

    QSettings settings(fileName,QSettings::IniFormat);
    settings.beginWriteArray("Points",mesures->count());

    for (int i=0; i < mesures->count();i++)
    {
        settings.setArrayIndex(i);
        settings.setValue("point",mesures->at(i));
    }

    settings.endArray();
}

// -------------------------------------------------------------------------------------------------------------------
void MainWindow::on_pushButton_Connect_clicked()
{
        wiiThread->start();
}

// -------------------------------------------------------------------------------------------------------------------
void MainWindow::XYautoscale()
{

    float m, xrange, yrange;

    // update autoscale

    for(int i=0;i<mesures->size();i++)
    {
        if ( mesures->at(i).x() > xmax ) xmax = mesures->at(i).x();
        if ( mesures->at(i).x() < xmin ) xmin = mesures->at(i).x();
        if ( mesures->at(i).y() > ymax ) ymax = mesures->at(i).y();
        if ( mesures->at(i).y() < ymin ) ymin = mesures->at(i).y();
    }

    //ui->XY->xAxis->setRange(xmin - ( xmax - xmin ) /10, xmax  + ( xmax - xmin ) /10);
    //ui->XY->yAxis->setRange(ymin - ( ymax - ymin ) /10, ymax  + ( ymax - ymin ) /10);

    xrange = abs(xmax - xmin);
    yrange = abs(ymax - ymin);

    if ( xrange > yrange )
    {
        m = ( xrange - yrange ) / 2 ;
        ui->XY->xAxis->setRange(xmin - ( xmax - xmin ) /10, xmax  + ( xmax - xmin ) /10);
        ui->XY->yAxis->setRange( - m + (ymin - ( ymax - ymin ) /10), m + (ymax  + ( ymax - ymin ) /10));
    }
    else
    {
        m = ( yrange - xrange ) / 2 ;
        ui->XY->xAxis->setRange( - m + ( xmin - ( xmax - xmin ) /10 ), m + (xmax  + ( xmax - xmin ) /10));
        ui->XY->yAxis->setRange(ymin - ( ymax - ymin ) /10, ymax  + ( ymax - ymin ) /10);
    }

}





