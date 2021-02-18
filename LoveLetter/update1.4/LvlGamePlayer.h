////////////////////////////////////////////////
//File Name           : LVLGamePlayer.h
//2nd Model Name      : Game
//Description         : Define the Player.
//Lastest Update Time : 2020/12/30 12:35
//Version             : 1.1
////////////////////////////////////////////////

#pragma once

#include <QtWidgets/QMainWindow>
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QTimer>
#include <QState>
#include <QStateMachine>
#include "ui_LVLGameMain.h"
#include "LvlInterObject.h"
#include "LvlGameMain.h"

///////////////////////////////////////////////////////////////
//                       ��һ���                            //
///////////////////////////////////////////////////////////////

class LVL_GamePlayer : public QWidget {
    Q_OBJECT

public:
    enum { HandCardNum = 2 };
    enum LVL_GamePlayerState { Normal, Guarded, Out };//�������״̬

    LVL_GamePlayer(LVL_GameCtrller *parent = Q_NULLPTR, bool bKnown = true, const QPoint &qpSlot = { 0, 0 }, const QPoint &qpDiscard = { 0, 0 },
        int sCardWidth = LVL_INTER_CARD_WIDTH, int sCardHeight = LVL_INTER_CARD_HEIGHT, int sCardSpace = LVL_INTER_CARD_SPACE);//���캯��

    inline void LVL_GameSetPortrait(const QString &qstrName = "", const QRect &qrPos = QRect(0, 0, 200, 200));//�������ͷ��
    inline void LVL_GameSetPortraitPic(const QString &qstrDefaultPic, const QString &qstrHoverPic, const QString &qstrPressedPic);//�������ͷͼ
    inline void LVL_GameSetDefaultPortraitPic();//�����ͷͼ����ΪĬ��ͷͼ
    inline LVL_GamePlayerState LVL_GameGetPlayerState() const;//��ȡ��ҵ�ǰ״̬
    inline void LVL_GameKickPlayer();//ʹ��ҳ���
    inline void LVL_GameGuardPlayer();//�������
    inline void LVL_GameUnguardPlayer();//�������
    inline void LVL_GameSetCard(LVL_InterCard *pstCard, const LVL_InterCard::LVL_InterCardType_E &enType);//���ÿ�����ĳ�ſ�������
    void LVL_GameMakeDiscard(LVL_InterCard *pstCard);//��������
    void LVL_GamePlayCard();//���һ�ſ���

protected:
    LVL_GamePlayerState enState;//���״̬
    LVL_InterBtn *pstPortrait;//���ͷ��
    QString qstrPortraitName;//���ͷ������
    LVL_InterCard *pstOldCard;//�ɿ�����
    LVL_InterCard *pstNewCard;//�¿�����
    QPoint qpDiscardPilePos;//���ƶ����Ͻ�λ��
    int sDiscardPileSize, sDiscardPilePoint;//���ƶ������������ܵ���
    bool bIsOwnRound;//�Ƿ����Լ��غ���

    LVL_InterCard *pstSelect;//ѡ����
    LVL_GamePlayer *pstTarget;//ѡ����Ч������Ŀ��
    QStateMachine *qsmCardCtrl;//ѡ��״̬��
    static int sActivePlayerNum;//�ڳ��������

signals:
    void sgnActive();//�źź���������������
    void sgnDeactive();//�źź���������δ����
    void sgnGameEnd();//�źź������������ƶ���գ���Ϸ����
    void sgnPortraitClicked();//�źź���������ͷ�񱻵��
    void sgnTarget(LVL_GamePlayer*);//�źź���������ѡ��Ч��Ŀ��
    void sgnUntarget(LVL_GamePlayer*);//�źź���������ȡ��ѡ��Ч��Ŀ��

public slots:
    void LVL_GameActivateHand();//�غϿ�ʼ
    void LVL_GameDeactivateHand() { bIsOwnRound = false;  emit sgnDeactive(); }//�غϽ���

    friend class LVL_GameCtrller;
};

/*
     ���ܣ��������ͷ��
     ���ݣ�����ͷ�����֣����������ͣʱ��ͷͼ�Ͱ���ʱ��ͷͼĬ������ΪqstrName_focused��qstrName_clicked
     ������qstrName-ͷ�����֣�ͬʱҲ��Ĭ��ͷͼ��qrPos-ͷ��λ��
     ����ֵ����
*/
inline void LVL_GamePlayer::LVL_GameSetPortrait(const QString &qstrName, const QRect &qrPos) {
    qstrPortraitName = qstrName;
    pstPortrait->LVL_InterSetBtnPic(qstrName, qstrName + LVL_PIC_FOCUSED, qstrName + LVL_PIC_FOCUSED);
    pstPortrait->setGeometry(qrPos);
}

