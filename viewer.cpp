#include <viewer.h>
#include <cmath>
#include <QtGui>
#include <iostream>

using namespace std;

QCViewer::QCViewer(QWidget *parent) : QMainWindow(parent), drawArch (false) {
  ui.setupUi(this);
	connect(ui.fileOpen, SIGNAL(triggered()), this, SLOT(openFile()));
	connect(ui.actionQuit, SIGNAL(triggered()), this, SLOT(quit()));
	connect(ui.actionDraw_warnings, SIGNAL(triggered()), this, SLOT(drawArchToggle()));
	connect(ui.actionComplete, SIGNAL(triggered()), this, SLOT(archComplete()));
  connect(ui.actionLinear_nearest_neighbour, SIGNAL(triggered()), this, SLOT(archLNN()));
	mainScene = new QGraphicsScene;
	currentCirc = NULL;
	currentCircImage = NULL;
	ui.mainView->setScene(mainScene);
}

void QCViewer::archComplete () {
  currentCirc->removeArch ();
  redraw ();
}

void QCViewer::archLNN () {
	currentCirc->removeArch ();
  currentCirc->newArch ();
  int n = currentCirc->numLines ();

	for (int i = 0; i < n - 1; i ++) {
    currentCirc->arch->set (i, i+1);
	}
	redraw ();
}

void makepicture (Circuit*,bool,double);

void QCViewer::openFile(){
	filename = QFileDialog::getOpenFileName(this,
	 	tr("open circuit file"), "",
		tr("circuit (*.tfc);;all files (*)"));
	free (currentCirc);

	currentCirc = parseCircuit(filename.toStdString());
	if (currentCirc == NULL) {
		cout << "error: could not load file " << filename.toStdString() << endl;
		return;
	}
  redraw ();
}

void QCViewer::redraw () {
	if (currentCircImage != NULL) {
		mainScene->removeItem (currentCircImage);
	  free (currentCircImage);
	}

	mainScene->clear();
	ui.mainView->setScene (NULL);
	delete mainScene;
	mainScene = new QGraphicsScene;
	ui.mainView->setScene (mainScene);
	makepicture(currentCirc, drawArch, 1.0);

	cout << "loading image... " << flush;
	currentCircImage = new QGraphicsPixmapItem(QPixmap("./circuit.png"));
	cout << "ding!\n" << flush;
	if (currentCircImage == NULL) {
		cout << "error: could not create image for file " << filename.toStdString() << endl;
		return;
	}
  mainScene->addItem(currentCircImage);
	resize (mainScene->width()+40,mainScene->height ()+70); // there must be a better way than this?
}

void QCViewer::drawArchToggle () {
	drawArch = !drawArch;
	redraw ();
}

void QCViewer::quit () {
  if (currentCircImage != NULL) mainScene->removeItem (currentCircImage);
	free (currentCirc);
	free (currentCircImage);
	exit(0); // classier way to do this?
}
