////////////////////////////////////////////////
//File Name           : LVLGamePlayer.cpp
//2nd Model Name      : Game
//Description         : Implement the Player.
//Lastest Update Time : 2020/12/30 12:35
//Version             : 1.1
////////////////////////////////////////////////

#include <QMessageBox>
#include <QPropertyAnimation.h>
#include "LvlGamePlayer.h"

int LVL_GamePlayer::sActivePlayerNum = 0;//初始在场玩家数为0

/*
    功能：玩家基类构造函数
    参数：parent-玩家所属窗口的指针；qpSlot-卡槽左上角坐标；qpDiscard-弃牌堆左上角坐标；sCard{Width, Height}-卡牌大小；sCardSpace-两张卡牌的间隔
*/
LVL_GamePlayer::LVL_GamePlayer(LVL_GameCtrller *parent, bool bKnown, const QPoint &qpSlot, const QPoint &qpDiscard, int sCardWidth, int sCardHeight, int sCardSpace)
    : QWidget(parent), enState(Normal), bIsOwnRound(false), qsmCardCtrl(new QStateMachine(this)), sDiscardPileSize(0), sDiscardPilePoint(0), qpDiscardPilePos(qpDiscard)
    , pstOldCard(new LVL_InterCard(window(), LVL_InterCard::none, bKnown, qpSlot.x(), qpSlot.y(), sCardWidth, sCardHeight))
    , pstNewCard(new LVL_InterCard(window(), LVL_InterCard::none, bKnown, qpSlot.x() + sCardWidth + sCardSpace, qpSlot.y(), sCardWidth, sCardHeight))
    , pstSelect(NULL), pstTarget(NULL), pstPortrait(new LVL_InterBtn(window()))
{
    sActivePlayerNum++;
    setGeometry(0, 0, 0, 0);
    connect(pstPortrait, &QPushButton::clicked, [this] { if (Normal == enState) emit sgnPortraitClicked(); });
    dynamic_cast<LVL_GameCtrller*>(parentWidget())->LVL_GameMakeDeal(this, pstOldCard, false);
}

/*
    功能：弃置手牌
    内容：在弃牌堆显示弃牌，并更改手牌和弃牌堆状态
    参数：pstCard-要弃置的手牌的指针
    返回值：无
*/
void LVL_GamePlayer::LVL_GameMakeDiscard(LVL_InterCard *pstCard) {
    int sDisCardWidth = pstCard->width() * LVL_GAME_DISCARD_SCALE, sDisCardHeight = pstCard->height() * LVL_GAME_DISCARD_SCALE;
    LVL_InterCard *pstDiscard = new LVL_InterCard(window(), pstCard->LVL_InterGetCardType(), true,
        qpDiscardPilePos.x() + sDiscardPileSize * (sDisCardWidth / 4.0), qpDiscardPilePos.y(), sDisCardWidth, sDisCardHeight);
    pstDiscard->show();
    sDiscardPileSize++;//弃牌堆+1
    sDiscardPilePoint += pstCard->LVL_InterGetCardType();
}

/*
    功能：打出一张卡牌
    内容：在弃牌堆显示弃牌，发动卡牌技能，并更改手牌状态，卡牌和技能对象由pstSelect和pstTarget确定
    参数：无
    返回值：无
*/
void LVL_GamePlayer::LVL_GamePlayCard() {
    if (NULL == pstSelect) return;
    
    LVL_InterCard::LVL_InterCardType_E enType = pstSelect->enType;
    pstSelect->LVL_InterWithdrawCard();//收回卡牌
    emit sgnUntarget(pstTarget);//重置头图
    LVL_GameMakeDiscard(pstSelect);//显示弃牌
    if (pstOldCard == pstSelect) {//显示动画
        pstOldCard->LVL_InterSetCardType(pstNewCard->LVL_InterGetCardType());
        QPropertyAnimation *qpaMove = new QPropertyAnimation(pstOldCard, "geometry");
        qpaMove->setDuration(500);
        qpaMove->setStartValue(pstNewCard->geometry());
        qpaMove->setEndValue(pstOldCard->geometry());
        qpaMove->setEasingCurve(QEasingCurve::InOutQuad);
        qpaMove->start();
    }
    pstNewCard->LVL_InterSetCardType(LVL_InterCard::none);//将被出的卡牌置none

    dynamic_cast<LVL_GameCtrller*>(parent())->LVL_GameTriggerEffect(enType, this, pstTarget);//发动卡牌效果
    pstSelect = NULL;//取消卡牌选择
    pstTarget = NULL;//取消目标选择
}

