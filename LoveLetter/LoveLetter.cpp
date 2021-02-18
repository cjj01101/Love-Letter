#include "LvlSystemRoom.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoveLetter w;
    w.setFixedSize(LVL_GAME_WINDOW_WIDTH, LVL_GAME_WINDOW_HEIGHT);
    w.show();
    return a.exec();
}