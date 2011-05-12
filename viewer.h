#ifndef QCVIEWER_MAIN_WINDOW
#define QCVIEWER_MAIN_WINDOW


#include "ui_viewer.h"
#include "thread.h"
#include <circuitParser.h>

class QCViewer : public QMainWindow
{
Q_OBJECT
  public:
    QCViewer( QWidget *parent = 0 );

  private slots:
		void openFile();
		void quit ();
		void loadImage ();

//  private:
  public:
		Circuit 					  *currentCirc;
		QGraphicsScene 			*mainScene;
		QGraphicsPixmapItem *currentCircImage;
    Ui::QCViewer ui;
		Thread* openthread;
		QString fileName;
		bool opening;
};

#endif
