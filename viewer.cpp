#include <viewer.h>
#include <cmath>
#include <QtGui>
#include <iostream>
#include "prompt.h"

QCViewer::QCViewer(QWidget *parent) : QMainWindow(parent), drawArch (false) {
  ui.setupUi(this);
	connect(ui.fileOpen, SIGNAL(triggered()), this, SLOT(openFile()));
	connect(ui.parseCommandLine, SIGNAL(triggered()), this, SLOT(parseCommandLine ()));
	connect(ui.actionQuit, SIGNAL(triggered()), this, SLOT(quit()));
	connect(ui.actionDraw_warnings, SIGNAL(triggered()), this, SLOT(drawArchToggle()));
	connect(ui.actionComplete, SIGNAL(triggered()), this, SLOT(archComplete()));
  connect(ui.actionLinear_nearest_neighbour, SIGNAL(triggered()), this, SLOT(archLNN()));
	mainScene = new QGraphicsScene;
	currentCirc = NULL;
	currentCircImage = NULL;
	ui.mainView->setScene(mainScene);
}

void QCViewer::parseCommandLine () {
	QString cmd = ui.lineEdit->text ();
	ui.lineEdit->clear ();
	string out = "> ";
	out.append (cmd.toStdString());
	ui.plainTextEdit->appendPlainText (QString::fromStdString(out));
	ui.plainTextEdit->appendPlainText (QString::fromStdString(prompt.parse (cmd.toStdString())));
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
	if (filename == "") return;
	cout << filename.toStdString () << endl << flush;
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
	  delete currentCircImage;
	}

	mainScene->clear();
	ui.mainView->setScene (NULL);
	delete mainScene;
	mainScene = new QGraphicsScene;
	ui.mainView->setScene (mainScene);
	makepicture(currentCirc, drawArch, 1.0);

	currentCircImage = new QGraphicsPixmapItem(QPixmap("./circuit.png"));
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
