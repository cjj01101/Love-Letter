////////////////////////////////////////////////
//File Name           : LVLGameMain.h
//2nd Model Name      : Game
//Description         : Define the GameController.
//Lastest Update Time : 2020/12/30 12:35
//Version             : 1.3
////////////////////////////////////////////////

#pragma once

#include <QtWidgets/QMainWindow>
#include <QMainWindow>
#include <QTime>
#include <QPalette>
#include <QPainter>
#include <QLayout>
#include <QState>
#include <QFinalState>
#include <QStateMachine>
#include "ui_LVLGameMain.h"
#include "LvlInterObject.h"

#define LVL_GAME_TOTAL_CARD_NUM         16   //��Ϸ������
#define LVL_GAME_UNSEEN_REMOVE_CARD_NUM 1    //�Ƴ��Ĳ��ɼ�������
#define LVL_GAME_SEEN_REMOVE_CARD_NUM   0    //�Ƴ��Ŀɼ�������
#define LVL_GAME_REMOVE_CARD_NUM        (LVL_GAME_UNSEEN_REMOVE_CARD_NUM + LVL_GAME_SEEN_REMOVE_CARD_NUM)    //�Ƴ����п�����
#define LVL_GAME_TOTAL_PLAYER_NUM       4    //��Ϸ�������
#define LVL_GAME_AI_WAIT_MSEC           1500 //AI��Ӧʱ��
#define LVL_GAME_WATCHING_MSEC          2000 //���ܷ��Ƴ���ʱ��

#define LVL_GAME_WINDOW_WIDTH           1200 //���ڿ��
#define LVL_GAME_WINDOW_HEIGHT          800  //���ڸ߶�
#define LVL_GAME_VERT_MARGIN            23   //��ֱ�߾�
#define LVL_GAME_RIGHT_MARGIN           23   //�ұ߾�
#define LVL_GAME_LEFT_MARGIN            19   //��߾�
#define LVL_GAME_DISCARD_SCALE          0.6  //���ƶѿ��Ƶ����Ŵ�С
#define LVL_GAME_REMOVE_SCALE           0.8  //���ƶѿ��Ƶ����Ŵ�С
///////�����ƶѵ���ʾλ��
#define LVL_GAME_DECK_SHIFT_X           40
#define LVL_GAME_DECK_TEXT_FONT         "Microsoft YaHei"
#define LVL_GAME_DECK_TEXT_SIZE         15
#define LVL_GAME_DECK_TEXT_LENGTH       300
#define LVL_GAME_DECK_TEXT_GEO          QRect((LVL_GAME_WINDOW_WIDTH - LVL_GAME_DECK_TEXT_LENGTH) / 2,\
                                        LVL_GAME_WINDOW_HEIGHT / 2 + LVL_GAME_DECK_TEXT_SIZE - 45 * (int)(2 == LVL_GAME_TOTAL_PLAYER_NUM) - LVL_GAME_DECK_SHIFT_X,\
                                        LVL_GAME_DECK_TEXT_LENGTH, LVL_GAME_DECK_TEXT_SIZE * 2)
#define LVL_GAME_REMOVECARD_WIDTH       (LVL_INTER_CARD_WIDTH * LVL_GAME_REMOVE_SCALE)
#define LVL_GAME_REMOVECARD_HEIGHT      (LVL_INTER_CARD_HEIGHT * LVL_GAME_REMOVE_SCALE)
#define LVL_GAME_REMOVECARD_X           ((LVL_GAME_WINDOW_WIDTH - LVL_GAME_REMOVECARD_WIDTH * LVL_GAME_SEEN_REMOVE_CARD_NUM) / 2)
#define LVL_GAME_REMOVECARD_Y           (LVL_GAME_WINDOW_HEIGHT / 2 - LVL_GAME_DECK_SHIFT_X)
///////�����ƶѵ���ʾλ��
///////�����ƽ������ʾλ��
#define LVL_GAME_GUESSER_WIDTH          800
#define LVL_GAME_GUESSER_HEIGHT         600
#define LVL_GAME_GUESSER_POS            QPoint((LVL_GAME_WINDOW_WIDTH - LVL_GAME_GUESSER_WIDTH) / 2, (LVL_GAME_WINDOW_HEIGHT - LVL_GAME_GUESSER_HEIGHT) / 2)
///////�����ƽ������ʾλ��
///////�������ͷ��λ�ô�С
#define LVL_GAME_PLAYERPORT_WIDTH       180
#define LVL_GAME_PLAYERPORT_HEIGHT      180
#define LVL_GAME_PLAYERPORT_GEO         QRect(LVL_GAME_WINDOW_WIDTH - LVL_GAME_PLAYERPORT_WIDTH - LVL_GAME_RIGHT_MARGIN * 5,\
                                        LVL_GAME_WINDOW_HEIGHT - LVL_GAME_PLAYERPORT_HEIGHT - LVL_GAME_VERT_MARGIN,\
                                        LVL_GAME_PLAYERPORT_WIDTH,  LVL_GAME_PLAYERPORT_HEIGHT)
#define LVL_GAME_AIPORT_WIDTH           120
#define LVL_GAME_AIPORT_HEIGHT          120
#define LVL_GAME_AI1PORT_GEO            QRect(LVL_GAME_LEFT_MARGIN,\
                                        (LVL_GAME_WINDOW_HEIGHT - LVL_INTER_CARD_HEIGHT) / 2 - LVL_GAME_AIPORT_HEIGHT - LVL_INTER_CARD_SHIFT,\
                                        LVL_GAME_AIPORT_WIDTH,  LVL_GAME_AIPORT_HEIGHT)