/*
    功能：如果玩家出局则跳过回合，否则从发牌堆中抽牌并激活手牌区；若发牌堆无牌或仅有一人存活，则宣告游戏结束
    参数：无
    返回值：无
*/
void LVL_GamePlayer::LVL_GameActivateHand() {
    if (sActivePlayerNum == 1 || (Out != enState && 0 == dynamic_cast<LVL_GameCtrller*>(parentWidget())->LVL_GameMakeDeal(this, pstNewCard, false))) emit sgnGameEnd();
    else if (Out == enState) emit sgnDeactive();
    else {
        if (Guarded == enState) LVL_GameUnguardPlayer();
        bIsOwnRound = true;
        emit sgnActive();
    }
}

///////////////////////////////////////////////////////////////

/*
    功能：人类玩家构造函数
    参数：parent-玩家所属窗口的指针；qpSlot-卡槽左上角坐标；qpDiscard-弃牌堆左上角坐标；sCard{Width, Height}-卡牌大小；sCardSpace-两张卡牌的间隔
    内容：实现人类玩家出牌相关功能
*/
LVL_GameHuman::LVL_GameHuman(LVL_GameCtrller *parent, const QPoint &qpSlot, const QPoint &qpDiscard, int sCardWidth, int sCardHeight, int sCardSpace)
    : LVL_GamePlayer(parent, true, qpSlot, qpDiscard, sCardWidth, sCardHeight, sCardSpace)
{ 
    LVL_InterBtn *pstConfirm = new LVL_InterBtn(window(), QStringLiteral("确认出牌"), QRect(720, 600, 80, 20));
    pstConfirm->setProperty("visible", false);

    QState *qsDeactive = new QState(qsmCardCtrl);
    QState *qsNoSelect = new QState(qsmCardCtrl);
    QState *qsSelected = new QState(qsmCardCtrl);
    QState *qsReady = new QState(qsmCardCtrl);
    qsmCardCtrl->setInitialState(qsDeactive);

    qsDeactive->addTransition(this, &LVL_GamePlayer::sgnActive, qsNoSelect);
    qsNoSelect->addTransition(this, &LVL_GameHuman::sgnSelect, qsSelected);
    qsSelected->addTransition(this, &LVL_GameHuman::sgnUnselect, qsNoSelect);

    qsSelected->addTransition(this, &LVL_GameHuman::sgnTarget, qsReady);
    qsReady->addTransition(this, &LVL_GameHuman::sgnUntarget, qsSelected);
    qsReady->addTransition(this, &LVL_GameHuman::sgnUnselect, qsNoSelect);
    qsReady->addTransition(pstConfirm, &QPushButton::clicked, qsDeactive);

    connect(pstOldCard, &LVL_InterCard::sgnClicked, this, &LVL_GameHuman::LVL_GameClickCard);
    connect(pstNewCard, &LVL_InterCard::sgnClicked, this, &LVL_GameHuman::LVL_GameClickCard);
    connect(qsReady, &QState::entered, [pstConfirm] { pstConfirm->setProperty("visible", true); });
    connect(qsReady, &QState::exited, [pstConfirm] { pstConfirm->setProperty("visible", false); });
    connect(qsDeactive, &QState::entered, [this] { LVL_GamePlayCard(); LVL_GameDeactivateHand();});

    qsmCardCtrl->start();
}

