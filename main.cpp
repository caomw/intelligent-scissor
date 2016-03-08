#include "scissor.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    scissor w;
    w.show();

    return a.exec();
}
