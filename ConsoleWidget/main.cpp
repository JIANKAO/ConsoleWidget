#include "ConsoleWidget.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ConsoleWidget window;
    window.show();
    return app.exec();
}
