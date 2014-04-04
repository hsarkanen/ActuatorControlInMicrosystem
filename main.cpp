//----------------------------------------------------------------------------
//
//  Pääohjelman toteutus
//
//----------------------------------------------------------------------------

#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    // ohjelmiston alustus
    QApplication a(argc, argv);

    // luodaan pääikkuna
    MainWindow w;

    // asetetaan otsikko pääikkunalle
    w.setWindowTitle("Actuator controller");

    // laitetaan pääikkuna näkyviin
    w.show();

    // kytketään sulkemissignaali ohjelman sulkemiseen
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    int result = a.exec();
    return result;
}