/*
    ���ܣ��������ͷͼ
    ������qstrDefaultPic-Ĭ��ͼƬ��qstrHoverPic-�����ͣʱ��ͼƬ��qstrPressedPic-��갴��ʱ��ͼƬ
    ����ֵ����
*/
inline void LVL_GamePlayer::LVL_GameSetPortraitPic(const QString &qstrDefaultPic, const QString &qstrHoverPic, const QString &qstrPressedPic) {
    pstPortrait->LVL_InterSetBtnPic(qstrDefaultPic, qstrHoverPic, qstrPressedPic);
};

/*
     ���ܣ������ͷͼ����ΪĬ��ͷͼ
     ��������
     ����ֵ����
 */
inline void LVL_GamePlayer::LVL_GameSetDefaultPortraitPic() {
    LVL_GameSetPortraitPic(qstrPortraitName, qstrPortraitName + LVL_PIC_FOCUSED, qstrPortraitName + LVL_PIC_FOCUSED);
}

/*
     ���ܣ���ȡ��ҵ�ǰ״̬
     ��������
     ����ֵ��LVL_GamePlayerState-NormalΪ�����ڳ���GuardedΪ��������OutΪ����
 */
inline LVL_GamePlayer::LVL_GamePlayerState LVL_GamePlayer::LVL_GameGetPlayerState() const { return enState; }

/*
     ���ܣ�ʹ��ҳ���
     ��������
     ����ֵ����
*/
inline void LVL_GamePlayer::LVL_GameKickPlayer() {
    pstPortrait->LVL_InterSetBtnPic(qstrPortraitName + LVL_PIC_OUT, qstrPortraitName + LVL_PIC_OUT, qstrPortraitName + LVL_PIC_OUT);//����ͷͼ
    enState = Out;//����״̬Ϊ����
    pstOldCard->LVL_InterSetCardKnown(true);//չʾ����
    sActivePlayerNum--;//�ڳ��������һ
}

/*
     ���ܣ��������
     ��������
     ����ֵ����
*/
inline void LVL_GamePlayer::LVL_GameGuardPlayer() {
    pstPortrait->LVL_InterSetBtnPic(qstrPortraitName + LVL_PIC_GUARDED, qstrPortraitName + LVL_PIC_GUARDED, qstrPortraitName + LVL_PIC_GUARDED);//����ͷͼ
    enState = Guarded;//����״̬Ϊ������
}

/*
     ���ܣ��������״̬
     ��������
     ����ֵ����
*/
inline void LVL_GamePlayer::LVL_GameUnguardPlayer() {
    LVL_GameSetDefaultPortraitPic();//��ͷͼ����ΪĬ��
    enState = Normal;//�������״̬
}

/*
     ���ܣ����ÿ�����ĳ�ſ�������
     ������pstCard-��Ҫ���õĿ��ƣ�enType-��Ҫ���õ����ࣻ��pstCardΪNULL����ʲôҲ����
     ����ֵ����
*/
inline void LVL_GamePlayer::LVL_GameSetCard(LVL_InterCard *pstCard, const LVL_InterCard::LVL_InterCardType_E &enType) {
    if (NULL != pstCard) pstCard->LVL_InterSetCardType(enType);
}

///////////////////////////////////////////////////////////////
//                      ���������                           //
///////////////////////////////////////////////////////////////

class LVL_GameHuman : public LVL_GamePlayer {
    Q_OBJECT
public:
    LVL_GameHuman(LVL_GameCtrller *parent = Q_NULLPTR, const QPoint &qpSlot = { 0, 0 }, const QPoint &qpDiscard = { 0, 0 },
        int sCardWidth = LVL_INTER_CARD_WIDTH, int sCardHeight = LVL_INTER_CARD_HEIGHT, int sCardSpace = LVL_INTER_CARD_SPACE);

signals:
    void sgnSelect();//�źź���������ѡ��
    void sgnUnselect();//�źź���������ȡ��ѡ��

public slots:
    void LVL_GameClickCard();//���һ�ſ���
    void LVL_GameClickPortrait();//���������ҵ�ͷ��
};

///////////////////////////////////////////////////////////////
//                      ���������                           //
///////////////////////////////////////////////////////////////

class LVL_GameAI : public LVL_GamePlayer {
    Q_OBJECT

public:
    LVL_GameAI(LVL_GameCtrller *parent = Q_NULLPTR, const QPoint &qpSlot = { 0, 0 }, const QPoint &qpDiscard = { 0, 0 },
        int sCardWidth = LVL_INTER_CARD_WIDTH, int sCardHeight = LVL_INTER_CARD_HEIGHT, int sCardSpace = LVL_INTER_CARD_SPACE);

signals:
    void sgnFinish();//�źź����������ȴ�����

private slots:
    void LVL_GameAIGetReady();//�ۺ������ȴ�ѡ��
};