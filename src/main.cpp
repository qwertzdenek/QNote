/*
 * Copyleft 2013 Zdeněk Janeček
 * ycdmdj@gmail.com
 *
 * Share it under GPL version 3
 *
 * version 1.2
 */

#include "mainwindow.h"
#include <QApplication>

#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QObject::connect(&a, SIGNAL(aboutToQuit()), &w, SLOT(cl()));

    return a.exec();
}
