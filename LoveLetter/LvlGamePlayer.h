////////////////////////////////////////////////
//File Name           : LvlGamePlayer.h
//2nd Model Name      : Game
//Description         : Define the Players.
//Lastest Update Time : 2021/2/14 16:36
//Version             : 1.2
////////////////////////////////////////////////

#pragma once

#include <QMessageBox>
#include <QTimer>
#include <QState>
#include <QStateMachine>
#include "LvlInterObject.h"

class LVL_GameCtrller;

////////////////////////////////////////////////////////////
//                       ��һ���                         //
////////////////////////////////////////////////////////////

class LVL_GamePlayer : public QWidget {
    Q_OBJECT

public:
    enum { HandCardNum = 2 };
    enum LVL_GamePlayerState_E { Normal, Guarded, Out };//�������״̬

    LVL_GamePlayer(LVL_GameCtrller *parent = Q_NULLPTR, bool bKnown = true, const QPoint &qpSlot = { 0, 0 }, const QPoint &qpDiscard = { 0, 0 },
        const QSize &qsCardSize = { LVL_INTER_CARD_WIDTH, LVL_INTER_CARD_HEIGHT }, int sCardSpace = LVL_INTER_CARD_SPACE);//���캯��

    inline void LVL_GameSetPortrait(const QString &qstrName = "", const QRect &qrPos = QRect(0, 0, 200, 200));//�������ͷ��
    inline void LVL_GameSetPortraitPic(const QString &qstrDefaultPic, const QString &qstrHoverPic, const QString &qstrPressedPic);//�������ͷͼ
    inline void LVL_GameSetDefaultPortraitPic();//�����ͷͼ����ΪĬ��ͷͼ
    inline LVL_GamePlayerState_E LVL_GameGetPlayerState() const;//��ȡ��ҵ�ǰ״̬
    inline void LVL_GameKickPlayer();//ʹ��ҳ���
    inline void LVL_GameGuardPlayer();//�������
    inline void LVL_GameUnguardPlayer();//�������
    inline void LVL_GameSetCard(LVL_InterCard *pstCard, const LVL_InterCard::LVL_InterCardType_E &enType);//���ÿ�����ĳ�ſ�������
    void LVL_GameMakeDiscard(LVL_InterCard *pstCard);//��������
    void LVL_GamePlayCard();//���һ�ſ���

protected:
    static int sActivePlayerNum;//�ڳ��������

    LVL_GamePlayerState_E enState;//���״̬
    LVL_InterBtn *pstPortrait;//���ͷ��
    QString qstrPortraitName;//���ͷ������
    LVL_InterCard *pstOldCard;//�ɿ�����
    LVL_InterCard *pstNewCard;//�¿�����
    QPoint qpDiscardPilePos;//���ƶ����Ͻ�λ��
    int sDiscardPileSize, sDiscardPilePoint;//���ƶ������������ܵ���
    bool bIsOwnRound;//�Ƿ����Լ��غ���

    LVL_InterCard *pstSelect;//ѡ����
    LVL_GamePlayer *pstTarget;//ѡ����Ч������Ŀ��

signals:
    void sgnRoundStarted();//�źź����������غϿ�ʼ
    void sgnRoundEnded();//�źź����������غϽ���
    void sgnPortraitClicked();//�źź���������ͷ�񱻵��
    void sgnWantDeal(LVL_InterCard*);//�źź�������Ҫ����
    void sgnTarget(LVL_GamePlayer*);//�źź���������ѡ��Ч��Ŀ��
    void sgnUntarget(LVL_GamePlayer*);//�źź���������ȡ��ѡ��Ч��Ŀ��

public slots:
    void LVL_GameStartRound();//�غϿ�ʼ
    void LVL_GameEndRound() { bIsOwnRound = false;  emit sgnRoundEnded(); }//�غϽ���

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
    setMask(mask() - pstPortrait->geometry());
    pstPortrait->setGeometry(qrPos);
    setMask(mask() + pstPortrait->geometry());
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
     ����ֵ��LVL_GamePlayerState_E-NormalΪ�����ڳ���GuardedΪ��������OutΪ����
 */
inline LVL_GamePlayer::LVL_GamePlayerState_E LVL_GamePlayer::LVL_GameGetPlayerState() const { return enState; }

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
     ���ܣ������ұ���״̬
     ��������
     ����ֵ����
*/
inline void LVL_GamePlayer::LVL_GameUnguardPlayer() {
    LVL_GameSetDefaultPortraitPic();//��ͷͼ����ΪĬ��
    enState = Normal;//�������״̬
}

/*
     ���ܣ����������������ĳ�ſ�������
     ������pstCard-��Ҫ���õĿ��Ƶ�ָ�룻enType-��Ҫ���õ����ࣻ��pstCardΪNULL����ʲôҲ����
     ����ֵ����
*/
inline void LVL_GamePlayer::LVL_GameSetCard(LVL_InterCard *pstCard, const LVL_InterCard::LVL_InterCardType_E &enType) {
    if (NULL == pstCard) exit(-1);
    pstCard->LVL_InterSetCardType(enType);
}

////////////////////////////////////////////////////////////
//                      ���������                        //
////////////////////////////////////////////////////////////

class LVL_GameHuman : public LVL_GamePlayer {
    Q_OBJECT
public:
    LVL_GameHuman(LVL_GameCtrller *parent = Q_NULLPTR, const QPoint &qpSlot = { 0, 0 }, const QPoint &qpDiscard = { 0, 0 },
        const QSize &qsCardSize = { LVL_INTER_CARD_WIDTH, LVL_INTER_CARD_HEIGHT }, int sCardSpace = LVL_INTER_CARD_SPACE);

signals:
    void sgnSelect();//�źź���������ѡ��
    void sgnUnselect();//�źź���������ȡ��ѡ��

public slots:
    void LVL_GameClickCard();//���һ�ſ���
    void LVL_GameClickPortrait();//���������ҵ�ͷ��
};

////////////////////////////////////////////////////////////
//                      ���������                        //
////////////////////////////////////////////////////////////

class LVL_GameAI : public LVL_GamePlayer {
    Q_OBJECT

public:
    LVL_GameAI(LVL_GameCtrller *parent = Q_NULLPTR, const QPoint &qpSlot = { 0, 0 }, const QPoint &qpDiscard = { 0, 0 },
        const QSize &qsCardSize = { LVL_INTER_CARD_WIDTH, LVL_INTER_CARD_HEIGHT }, int sCardSpace = LVL_INTER_CARD_SPACE);

signals:
    void sgnFinish();//�źź����������ȴ�����

private slots:
    void LVL_GameAIGetReady();//�ۺ������ȴ�ѡ��
};