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
//                       玩家基类                         //
////////////////////////////////////////////////////////////

class LVL_GamePlayer : public QWidget {
    Q_OBJECT

public:
    enum { HandCardNum = 2 };
    enum LVL_GamePlayerState_E { Normal, Guarded, Out };//所有玩家状态

    LVL_GamePlayer(LVL_GameCtrller *parent = Q_NULLPTR, bool bKnown = true, const QPoint &qpSlot = { 0, 0 }, const QPoint &qpDiscard = { 0, 0 },
        const QSize &qsCardSize = { LVL_INTER_CARD_WIDTH, LVL_INTER_CARD_HEIGHT }, int sCardSpace = LVL_INTER_CARD_SPACE);//构造函数

    inline void LVL_GameSetPortrait(const QString &qstrName = "", const QRect &qrPos = QRect(0, 0, 200, 200));//设置玩家头像
    inline void LVL_GameSetPortraitPic(const QString &qstrDefaultPic, const QString &qstrHoverPic, const QString &qstrPressedPic);//设置玩家头图
    inline void LVL_GameSetDefaultPortraitPic();//将玩家头图设置为默认头图
    inline LVL_GamePlayerState_E LVL_GameGetPlayerState() const;//获取玩家当前状态
    inline void LVL_GameKickPlayer();//使玩家出局
    inline void LVL_GameGuardPlayer();//保护玩家
    inline void LVL_GameUnguardPlayer();//解除保护
    inline void LVL_GameSetCard(LVL_InterCard *pstCard, const LVL_InterCard::LVL_InterCardType_E &enType);//设置卡组中某张卡牌种类
    void LVL_GameMakeDiscard(LVL_InterCard *pstCard);//弃置手牌
    void LVL_GamePlayCard();//打出一张卡牌

protected:
    static int sActivePlayerNum;//在场玩家数量

    LVL_GamePlayerState_E enState;//玩家状态
    LVL_InterBtn *pstPortrait;//玩家头像
    QString qstrPortraitName;//玩家头像名字
    LVL_InterCard *pstOldCard;//旧卡卡槽
    LVL_InterCard *pstNewCard;//新卡卡槽
    QPoint qpDiscardPilePos;//弃牌堆左上角位置
    int sDiscardPileSize, sDiscardPilePoint;//弃牌堆牌数和弃牌总点数
    bool bIsOwnRound;//是否在自己回合内

    LVL_InterCard *pstSelect;//选择卡牌
    LVL_GamePlayer *pstTarget;//选择卡牌效果作用目标

signals:
    void sgnRoundStarted();//信号函数，表明回合开始
    void sgnRoundEnded();//信号函数，表明回合结束
    void sgnPortraitClicked();//信号函数，表明头像被点击
    void sgnWantDeal(LVL_InterCard*);//信号函数，需要发牌
    void sgnTarget(LVL_GamePlayer*);//信号函数，表明选择效果目标
    void sgnUntarget(LVL_GamePlayer*);//信号函数，表明取消选择效果目标

public slots:
    void LVL_GameStartRound();//回合开始
    void LVL_GameEndRound() { bIsOwnRound = false;  emit sgnRoundEnded(); }//回合结束

    friend class LVL_GameCtrller;
};

/*
     功能：设置玩家头像
     内容：设置头像名字，并将鼠标悬停时的头图和按下时的头图默认设置为qstrName_focused和qstrName_clicked
     参数：qstrName-头像名字，同时也是默认头图；qrPos-头像位置
     返回值：无
*/
inline void LVL_GamePlayer::LVL_GameSetPortrait(const QString &qstrName, const QRect &qrPos) {
    qstrPortraitName = qstrName;
    pstPortrait->LVL_InterSetBtnPic(qstrName, qstrName + LVL_PIC_FOCUSED, qstrName + LVL_PIC_FOCUSED);
    setMask(mask() - pstPortrait->geometry());
    pstPortrait->setGeometry(qrPos);
    setMask(mask() + pstPortrait->geometry());
}

/*
    功能：设置玩家头图
    参数：qstrDefaultPic-默认图片；qstrHoverPic-鼠标悬停时的图片；qstrPressedPic-鼠标按下时的图片
    返回值：无
*/
inline void LVL_GamePlayer::LVL_GameSetPortraitPic(const QString &qstrDefaultPic, const QString &qstrHoverPic, const QString &qstrPressedPic) {
    pstPortrait->LVL_InterSetBtnPic(qstrDefaultPic, qstrHoverPic, qstrPressedPic);
};

