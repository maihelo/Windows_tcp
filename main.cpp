#include "wifireceiver.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WifiReceiver w;
    w.show();
    return a.exec();
}
