#ifndef WIITHREAD_H
#define WIITHREAD_H

#include <QThread>

class WiiThread : public QThread
{
    Q_OBJECT

    void run();

signals:
    void connected();
    void gotEvent (float, float, float, float, float);
    void disconnected();
};

#endif // WIITHREAD_H



