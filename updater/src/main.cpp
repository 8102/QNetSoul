#include <QtGui>
#include "Updater.h"

int     main(int argc, char** argv)
{
  QApplication  app(argc, argv);
  Updater       updater;

  if (argc != 3) return 1;
  updater.show();
  return app.exec();
}
