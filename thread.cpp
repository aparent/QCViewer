#include "viewer.h"
#include "thread.h"
#include <iostream>
using namespace std;

Thread::Thread(QObject *parent) : QThread(parent), qc((QCViewer*)parent) {}
void makepicture (Circuit*, double);

void Thread::run() {
	cout << "Parsing circuit... ";
	qc->currentCirc = parseCircuit(qc->fileName.toStdString());
  cout << "ding!\n";
	if (qc->currentCirc == NULL) {
		cout << "ERROR: Could not load file " << qc->fileName.toStdString() << endl;
		return;
	}
	makepicture(qc->currentCirc, 1.0);
  emit doneSignal ();
	exec();
}