#define LVL_GAME_AI2PORT_GEO            QRect(LVL_GAME_WINDOW_WIDTH / 2 - LVL_INTER_CARD_WIDTH - LVL_GAME_AIPORT_WIDTH,\
                                        LVL_GAME_VERT_MARGIN,\
                                        LVL_GAME_AIPORT_WIDTH,  LVL_GAME_AIPORT_HEIGHT)
#define LVL_GAME_AI3PORT_GEO            QRect(LVL_GAME_WINDOW_WIDTH - LVL_GAME_AIPORT_WIDTH - LVL_GAME_RIGHT_MARGIN,\
                                        (LVL_GAME_WINDOW_HEIGHT - LVL_INTER_CARD_HEIGHT) / 2 - LVL_GAME_AIPORT_HEIGHT - LVL_INTER_CARD_SHIFT,\
                                        LVL_GAME_AIPORT_WIDTH,  LVL_GAME_AIPORT_HEIGHT)
///////�������ͷ��λ�ô�С
///////����������������ƶ�λ��
#define LVL_GAME_PLAYER_SLOT            QPoint(LVL_GAME_WINDOW_WIDTH / 2 - LVL_INTER_CARD_WIDTH, LVL_GAME_WINDOW_HEIGHT - LVL_GAME_VERT_MARGIN - LVL_INTER_CARD_HEIGHT)
#define LVL_GAME_PLAYER_DISCARD         (LVL_GAME_PLAYER_SLOT + QPoint(LVL_INTER_CARD_WIDTH * 2, LVL_INTER_CARD_HEIGHT * (1 - LVL_GAME_DISCARD_SCALE)))
#define LVL_GAME_AI1_SLOT               QPoint(LVL_GAME_LEFT_MARGIN, (LVL_GAME_WINDOW_HEIGHT - LVL_INTER_CARD_HEIGHT) / 2)
#define LVL_GAME_AI1_DISCARD            (LVL_GAME_AI1_SLOT + QPoint(0, LVL_INTER_CARD_HEIGHT))
#define LVL_GAME_AI2_SLOT               QPoint(LVL_GAME_WINDOW_WIDTH / 2 - LVL_INTER_CARD_WIDTH, LVL_INTER_CARD_SHIFT + LVL_GAME_VERT_MARGIN)
#define LVL_GAME_AI2_DISCARD            (LVL_GAME_AI2_SLOT + QPoint(LVL_INTER_CARD_WIDTH * 2, 0))
#define LVL_GAME_AI3_SLOT               QPoint(LVL_GAME_WINDOW_WIDTH - LVL_INTER_CARD_WIDTH * 2 - LVL_GAME_RIGHT_MARGIN, (LVL_GAME_WINDOW_HEIGHT - LVL_INTER_CARD_HEIGHT) / 2)
#define LVL_GAME_AI3_DISCARD            (LVL_GAME_AI3_SLOT + QPoint(0, LVL_INTER_CARD_HEIGHT))
///////����������������ƶ�λ��

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

    friend class LVL_GameCtrller;
};

///////////////////////////////////////////////////////////////
//                        ������                             //
///////////////////////////////////////////////////////////////

class LVL_GameDealer : public QWidget {
    Q_OBJECT

public:
    LVL_GameDealer(QWidget *parent = Q_NULLPTR);
    int LVL_GameMakeDeal(LVL_GamePlayer *pstPlayer, LVL_InterCard *pstCard, bool bDealUnseen);//���з��Ʋ���

protected:
    LVL_InterCard::LVL_InterCardType_E aenDrawPile[LVL_GAME_TOTAL_CARD_NUM];//���ƶ�
    int sCurCard;//��һ��Ҫ������
};

///////////////////////////////////////////////////////////////
//                        �¿���                             //
///////////////////////////////////////////////////////////////

class LVL_GameGuesser : public QWidget {
    Q_OBJECT

public:
    LVL_GameGuesser(QWidget *parent = Q_NULLPTR, const QPoint &qpPos = { 0, 0 }, int width = LVL_GAME_GUESSER_WIDTH, int height = LVL_GAME_GUESSER_HEIGHT);

private:
    LVL_InterCard::LVL_InterCardType_E enGuess;

signals:
    void sgnGuessFinished(LVL_InterCard::LVL_InterCardType_E);
};

///////////////////////////////////////////////////////////////
//                      ��Ϸ������                           //
///////////////////////////////////////////////////////////////

class LVL_GameCtrller : public LVL_GameDealer {
    Q_OBJECT

public:
    LVL_GameCtrller(QWidget *parent = Q_NULLPTR);
    LVL_GamePlayer *LVL_GameGetTarget(LVL_InterCard::LVL_InterCardType_E enSelect, LVL_GamePlayer *pstSelf);
    void LVL_GameTriggerEffect(LVL_InterCard::LVL_InterCardType_E enType, LVL_GamePlayer* pstSelf, LVL_GamePlayer *pstTarget);//�������Ƽ���

private:
    LVL_GamePlayer *pstHuman, *pstAI1, *pstAI2, *pstAI3;
    LVL_GameGuesser *pstGuesser;

private slots:
    void LVL_GameCalculatePoint();
};
