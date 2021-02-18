////////////////////////////////////////////////
//File Name           : LvlGameCtrl.h
//2nd Model Name      : Game
//Description         : Define the GameController that controls the game.
//Lastest Update Time : 2021/2/14 16:36
//Version             : 1.4
////////////////////////////////////////////////

#pragma once

#include <QTime>
#include <QLayout>
#include <QState>
#include <QStateMachine>
#include <QEventLoop>
#include "LvlInterObject.h"

#define LVL_GAME_TOTAL_CARD_NUM         (16)   //��Ϸ������
#define LVL_GAME_MINIMUM_PLAYER_NUM     (2)    //��Ϸ��С�����
#define LVL_GAME_MAXIMUM_PLAYER_NUM     (4)    //��Ϸ��������
#define LVL_GAME_UNSEEN_REMOVE_CARD_NUM (1)    //�Ƴ��Ĳ��ɼ�������
#define LVL_GAME_SEEN_REMOVE_CARD_NUM   ((2 == sPlayerNum) ? 3 : 0)    //�Ƴ��Ŀɼ�������
#define LVL_GAME_REMOVE_CARD_NUM        (LVL_GAME_UNSEEN_REMOVE_CARD_NUM + LVL_GAME_SEEN_REMOVE_CARD_NUM)    //�Ƴ����п�����
#define LVL_GAME_AI_WAITING_MSEC        (1500) //AI��Ӧʱ��
#define LVL_GAME_MOVING_MSEC            (500)  //�ƶ����ƶ���ʱ��
#define LVL_GAME_FLIPPING_MSEC          (400)  //���ƶ���ʱ��
#define LVL_GAME_WATCHING_MSEC          (2000) //���ƺ�鿴ʱ��

#define LVL_GAME_WINDOW_WIDTH           (1200) //���ڿ��
#define LVL_GAME_WINDOW_HEIGHT          (800)  //���ڸ߶�
#define LVL_GAME_VERT_MARGIN            (23)   //��ֱ�߾�
#define LVL_GAME_RIGHT_MARGIN           (23)   //�ұ߾�
#define LVL_GAME_LEFT_MARGIN            (19)   //��߾�
#define LVL_GAME_DISCARD_SCALE          (0.6)  //���ƶѿ��Ƶ����Ŵ�С
#define LVL_GAME_REMOVE_SCALE           (0.8)  //���ƶѿ��Ƶ����Ŵ�С
#define LVL_GAME_CONFIRM_BTN_GEO        (QRect(720, 595, 70, 70)) //ȷ�ϰ�ť��λ��
///////�����ƶѵļ��β���
#define LVL_GAME_DECK_SHIFT_X           (40)
#define LVL_GAME_DECK_TEXT_FONT         ("Microsoft YaHei")
#define LVL_GAME_DECK_TEXT_SIZE         (15)
#define LVL_GAME_DECK_TEXT_LENGTH       (300)
#define LVL_GAME_DECK_TEXT_COLOR        (QColor(255,195,14))
#define LVL_GAME_DECK_TEXT_GEO          (QRect((LVL_GAME_WINDOW_WIDTH - LVL_GAME_DECK_TEXT_LENGTH) / 2,\
                                        LVL_GAME_WINDOW_HEIGHT / 2 + LVL_GAME_DECK_TEXT_SIZE - 55 * (int)(2 == sPlayerNum) - LVL_GAME_DECK_SHIFT_X,\
                                        LVL_GAME_DECK_TEXT_LENGTH, LVL_GAME_DECK_TEXT_SIZE * 2))
#define LVL_GAME_REMOVECARD_WIDTH       (LVL_INTER_CARD_WIDTH * LVL_GAME_REMOVE_SCALE)
#define LVL_GAME_REMOVECARD_HEIGHT      (LVL_INTER_CARD_HEIGHT * LVL_GAME_REMOVE_SCALE)
#define LVL_GAME_REMOVECARD_POS         (QPoint((LVL_GAME_WINDOW_WIDTH - LVL_GAME_REMOVECARD_WIDTH * LVL_GAME_SEEN_REMOVE_CARD_NUM) / 2,\
                                        LVL_GAME_WINDOW_HEIGHT / 2 - LVL_GAME_DECK_SHIFT_X))
