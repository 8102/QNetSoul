#include <QApplication>
#include "pluginsmanager.h"

int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  PluginsManager pm;

  pm.show();
  return app.exec();
}
