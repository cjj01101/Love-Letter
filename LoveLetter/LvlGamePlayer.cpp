////////////////////////////////////////////////
//File Name           : LvlGamePlayer.cpp
//2nd Model Name      : Game
//Description         : Implement the Players.
//Lastest Update Time : 2021/2/14 16:36
//Version             : 1.2
////////////////////////////////////////////////

#include "LvlGamePlayer.h"
#include "LvlGameCtrl.h"

int LVL_GamePlayer::sActivePlayerNum = 0;//初始在场玩家数为0

/*
    功能：玩家基类构造函数
    参数：parent-玩家所属窗口的指针；bKnown-玩家卡牌是否可见；qpSlot-卡槽左上角坐标；qpDiscard-弃牌堆左上角坐标；sCard{Width, Height}-卡牌大小；sCardSpace-两张卡牌的间隔
*/
LVL_GamePlayer::LVL_GamePlayer(LVL_GameCtrller *parent, bool bKnown, const QPoint &qpSlot, const QPoint &qpDiscard, const QSize &qsCardSize, int sCardSpace)
    : QWidget(parent), pstCtrller(parent), enState(LVL_GamePlayerState_E::Normal), bIsOwnRound(false), sDiscardPileSize(0), sDiscardPilePoint(0), qpDiscardPilePos(qpDiscard)
    , pstOldCard(new LVL_InterCard(parent, LVL_InterCardType_E::none, bKnown, qpSlot, qsCardSize))
    , pstNewCard(new LVL_InterCard(parent, LVL_InterCardType_E::none, bKnown, qpSlot + QPoint(qsCardSize.width() + sCardSpace, 0), qsCardSize))
    , pstSelect(NULL), pstTarget(NULL), pstPortrait(new LVL_InterBtn(parent))
{
    sActivePlayerNum++;//在场玩家数加一
    setGeometry(0, 0, 0, 0);
    connect(pstPortrait, &QPushButton::clicked, [this] { if (LVL_GamePlayerState_E::Normal == enState) emit sgnPortraitClicked(); });//头像点击操作
}

/*
     功能：使玩家出局
     参数：无
     返回值：无
*/
void LVL_GamePlayer::LVL_GameKickPlayer() {
    pstPortrait->LVL_InterSetBtnPic(qstrPortraitName + LVL_PIC_OUT, qstrPortraitName + LVL_PIC_OUT, qstrPortraitName + LVL_PIC_OUT);//更改头图
    enState = LVL_GamePlayerState_E::Out;//设置状态为出局
    pstOldCard->LVL_InterSetCardKnown(true);//展示手牌
    pstCtrller->LVL_GameAddDiscardEntry(this, pstOldCard->LVL_InterGetCardType());
    sActivePlayerNum--;//在场玩家数减一
}

/*
    功能：弃置手牌
    内容：在弃牌堆显示弃牌，并更改手牌和弃牌堆状态
    参数：pstCard-要弃置的手牌的指针
    返回值：无
*/
void LVL_GamePlayer::LVL_GameMakeDiscard(LVL_InterCard *pstCard) {
    if (NULL == pstCard) exit(-1);
    LVL_InterCardType_E enType = pstCard->LVL_InterGetCardType();
    QSize qsDisCardSize = { (int)(pstCard->width() * LVL_GAME_DISCARD_SCALE), (int)(pstCard->height() * LVL_GAME_DISCARD_SCALE) };
    LVL_InterCard *pstDiscard = new LVL_InterCard(parentWidget(), enType, true,
        qpDiscardPilePos + QPoint(sDiscardPileSize * (qsDisCardSize.width() / 4.0), 0), qsDisCardSize);
    pstDiscard->show();
    pstCtrller->LVL_GameAddDiscardEntry(this, enType);

    sDiscardPileSize++;
    sDiscardPilePoint += (int)enType;
}

/*
    功能：打出一张卡牌
    内容：在弃牌堆显示弃牌，发动卡牌技能，并更改手牌状态，卡牌和技能对象由pstSelect和pstTarget确定
    参数：无
    返回值：无
*/
void LVL_GamePlayer::LVL_GamePlayCard() {
    if (NULL == pstSelect) return;
    
    LVL_InterCardType_E enType = pstSelect->LVL_InterGetCardType();
    pstSelect->LVL_InterWithdrawCard();//收回卡牌
    QPoint qpOrigin = pstOldCard->pos();

    LVL_GameMakeDiscard(pstSelect);//显示弃牌
    if (pstOldCard == pstSelect) {//如果出的是旧牌，则将新牌变为旧牌
        pstOldCard->LVL_InterSetCardType(pstNewCard->LVL_InterGetCardType());
        pstOldCard->LVL_InterSetCardPos(pstNewCard->pos());
    }
    pstNewCard->LVL_InterSetCardType(LVL_InterCardType_E::none);//将被出的卡牌置none

    pstCtrller->LVL_GameTriggerEffect(enType, this, pstTarget);//发动卡牌效果
    emit sgnUntarget(pstTarget);//重置头图
    pstTarget = NULL;//取消目标选择

    if (pstOldCard == pstSelect) {
        QAbstractAnimation* pqaaMoving = pstOldCard->LVL_InterCreateAnimation("pos", LVL_GAME_MOVING_MSEC, pstOldCard->pos(), qpOrigin, QEasingCurve::InOutQuad);
        pqaaMoving->start(QAbstractAnimation::DeleteWhenStopped);
        QEventLoop qelLoop;
        connect(pqaaMoving, &QAbstractAnimation::finished, &qelLoop, &QEventLoop::quit);
        qelLoop.exec();
    }
    pstSelect = NULL;//取消卡牌选择
}