///////�����ƶѵļ��β���
///////�����ƽ���ļ��β���
#define LVL_GAME_GUESSER_WIDTH          (800)
#define LVL_GAME_GUESSER_HEIGHT         (600)
#define LVL_GAME_GUESSER_POS            (QPoint((LVL_GAME_WINDOW_WIDTH - LVL_GAME_GUESSER_WIDTH) / 2, (LVL_GAME_WINDOW_HEIGHT - LVL_GAME_GUESSER_HEIGHT) / 2))
#define LVL_GAME_GUEESER_MARGIN         (100)
#define LVL_GAME_GUESSER_COLUMN         (4)
#define LVL_GAME_GUESSER_CONFIRM_SIZE   (QSize(120, 120))
///////�����ƽ���ļ��β���
///////�������ͷ��ļ��β���
#define LVL_GAME_HUMAN_PORT_WIDTH       (180)
#define LVL_GAME_HUMAN_PORT_HEIGHT      (180)
#define LVL_GAME_HUMAN_PORT_GEO         (QRect(LVL_GAME_WINDOW_WIDTH - LVL_GAME_HUMAN_PORT_WIDTH - LVL_GAME_RIGHT_MARGIN * 5,\
                                        LVL_GAME_WINDOW_HEIGHT - LVL_GAME_HUMAN_PORT_HEIGHT - LVL_GAME_VERT_MARGIN,\
                                        LVL_GAME_HUMAN_PORT_WIDTH,  LVL_GAME_HUMAN_PORT_HEIGHT))
#define LVL_GAME_AI_PORT_WIDTH          (120)
#define LVL_GAME_AI_PORT_HEIGHT         (120)
#define LVL_GAME_AI1_PORT_GEO           (QRect(LVL_GAME_LEFT_MARGIN,\
                                        (LVL_GAME_WINDOW_HEIGHT - LVL_INTER_CARD_HEIGHT) / 2 - LVL_GAME_AI_PORT_HEIGHT - LVL_INTER_CARD_SHIFT,\
                                        LVL_GAME_AI_PORT_WIDTH,  LVL_GAME_AI_PORT_HEIGHT))
#define LVL_GAME_AI2_PORT_GEO           (QRect(LVL_GAME_WINDOW_WIDTH / 2 - LVL_INTER_CARD_WIDTH - LVL_GAME_AI_PORT_WIDTH,\
                                        LVL_GAME_VERT_MARGIN,\
                                        LVL_GAME_AI_PORT_WIDTH,  LVL_GAME_AI_PORT_HEIGHT))
#define LVL_GAME_AI3_PORT_GEO           (QRect(LVL_GAME_WINDOW_WIDTH - LVL_GAME_AI_PORT_WIDTH - LVL_GAME_RIGHT_MARGIN,\
                                        (LVL_GAME_WINDOW_HEIGHT - LVL_INTER_CARD_HEIGHT) / 2 - LVL_GAME_AI_PORT_HEIGHT - LVL_INTER_CARD_SHIFT,\
                                        LVL_GAME_AI_PORT_WIDTH,  LVL_GAME_AI_PORT_HEIGHT))
///////�������ͷ��ļ��β���
///////����������������ƶѵļ��β���
#define LVL_GAME_HUMAN_SLOT            (QPoint(LVL_GAME_WINDOW_WIDTH / 2 - LVL_INTER_CARD_WIDTH, LVL_GAME_WINDOW_HEIGHT - LVL_GAME_VERT_MARGIN - LVL_INTER_CARD_HEIGHT))
#define LVL_GAME_HUMAN_DISCARD         (LVL_GAME_HUMAN_SLOT + QPoint(LVL_INTER_CARD_WIDTH * 2, LVL_INTER_CARD_HEIGHT * (1 - LVL_GAME_DISCARD_SCALE)))
#define LVL_GAME_AI1_SLOT              (QPoint(LVL_GAME_LEFT_MARGIN, (LVL_GAME_WINDOW_HEIGHT - LVL_INTER_CARD_HEIGHT) / 2))
#define LVL_GAME_AI1_DISCARD           (LVL_GAME_AI1_SLOT + QPoint(0, LVL_INTER_CARD_HEIGHT))
#define LVL_GAME_AI2_SLOT              (QPoint(LVL_GAME_WINDOW_WIDTH / 2 - LVL_INTER_CARD_WIDTH, LVL_INTER_CARD_SHIFT + LVL_GAME_VERT_MARGIN))
#define LVL_GAME_AI2_DISCARD           (LVL_GAME_AI2_SLOT + QPoint(LVL_INTER_CARD_WIDTH * 2, 0))
#define LVL_GAME_AI3_SLOT              (QPoint(LVL_GAME_WINDOW_WIDTH - LVL_INTER_CARD_WIDTH * 2 - LVL_GAME_RIGHT_MARGIN, (LVL_GAME_WINDOW_HEIGHT - LVL_INTER_CARD_HEIGHT) / 2))
#define LVL_GAME_AI3_DISCARD           (LVL_GAME_AI3_SLOT + QPoint(0, LVL_INTER_CARD_HEIGHT))
///////����������������ƶѵļ��β���

