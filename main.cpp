#include <QApplication>
#include <circuitParser.h>
#include "viewer.h"

int main(int argc,char *argv[])
{
  QApplication app(argc, argv);
  QCViewer mainWindow;

  mainWindow.show();
  return app.exec();
}
