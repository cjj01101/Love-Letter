////////////////////////////////////////////////
//File Name           : LvlSystemRoom.h
//2nd Model Name      : System
//Description         : Define all the rooms in the program.
//Lastest Update Time : 2021/2/14 16:36
//Version             : 1.0
////////////////////////////////////////////////

#pragma once

#include <QtWidgets/QMainWindow>
#include <QMainWindow>
#include <QStackedWidget>
#include "LvlGameCtrl.h"
#include "ui_LoveLetter.h"

///////�����水ť�ļ��β���
#define LVL_SYSTEM_START_BTN_WIDTH        (160)
#define LVL_SYSTEM_START_BTN_HEIGHT       (60)
#define LVL_SYSTEM_START_BTN_TOP          (LVL_GAME_WINDOW_HEIGHT / 2 - 80)
#define LVL_SYSTEM_START_BTN_SPACE        (120)
#define LVL_SYSTEM_START_PLAY_GEO         (QRect((LVL_GAME_WINDOW_WIDTH - LVL_SYSTEM_START_BTN_WIDTH) / 2, LVL_SYSTEM_START_BTN_TOP,\
                                          LVL_SYSTEM_START_BTN_WIDTH , LVL_SYSTEM_START_BTN_HEIGHT))
#define LVL_SYSTEM_START_OPTION_GEO       (QRect((LVL_GAME_WINDOW_WIDTH - LVL_SYSTEM_START_BTN_WIDTH) / 2, LVL_SYSTEM_START_BTN_TOP + LVL_SYSTEM_START_BTN_SPACE,\
                                          LVL_SYSTEM_START_BTN_WIDTH , LVL_SYSTEM_START_BTN_HEIGHT))
#define LVL_SYSTEM_START_EXIT_GEO         (QRect((LVL_GAME_WINDOW_WIDTH - LVL_SYSTEM_START_BTN_WIDTH) / 2, LVL_SYSTEM_START_BTN_TOP + LVL_SYSTEM_START_BTN_SPACE * 2,\
                                          LVL_SYSTEM_START_BTN_WIDTH , LVL_SYSTEM_START_BTN_HEIGHT))
#define LVL_SYSTEM_PLAYER_NUM_HINT_GEO    (QRect(400, 197, 200, 30))
#define LVL_SYSTEM_PLAYER_NUM_SPINBOX_GEO (QRect(600, 200, 50, 30))
#define LVL_SYSTEM_BACK_BTN_GEO           (QRect(0, 0, 120, 60))
///////�����水ť�ļ��β���
#define LVL_SYSTEM_CREATE_ROOM(manager, roomname)  LVL_System##roomname *pst##roomname = new LVL_System##roomname(this);\
                                                   manager##->LVL_SystemAddRoom(pst##roomname, #roomname);

///////////////////////////////////////////////////////////////

QT_BEGIN_NAMESPACE
namespace Ui { class LoveLetterClass; }
QT_END_NAMESPACE

class LoveLetter : public QMainWindow
{
    Q_OBJECT

public:
    LoveLetter(QWidget *parent = Q_NULLPTR);
    ~LoveLetter();

private:
    Ui::LoveLetterClass *ui;
};

////////////////////////////////////////////////////////////
//                      �������                          //
////////////////////////////////////////////////////////////

class LVL_SystemAbstractRoom : public QWidget {
    Q_OBJECT

public:
    LVL_SystemAbstractRoom(QWidget *parent = Q_NULLPTR, const QString &qstrBackground = "");//���캯��

signals:
    void sgnChangeRoom(const QString &);//�źź�����ת������
};

////////////////////////////////////////////////////////////
//                      ��ʼ����                          //
////////////////////////////////////////////////////////////

class LVL_SystemStartRoom : public LVL_SystemAbstractRoom {
    Q_OBJECT

public:
    LVL_SystemStartRoom(QWidget *parent = Q_NULLPTR);//���캯��
};

////////////////////////////////////////////////////////////
//                      ��Ϸ����                          //
////////////////////////////////////////////////////////////

class LVL_SystemGameRoom : public LVL_SystemAbstractRoom {
    Q_OBJECT

public:
    LVL_SystemGameRoom(QWidget *parent = Q_NULLPTR);//���캯��
    void LVL_SystemInitGame();//��ʼ����Ϸ
    void LVL_SystemClearGame() { pstGame->close(); } //�������

protected:
    void showEvent(QShowEvent *event) { LVL_SystemInitGame(); }
    void hideEvent(QHideEvent *event) { LVL_SystemClearGame();}

private:
    LVL_GameCtrller *pstGame;
};

////////////////////////////////////////////////////////////
//                      ���÷���                          //
////////////////////////////////////////////////////////////

class LVL_SystemOptionRoom : public LVL_SystemAbstractRoom {
    Q_OBJECT

public:
    LVL_SystemOptionRoom(QWidget *parent = Q_NULLPTR);//���캯��

protected:
    void hideEvent(QHideEvent *event);

private:
    QSpinBox *pstPlayerNumSpinBox;
};

///////////////////////////////////////////////////////////////
//                     ���������                            //
///////////////////////////////////////////////////////////////

class LVL_SystemRoomManager : public QStackedWidget {
    Q_OBJECT

public:
    LVL_SystemRoomManager(QWidget *parent = Q_NULLPTR) : QStackedWidget(parent) { setGeometry(0, 0, LVL_GAME_WINDOW_WIDTH, LVL_GAME_WINDOW_HEIGHT); }//���캯��

    void LVL_SystemAddRoom(LVL_SystemAbstractRoom* pstRoom, const QString& qstrRoomName) {
        qmRooms.insert(qstrRoomName, addWidget(pstRoom));
        connect(pstRoom, &LVL_SystemAbstractRoom::sgnChangeRoom, this, &LVL_SystemRoomManager::LVL_SystemChangeRoom);
    }

private:
    QMap<QString, int> qmRooms;

public slots:
    void LVL_SystemChangeRoom(const QString &qstrRoomName) {
        if (!qmRooms.contains(qstrRoomName)) return;
        setCurrentIndex(qmRooms[qstrRoomName]);
    }
};