class LVL_GamePlayer;

////////////////////////////////////////////////////////////
//                      ��Ϸ����                          //
////////////////////////////////////////////////////////////

class LVL_GameSetting {
public:
    ~LVL_GameSetting() {}

    static LVL_GameSetting& LVL_SystemGetSettings() {
        static LVL_GameSetting stGlobalSetting;
        return stGlobalSetting;
    }

    template<typename T>
    T LVL_GameReadSetting(const QString &qstrName) const {
        if (!qmSettings.contains(qstrName) || !qmSettings[qstrName].canConvert<T>()) return 0;
        return qmSettings[qstrName].value<T>();
    }

    template<typename T>
    void LVL_GameWriteSetting(const QString &qstrName, const T &tValue) {
        if (!qmSettings.contains(qstrName) || !qmSettings[qstrName].canConvert<T>()) return;
        qmSettings[qstrName].setValue<T>(tValue);
    }

private:
    QMap<QString, QVariant> qmSettings;

    //���캯��
    LVL_GameSetting() : qmSettings() {
        qmSettings.insert("Player Number", (int)LVL_GAME_MAXIMUM_PLAYER_NUM);
    }
    LVL_GameSetting(const LVL_GameSetting&) = delete;
    LVL_GameSetting& operator=(const LVL_GameSetting&) = delete;
};

////////////////////////////////////////////////////////////
//                      ������                            //
////////////////////////////////////////////////////////////

class LVL_GameGuesser : public QWidget {
    Q_OBJECT

public:
    LVL_GameGuesser(QWidget *parent = Q_NULLPTR, const QPoint &qpPos = { 0, 0 }, const QSize &qsSize = { LVL_GAME_GUESSER_WIDTH, LVL_GAME_GUESSER_HEIGHT });//���캯��

private:
    LVL_InterCard::LVL_InterCardType_E enGuess;//��Ҫ�µ��Ƶ���

signals:
    void sgnGuessFinished(LVL_InterCard::LVL_InterCardType_E);//��ʾ���ƽ���
};

////////////////////////////////////////////////////////////
//                     �ƶѹ�����                         //
////////////////////////////////////////////////////////////

class LVL_GameDeckManager : public QWidget {
    Q_OBJECT

public:
    const static LVL_InterCard::LVL_InterCardType_E m_aenCardSet[LVL_GAME_TOTAL_CARD_NUM];//���������п���

    LVL_GameDeckManager(QWidget *parent = Q_NULLPTR);//���캯��
    int LVL_GameMakeDeal(LVL_GamePlayer *pstPlayer, LVL_InterCard *pstCard, bool bDealUnseen);//���з��Ʋ���

protected:
    LVL_InterCard::LVL_InterCardType_E aenDrawPile[LVL_GAME_TOTAL_CARD_NUM];//���ƶ�
    QLabel *pqlRest;//��ʾʣ������������
    int sCurCard;//��һ��Ҫ������
};

////////////////////////////////////////////////////////////
//                      ��Ϸ������                        //
////////////////////////////////////////////////////////////

class LVL_GameCtrller : public QWidget {
    Q_OBJECT

public:
    LVL_GameCtrller(QWidget *parent = Q_NULLPTR);//���캯��

    LVL_GamePlayer *LVL_GameGetTarget(LVL_InterCard::LVL_InterCardType_E enSelect, LVL_GamePlayer *pstSelf);//Ϊ���ƻ�ȡһ�����Ŀ��
    void LVL_GameTriggerEffect(LVL_InterCard::LVL_InterCardType_E enType, LVL_GamePlayer* pstSelf, LVL_GamePlayer *pstTarget);//�������Ƽ���

protected:
    void closeEvent(QCloseEvent *event);

private:
    LVL_GamePlayer *apstPlayers[LVL_GAME_MAXIMUM_PLAYER_NUM];//���
    LVL_GameGuesser *pstGuesser;//������
    LVL_GameDeckManager *pstDeckManager;//�ƶѹ�����
    bool bIsProcessing;//�Ƿ���ִ�п���Ч��

private slots:
    void LVL_GameCalculatePoint();//��Ϸ����ʱ���н���

signals:
    void sgnNoProcessing();//�źź���������Ч��ִ�����
};