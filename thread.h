#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QtGui>
#include <QtCore/QThread>
#include <QtCore/QMutex>

class QCViewer;

class Thread : public QThread
{
    Q_OBJECT

public:
    QCViewer *qc;
    Thread(QObject *parent);
    void run(); // this is virtual method, we must implement it in our subclass of QThread

signals:
    void doneSignal ();
};

#endif
