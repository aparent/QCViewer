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
		void quit ();
		void redraw ();
		void drawArchToggle ();

		void archComplete ();
		void archLNN ();

//  private:
  public:
		Circuit 					  *currentCirc;
		QGraphicsScene 			*mainScene;
		QGraphicsPixmapItem *currentCircImage;
    Ui::QCViewer ui;
		QString filename;
		bool drawArch;
};

#endif
