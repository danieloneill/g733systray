#include <QApplication>
#include <QDBusServiceWatcher>

#include "mainapplication.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    MainApplication m;

    return a.exec();
}
