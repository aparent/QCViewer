#ifndef QCVIEWER_MAIN_WINDOW
#define QCVIEWER_MAIN_WINDOW


#include "ui_viewer.h"
#include <circuitParser.h>

class QCViewer : public QMainWindow
{
Q_OBJECT
  public:
    QCViewer( QWidget *parent = 0 );

  private slots:
		void openFile();

  private:
		Circuit 					  *currentCirc;
		QGraphicsScene 			*mainScene;
		QGraphicsPixmapItem *currentCircImage;
    Ui::QCViewer ui;
};

#endif
