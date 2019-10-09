#include "frame.h"
#include "deftools/appinit.h"
#include "deftools/apphelper.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    tools::AppInit *initApp = new  tools::AppInit;
    initApp->loadInfo();

    Frame w;
    tools::Helper::setStyle(":/pic/picture/qss/black.css");
    w.show();

    return a.exec();
}
