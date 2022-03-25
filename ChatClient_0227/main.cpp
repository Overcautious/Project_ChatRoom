#include "widget.h"
#include <QApplication>
#include "buffer.h"
#include "dataencoder.h"
#include "dataparser.h"
#include <QDebug>
#include <QDateTime>
#include "loginform.h"

int main(int argc, char *argv[])
{



    QApplication a(argc, argv);
//    widget w;
//    w.show();
    loginForm l;
    l.show();

    return a.exec();
}