/*
    功能：根据点击的卡牌更改出牌状态
    参数：无
    返回值：无
*/
void LVL_GameHuman::LVL_GameClickCard() {
    if (false == bIsOwnRound) return;//未到自己回合不能选牌
    
    //判定女伯爵技能
    LVL_InterCard* pstSender = dynamic_cast<LVL_InterCard*>(sender());
    if (LVL_InterCard::prince == pstSender->LVL_InterGetCardType() || LVL_InterCard::king == pstSender->LVL_InterGetCardType()) {
        LVL_InterCard* pstOther = (pstNewCard == pstSender) ? pstOldCard : pstNewCard;
        if (LVL_InterCard::countess == pstOther->LVL_InterGetCardType()) {
            QMessageBox::information(NULL, QStringLiteral("操作无效"), QStringLiteral("当你手上有王子或国王时，你只能打出女伯爵。"));
            return;
        }
    }

    if (NULL == pstSelect) emit sgnSelect();
    else {
        pstSelect->LVL_InterWithdrawCard();
        emit sgnUntarget(pstTarget);
        pstTarget = NULL;
    }
    if (pstSender == pstSelect) {
        pstSelect = NULL;
        emit sgnUnselect();
    } else {
        pstSender->LVL_InterPopCard();
        pstSelect = pstSender;
        if (NULL == dynamic_cast<LVL_GameCtrller*>(parent())->LVL_GameGetTarget(pstSelect->LVL_InterGetCardType(), this)) emit sgnTarget(NULL);//打出的卡牌没有可选目标或无需选择目标
    }
}

/*
    功能：根据点击的玩家更改目标选择状态
    参数：无
    返回值：无
*/
void LVL_GameHuman::LVL_GameClickPortrait() {
    if (false == bIsOwnRound || NULL == pstSelect || false == LVL_InterCard::m_bNeedTarget[pstSelect->LVL_InterGetCardType()]) return;//未到自己回合、未选牌或出牌无需目标时不能选择对象
    
    LVL_GamePlayer* pstSender = dynamic_cast<LVL_GamePlayer*>(sender());
    if (this == pstSender && LVL_InterCard::prince != pstSelect->LVL_InterGetCardType()) return;//除王子外不能选择自己作为目标
    if (NULL != pstTarget) emit sgnUntarget(pstTarget);
    if (pstSender == pstTarget) pstTarget = NULL;
    else { pstTarget = pstSender; emit sgnTarget(pstSender);}
}

///////////////////////////////////////////////////////////////

/*
    功能：AI玩家构造函数
    参数：parent-玩家所属窗口的指针；qpSlot-卡槽左上角坐标；qpDiscard-弃牌堆左上角坐标；sCard{Width, Height}-卡牌大小；sCardSpace-两张卡牌的间隔
    内容：实现人类玩家出牌相关功能
*/
LVL_GameAI::LVL_GameAI(LVL_GameCtrller *parent, const QPoint &qpSlot, const QPoint &qpDiscard, int sCardWidth, int sCardHeight, int sCardSpace)
    : LVL_GamePlayer(parent, false, qpSlot, qpDiscard, sCardWidth, sCardHeight, sCardSpace)
{
    connect(this, &LVL_GameAI::sgnActive, [this] { QTimer::singleShot(LVL_GAME_AI_WAIT_MSEC, this, &LVL_GameAI::LVL_GameAIGetReady); });
    connect(this, &LVL_GameAI::sgnFinish, [this] { QTimer::singleShot(LVL_GAME_AI_WAIT_MSEC, this, [this] {
        pstSelect->LVL_InterSetCardKnown(false);
        LVL_GamePlayCard();
        LVL_GameDeactivateHand();
    }); });
}

void LVL_GameAI::LVL_GameAIGetReady() {
    LVL_InterCard::LVL_InterCardType_E enOld = pstOldCard->LVL_InterGetCardType(), enNew = pstNewCard->LVL_InterGetCardType();
    if (LVL_InterCard::princess == enNew//避免出公主牌
        || LVL_InterCard::countess == enOld && (LVL_InterCard::prince == enNew || LVL_InterCard::king == enNew)) pstSelect = pstOldCard;//判定女伯爵效果
    else if (LVL_InterCard::princess == enOld//避免出公主牌
        || LVL_InterCard::countess == enNew && (LVL_InterCard::prince == enOld || LVL_InterCard::king == enOld)) pstSelect = pstNewCard;//判定女伯爵效果
    else pstSelect = (qrand() % HandCardNum) ? pstNewCard : pstOldCard;//若不生效，则随意选择卡牌
    pstSelect->LVL_InterPopCard();//弹出卡牌
    pstSelect->LVL_InterSetCardKnown(true);//使卡牌可视化
    pstTarget = dynamic_cast<LVL_GameCtrller*>(parent())->LVL_GameGetTarget(pstSelect->LVL_InterGetCardType(), this);//选择目标
    emit sgnTarget(pstTarget);//标记目标
    emit sgnFinish();
}