/*
     功能：将玩家头图设置为默认头图
     参数：无
     返回值：无
 */
inline void LVL_GamePlayer::LVL_GameSetDefaultPortraitPic() {
    LVL_GameSetPortraitPic(qstrPortraitName, qstrPortraitName + LVL_PIC_FOCUSED, qstrPortraitName + LVL_PIC_FOCUSED);
}

/*
     功能：获取玩家当前状态
     参数：无
     返回值：LVL_GamePlayerState_E-Normal为正常在场，Guarded为被保护；Out为出局
 */
inline LVL_GamePlayer::LVL_GamePlayerState_E LVL_GamePlayer::LVL_GameGetPlayerState() const { return enState; }

/*
     功能：使玩家出局
     参数：无
     返回值：无
*/
inline void LVL_GamePlayer::LVL_GameKickPlayer() {
    pstPortrait->LVL_InterSetBtnPic(qstrPortraitName + LVL_PIC_OUT, qstrPortraitName + LVL_PIC_OUT, qstrPortraitName + LVL_PIC_OUT);//更改头图
    enState = Out;//设置状态为出局
    pstOldCard->LVL_InterSetCardKnown(true);//展示手牌
    sActivePlayerNum--;//在场玩家数减一
}

/*
     功能：保护玩家
     参数：无
     返回值：无
*/
inline void LVL_GamePlayer::LVL_GameGuardPlayer() {
    pstPortrait->LVL_InterSetBtnPic(qstrPortraitName + LVL_PIC_GUARDED, qstrPortraitName + LVL_PIC_GUARDED, qstrPortraitName + LVL_PIC_GUARDED);//更改头图
    enState = Guarded;//设置状态为被保护
}

/*
     功能：解除玩家保护状态
     参数：无
     返回值：无
*/
inline void LVL_GamePlayer::LVL_GameUnguardPlayer() {
    LVL_GameSetDefaultPortraitPic();//将头图更改为默认
    enState = Normal;//解除保护状态
}

/*
     功能：设置玩家手牌组中某张卡牌种类
     参数：pstCard-需要设置的卡牌的指针；enType-需要设置的种类；若pstCard为NULL，则什么也不做
     返回值：无
*/
inline void LVL_GamePlayer::LVL_GameSetCard(LVL_InterCard *pstCard, const LVL_InterCard::LVL_InterCardType_E &enType) {
    if (NULL == pstCard) exit(-1);
    pstCard->LVL_InterSetCardType(enType);
}

////////////////////////////////////////////////////////////
//                      人类玩家类                        //
////////////////////////////////////////////////////////////

class LVL_GameHuman : public LVL_GamePlayer {
    Q_OBJECT
public:
    LVL_GameHuman(LVL_GameCtrller *parent = Q_NULLPTR, const QPoint &qpSlot = { 0, 0 }, const QPoint &qpDiscard = { 0, 0 },
        const QSize &qsCardSize = { LVL_INTER_CARD_WIDTH, LVL_INTER_CARD_HEIGHT }, int sCardSpace = LVL_INTER_CARD_SPACE);

signals:
    void sgnSelect();//信号函数，表明选卡
    void sgnUnselect();//信号函数，表明取消选卡

public slots:
    void LVL_GameClickCard();//点击一张卡牌
    void LVL_GameClickPortrait();//点击其它玩家的头像
};

////////////////////////////////////////////////////////////
//                      电脑玩家类                        //
////////////////////////////////////////////////////////////

class LVL_GameAI : public LVL_GamePlayer {
    Q_OBJECT

public:
    LVL_GameAI(LVL_GameCtrller *parent = Q_NULLPTR, const QPoint &qpSlot = { 0, 0 }, const QPoint &qpDiscard = { 0, 0 },
        const QSize &qsCardSize = { LVL_INTER_CARD_WIDTH, LVL_INTER_CARD_HEIGHT }, int sCardSpace = LVL_INTER_CARD_SPACE);

signals:
    void sgnFinish();//信号函数，表明等待结束

private slots:
    void LVL_GameAIGetReady();//槽函数，等待选择
};