#include <viewer.h>
#include <cmath>
#include <QtGui>
#include <iostream>


using namespace std;
void draw (Circuit*);

QCViewer::QCViewer(QWidget *parent) : QMainWindow(parent){
  ui.setupUi(this);
	connect(ui.fileOpen, SIGNAL(triggered()), this, SLOT(openFile()));
	mainScene = new QGraphicsScene;
	currentCirc = NULL;
	currentCircImage = NULL;
	ui.mainView->setScene(mainScene);
}

void QCViewer::openFile(){
	QString fileName = QFileDialog::getOpenFileName(this,
	 	tr("Open Circuit File"), "",
		tr("Circuit (*.tfc);;All Files (*)"));
	mainScene->clear();
	free (currentCirc);
	currentCirc = parseCircuit(fileName.toStdString()); 
	draw(currentCirc);
	free (currentCircImage);
	currentCircImage= new QGraphicsPixmapItem(QPixmap("./hello.png"));
	mainScene->addItem(currentCircImage);
}
