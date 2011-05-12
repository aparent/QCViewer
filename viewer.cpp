#include <viewer.h>
#include <cmath>
#include <QtGui>
#include <iostream>


using namespace std;

QCViewer::QCViewer(QWidget *parent) : QMainWindow(parent){
  ui.setupUi(this);
	connect(ui.fileOpen, SIGNAL(triggered()), this, SLOT(openFile()));
	connect(ui.actionQuit, SIGNAL(triggered()), this, SLOT(quit()));
	mainScene = new QGraphicsScene;
	currentCirc = NULL;
	currentCircImage = NULL;
	ui.mainView->setScene(mainScene);
	opening = false;
}

void QCViewer::openFile(){
	if (opening) return;
  opening = true;

	fileName = QFileDialog::getOpenFileName(this,
	 	tr("Open Circuit File"), "",
		tr("Circuit (*.tfc);;All Files (*)"));
	if (currentCircImage != NULL) {
		mainScene->removeItem (currentCircImage);
	  free (currentCircImage);
	}
	free (currentCirc);
	mainScene->clear();

  openthread = new Thread(this);
	connect(openthread, SIGNAL(doneSignal()), this, SLOT(loadImage()));
	openthread->start ();
}

void QCViewer::loadImage () {
	cout << "Loading image... " << flush;
	currentCircImage = new QGraphicsPixmapItem(QPixmap("./circuit.png"));
	cout << "ding!\n" << flush;
	if (currentCircImage == NULL) {
		cout << "ERROR: Could not create image for file " << fileName.toStdString() << endl;
		return;
	}
  mainScene->addItem(currentCircImage);
	disconnect (openthread, SIGNAL (doneSignal()), this, SLOT(loadImage()));
  opening = false;
//	free (openthread);
//	openthread = NULL;
}

void QCViewer::quit () {
  if (currentCircImage != NULL) mainScene->removeItem (currentCircImage);
	free (currentCirc);
	free (currentCircImage);
	exit(0); // classier way to do this?
}