/*
    功能：如果玩家出局则跳过回合，否则从发牌堆中抽牌并激活手牌区；若发牌堆无牌或仅有一人存活，则宣告游戏结束
    参数：无
    返回值：无
*/
void LVL_GamePlayer::LVL_GameStartRound() {
    if (LVL_GamePlayerState_E::Out == enState) return emit sgnRoundEnded();
    emit sgnWantDeal(pstNewCard);
    if (LVL_InterCardType_E::none == pstNewCard->LVL_InterGetCardType()) return emit sgnRoundEnded();//没有得到发牌，结束回合
    if (LVL_GamePlayerState_E::Guarded == enState) LVL_GameUnguardPlayer();//若上一回合被保护，则取消保护
    bIsOwnRound = true;
    emit sgnRoundStarted();
}

///////////////////////////////////////////////////////////////

/*
    功能：人类玩家构造函数
    内容：实现人类玩家出牌相关功能
    参数：parent-玩家所属窗口的指针；qpSlot-卡槽左上角坐标；qpDiscard-弃牌堆左上角坐标；sCard{Width, Height}-卡牌大小；sCardSpace-两张卡牌的间隔
*/
LVL_GameHuman::LVL_GameHuman(LVL_GameCtrller *parent, const QPoint &qpSlot, const QPoint &qpDiscard, const QSize &qsCardSize, int sCardSpace)
    : LVL_GamePlayer(parent, true, qpSlot, qpDiscard, qsCardSize, sCardSpace)
{ 
    LVL_InterBtn *pstConfirm = new LVL_InterBtn(parent, "", LVL_GAME_CONFIRM_BTN_GEO);//确认出牌按钮
    pstConfirm->LVL_InterSetBtnPic(LVL_PIC_CONFIRM_BTN, LVL_PIC_CONFIRM_BTN + LVL_PIC_FOCUSED, LVL_PIC_CONFIRM_BTN + LVL_PIC_CLICKED);
    pstConfirm->setProperty("visible", false);

    QStateMachine *pqsmCardCtrl = new QStateMachine(this);
    pqsmCardCtrl->setGlobalRestorePolicy(QStateMachine::RestoreProperties);

    QState *pqsDeactive = new QState(pqsmCardCtrl);
    QState *pqsNoSelect = new QState(pqsmCardCtrl);
    QState *pqsSelected = new QState(pqsmCardCtrl);
    QState *pqsReady = new QState(pqsmCardCtrl);
    pqsmCardCtrl->setInitialState(pqsDeactive);

    pqsDeactive->addTransition(this, &LVL_GamePlayer::sgnRoundStarted, pqsNoSelect);
    pqsNoSelect->addTransition(this, &LVL_GameHuman::sgnSelect, pqsSelected);
    pqsSelected->addTransition(this, &LVL_GameHuman::sgnUnselect, pqsNoSelect);

    pqsSelected->addTransition(this, &LVL_GameHuman::sgnTarget, pqsReady);
    pqsReady->addTransition(this, &LVL_GameHuman::sgnUntarget, pqsSelected);
    pqsReady->addTransition(this, &LVL_GameHuman::sgnUnselect, pqsNoSelect);
    pqsReady->addTransition(pstConfirm, &QPushButton::clicked, pqsDeactive);
    pqsReady->assignProperty(pstConfirm, "visible", true);

    connect(pstOldCard, &LVL_InterCard::sgnClicked, this, &LVL_GameHuman::LVL_GameClickCard);
    connect(pstNewCard, &LVL_InterCard::sgnClicked, this, &LVL_GameHuman::LVL_GameClickCard);
    connect(pqsDeactive, &QState::entered, [this] { LVL_GamePlayCard(); LVL_GameEndRound();});

    pqsmCardCtrl->start();
}

/*
    功能：根据点击的卡牌更改出牌状态
    参数：无
    返回值：无
*/
void LVL_GameHuman::LVL_GameClickCard() {
    if (false == bIsOwnRound) return;//未到自己回合不能选牌
    
    //判定女伯爵技能
    LVL_InterCard* pstSender = qobject_cast<LVL_InterCard*>(sender());
    if (LVL_InterCardType_E::prince == pstSender->LVL_InterGetCardType() || LVL_InterCardType_E::king == pstSender->LVL_InterGetCardType()) {//选择的牌是王子或国王
        LVL_InterCard* pstOther = (pstNewCard == pstSender) ? pstOldCard : pstNewCard;
        if (LVL_InterCardType_E::countess == pstOther->LVL_InterGetCardType()) {//且另一张卡是女伯爵
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
        if (0 == pstCtrller->LVL_GameGetTargets(pstSelect->LVL_InterGetCardType(), this).length()) emit sgnTarget(NULL);//打出的卡牌没有可选目标或无需选择目标时，跳过选择目标阶段
    }
}

/*
    功能：根据点击的玩家更改目标选择状态
    参数：无
    返回值：无
*/
void LVL_GameHuman::LVL_GameClickPortrait() {
    if (false == bIsOwnRound || NULL == pstSelect || false == LVL_InterCard::m_abNeedTarget[(int)(pstSelect->LVL_InterGetCardType())]) return;//未到自己回合、未选牌或出牌无需目标时不能选择对象
    
    LVL_GamePlayer* pstSender = dynamic_cast<LVL_GamePlayer*>(sender());//获取点选的目标
    if (this == pstSender && LVL_InterCardType_E::prince != pstSelect->LVL_InterGetCardType()) return;//除王子外不能选择自己作为目标
    if (NULL != pstTarget) emit sgnUntarget(pstTarget);//已选目标时，取消先前选择的目标
    if (pstSender == pstTarget) pstTarget = NULL;//如果点击的是先前已经选择的目标，则将选择目标置空
    else { pstTarget = pstSender; emit sgnTarget(pstSender);}//否则选择点击的目标
}

///////////////////////////////////////////////////////////////

/*
    功能：AI玩家构造函数
    内容：实现人类玩家出牌相关功能
    参数：parent-玩家所属窗口的指针；qpSlot-卡槽左上角坐标；qpDiscard-弃牌堆左上角坐标；sCard{Width, Height}-卡牌大小；sCardSpace-两张卡牌的间隔
*/
LVL_GameAI::LVL_GameAI(LVL_GameCtrller *parent, const QPoint &qpSlot, const QPoint &qpDiscard, const QSize &qsCardSize, int sCardSpace)
    : LVL_GamePlayer(parent, false, qpSlot, qpDiscard, qsCardSize, sCardSpace)
{
    connect(this, &LVL_GameAI::sgnRoundStarted, [this] { QTimer::singleShot(LVL_GAME_AI_WAITING_MSEC, this, &LVL_GameAI::LVL_GameAIGetReady); });//选牌的等待时间
    connect(this, &LVL_GameAI::sgnFinish, [this] { QTimer::singleShot(LVL_GAME_AI_WAITING_MSEC, this, [this] {//选择结束
        pstSelect->LVL_InterSetCardKnown(false);//被选卡槽变回不可见
        LVL_GamePlayCard();//打出卡牌
        LVL_GameEndRound();//回合结束
    }); });
}

void LVL_GameAI::LVL_GameAIGetReady() {
    const LVL_GameRecord &stRecord = pstCtrller->LVL_GameGetRecord();

    //选择手牌
    LVL_InterCardType_E enOld = pstOldCard->LVL_InterGetCardType(), enNew = pstNewCard->LVL_InterGetCardType(), enSelect;
    if (LVL_InterCardType_E::princess == enNew//避免出公主牌
        || LVL_InterCardType_E::countess == enOld && (LVL_InterCardType_E::prince == enNew || LVL_InterCardType_E::king == enNew)) pstSelect = pstOldCard;//判定女伯爵效果
    else if (LVL_InterCardType_E::princess == enOld//避免出公主牌
        || LVL_InterCardType_E::countess == enNew && (LVL_InterCardType_E::prince == enOld || LVL_InterCardType_E::king == enOld)) pstSelect = pstNewCard;//判定女伯爵效果
    else pstSelect = (qrand() % HandCardNum) ? pstNewCard : pstOldCard;//若不生效，则随意选择卡牌
    
    pstSelect->LVL_InterPopCard();//弹出卡牌
    pstSelect->LVL_InterSetCardKnown(true);//使卡牌可视化
    enSelect = pstSelect->LVL_InterGetCardType();

    //选择目标
    QList<LVL_GamePlayer*> qlistTargets = pstCtrller->LVL_GameGetTargets(enSelect, this);
    if (LVL_InterCardType_E::prince == enSelect && qlistTargets.length() > 1) qlistTargets.removeAll(this);//如有可能，出王子牌时不选自己
    if (qlistTargets.length() > 0) pstTarget = qlistTargets.at(qrand() % qlistTargets.length());
    emit sgnTarget(pstTarget);//标记目标
    emit sgnFinish();//AI选择阶段结束
}