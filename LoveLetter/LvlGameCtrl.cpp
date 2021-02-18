////////////////////////////////////////////////
//File Name           : LvlGameCtrl.cpp
//2nd Model Name      : Game
//Description         : Implement the GameController that controls the game.
//Lastest Update Time : 2021/2/14 16:36
//Version             : 1.4
////////////////////////////////////////////////

#include "LvlGameCtrl.h"
#include "LvlGamePlayer.h"

/*
    功能：游戏控制器构造函数
    内容：创建四位玩家，创建游戏过程状态机
    参数：parent-父窗口的指针；sPlayerNum-玩家数量
*/
LVL_GameCtrller::LVL_GameCtrller(QWidget *parent) : QWidget(parent), bIsProcessing(false)
{
    setGeometry(0, 0, LVL_GAME_WINDOW_WIDTH, LVL_GAME_WINDOW_HEIGHT);

    //初始化牌堆和猜牌器
    pstDeckManager = new LVL_GameDeckManager(this);
    pstGuesser = new LVL_GameGuesser(this, LVL_GAME_GUESSER_POS);
    pstGuesser->hide();//隐藏猜牌界面

    //初始化玩家
    LVL_GamePlayer::sActivePlayerNum = 0;
    apstPlayers[0] = new LVL_GameHuman(this, LVL_GAME_HUMAN_SLOT, LVL_GAME_HUMAN_DISCARD);
    apstPlayers[1] = new LVL_GameAI(this, LVL_GAME_AI1_SLOT, LVL_GAME_AI1_DISCARD);
    apstPlayers[2] = new LVL_GameAI(this, LVL_GAME_AI2_SLOT, LVL_GAME_AI2_DISCARD);
    apstPlayers[3] = new LVL_GameAI(this, LVL_GAME_AI3_SLOT, LVL_GAME_AI3_DISCARD);
    apstPlayers[0]->LVL_GameSetPortrait(LVL_PIC_PLAYER_PORT, LVL_GAME_HUMAN_PORT_GEO);
    apstPlayers[1]->LVL_GameSetPortrait(LVL_PIC_AI1_PORT, LVL_GAME_AI1_PORT_GEO);
    apstPlayers[2]->LVL_GameSetPortrait(LVL_PIC_AI2_PORT, LVL_GAME_AI2_PORT_GEO);
    apstPlayers[3]->LVL_GameSetPortrait(LVL_PIC_AI3_PORT, LVL_GAME_AI3_PORT_GEO);

    //创建状态与状态机
    QStateMachine *pqsmGameCtrl = new QStateMachine(this);//游戏状态机
    QState *apqsPlayers[LVL_GAME_MAXIMUM_PLAYER_NUM];
    for (int i = 0; i < LVL_GAME_MAXIMUM_PLAYER_NUM; i++) apqsPlayers[i] = new QState(pqsmGameCtrl);

    //读取游戏设置
    const LVL_GameSetting &stSettings = LVL_GameSetting::LVL_SystemGetSettings();

    //状态转移和信号连接
    for (int i = 0; i < LVL_GAME_MAXIMUM_PLAYER_NUM; i++) {
        apqsPlayers[i]->addTransition(apstPlayers[i], &LVL_GamePlayer::sgnRoundEnded, apqsPlayers[(i + 1) % LVL_GAME_MAXIMUM_PLAYER_NUM]);//玩家回合转移
        connect(apqsPlayers[i], &QState::entered, apstPlayers[i], &LVL_GamePlayer::LVL_GameStartRound);//玩家回合开始信号
        connect(apstPlayers[i], &LVL_GamePlayer::sgnWantDeal, [=](LVL_InterCard* pstCard) {//玩家需要发牌信号
            if (1 == LVL_GamePlayer::sActivePlayerNum) pqsmGameCtrl->stop();
            else if (0 == pstDeckManager->LVL_GameMakeDeal(apstPlayers[i], pstCard, false)) pqsmGameCtrl->stop();
        });
        connect(apstPlayers[i], &LVL_GamePlayer::sgnTarget, [this](LVL_GamePlayer *pstTarget) {//玩家被选择信号
            if (NULL != pstTarget) pstTarget->LVL_GameSetPortraitPic(pstTarget->qstrPortraitName + LVL_PIC_TARGETED,
                pstTarget->qstrPortraitName + LVL_PIC_FOCUSED, pstTarget->qstrPortraitName + LVL_PIC_FOCUSED);
        });
        connect(apstPlayers[i], &LVL_GamePlayer::sgnUntarget, [this](LVL_GamePlayer *pstTarget) {//玩家被取消选择信号
            if (NULL != pstTarget && LVL_GamePlayer::Normal == pstTarget->LVL_GameGetPlayerState()) pstTarget->LVL_GameSetDefaultPortraitPic();
        });
        connect(apstPlayers[i], &LVL_GamePlayer::sgnPortraitClicked, qobject_cast<LVL_GameHuman*>(apstPlayers[0]), &LVL_GameHuman::LVL_GameClickPortrait);//玩家头像点击信号

        //判定游戏人数，将不需要的玩家踢出局并隐藏，需要的玩家则各发一张牌
        int sPlayerNum = stSettings.LVL_GameReadSetting<int>("Player Number");
        if ((2 == sPlayerNum && (1 == i || 3 == i)) || (3 == sPlayerNum && 2 == i)) {
            apstPlayers[i]->LVL_GameKickPlayer();
            apstPlayers[i]->pstPortrait->hide();
        }
        else pstDeckManager->LVL_GameMakeDeal(apstPlayers[i], apstPlayers[i]->pstOldCard, false);
    }

    connect(pqsmGameCtrl, &QStateMachine::stopped, this, &LVL_GameCtrller::LVL_GameCalculatePoint);

    pqsmGameCtrl->setInitialState(apqsPlayers[0]);
    pqsmGameCtrl->start();
}

/*
    功能：随机提供一个可以作为卡牌技能对象的目标
    参数：enSelect-要出的手牌；pstSelf-卡牌持有者的指针
    返回值：LVL_GamePlayer*-目标玩家的指针，如果没有玩家可以作为目标或不需要目标，则返回NULL
*/
LVL_GamePlayer *LVL_GameCtrller::LVL_GameGetTarget(LVL_InterCard::LVL_InterCardType_E enSelect, LVL_GamePlayer *pstSelf) {
    if (NULL == pstSelf) exit(-1);
    if (false == LVL_InterCard::m_abNeedTarget[enSelect]) return NULL;

    LVL_GamePlayer *apstActivePlayer[LVL_GAME_MAXIMUM_PLAYER_NUM];
    int32_t sSize = 0;
    for (int i = 0; i < sizeof(apstPlayers)/sizeof(apstPlayers[0]); i++)
        if ((LVL_InterCard::prince == enSelect || pstSelf != apstPlayers[i]) && LVL_GamePlayer::Normal == apstPlayers[i]->LVL_GameGetPlayerState())//除王子外，不能选择自己作为目标
            apstActivePlayer[sSize++] = apstPlayers[i];
    if (sSize == 0) return NULL;
    return apstActivePlayer[qrand() % sSize];
}

/*
    功能：发动卡牌技能效果
    参数：enType-要出的卡牌种类；pstSelf-卡牌持有者的指针；pstTarget-卡牌作用对象的指针
    返回值：无
*/
void LVL_GameCtrller::LVL_GameTriggerEffect(LVL_InterCard::LVL_InterCardType_E enType, LVL_GamePlayer *pstSelf, LVL_GamePlayer *pstTarget) {
    if (NULL == pstSelf) exit(-1);
    if (true == LVL_InterCard::m_abNeedTarget[enType] && NULL == pstTarget) return;//如果没有可行的目标，就不执行卡牌效果
    
    QEventLoop qelLoop;
    LVL_GamePlayer *pstShow = NULL;

    bIsProcessing = true;
    switch (enType) {

        //守卫
    case LVL_InterCard::guard: {
        LVL_InterCard::LVL_InterCardType_E enGuess;
        if (qobject_cast<LVL_GameHuman*>(pstSelf)) {
            pstGuesser->raise();  pstGuesser->show();
            connect(pstGuesser, &LVL_GameGuesser::sgnGuessFinished, [&](LVL_InterCard::LVL_InterCardType_E enReceive) {
                pstGuesser->hide();
                enGuess = enReceive;
                qelLoop.quit();
            });
            qelLoop.exec();
        }
        else enGuess = (LVL_InterCard::LVL_InterCardType_E)(qrand() % 7 + 2);
        LVL_InterCard *pstGuess = new LVL_InterCard(this, enGuess, true,
            QPoint((LVL_GAME_WINDOW_WIDTH - LVL_INTER_CARD_WIDTH) / 2, (LVL_GAME_WINDOW_HEIGHT - LVL_INTER_CARD_HEIGHT) / 2));
        pstGuess->show();
        QTimer::singleShot(LVL_GAME_WATCHING_MSEC, &qelLoop, &QEventLoop::quit);
        qelLoop.exec();
        delete pstGuess;
        if (enGuess == pstTarget->pstOldCard->LVL_InterGetCardType()) pstTarget->LVL_GameKickPlayer();
        break;
    }

        //牧师
    case LVL_InterCard::priest: {
        if (qobject_cast<LVL_GameHuman*>(pstSelf)) {
            QSequentialAnimationGroup* pqsagWatching = new QSequentialAnimationGroup(this);
            pqsagWatching->addAnimation(pstTarget->pstOldCard->LVL_InterCreateFilppingAnimation(LVL_GAME_FLIPPING_MSEC));
            pqsagWatching->addAnimation(new QPauseAnimation(LVL_GAME_WATCHING_MSEC, this));
            pqsagWatching->addAnimation(pstTarget->pstOldCard->LVL_InterCreateFilppingAnimation(LVL_GAME_FLIPPING_MSEC));
            connect(pqsagWatching, &QAbstractAnimation::finished, &qelLoop, &QEventLoop::quit);
            pqsagWatching->start(QAbstractAnimation::DeleteWhenStopped);
            qelLoop.exec();
        }
        break;
    }

        //男爵
    case LVL_InterCard::baron: {

        //显示手牌
        if (qobject_cast<LVL_GameHuman*>(pstSelf) || qobject_cast<LVL_GameHuman*>(pstTarget)) {
            pstShow = (qobject_cast<LVL_GameAI*>(pstSelf)) ? pstSelf : pstTarget;
            QSequentialAnimationGroup* pqsagWatching = new QSequentialAnimationGroup(this);
            pqsagWatching->addAnimation(pstShow->pstOldCard->LVL_InterCreateFilppingAnimation(LVL_GAME_FLIPPING_MSEC));
            pqsagWatching->addAnimation(new QPauseAnimation(LVL_GAME_WATCHING_MSEC, this));
            pqsagWatching->start(QAbstractAnimation::DeleteWhenStopped);
            connect(pqsagWatching, &QAbstractAnimation::finished, &qelLoop, &QEventLoop::quit);
            qelLoop.exec();
        }

        //比拼手牌点数
        if (pstSelf->pstOldCard->LVL_InterGetCardType() > pstTarget->pstOldCard->LVL_InterGetCardType()) pstTarget->LVL_GameKickPlayer();
        else if (pstSelf->pstOldCard->LVL_InterGetCardType() < pstTarget->pstOldCard->LVL_InterGetCardType()) pstSelf->LVL_GameKickPlayer();

        //隐藏手牌
        if (NULL != pstShow && LVL_GamePlayer::Out != pstShow->LVL_GameGetPlayerState()) {
            QAbstractAnimation* pqaaSlipping = pstShow->pstOldCard->LVL_InterCreateFilppingAnimation(LVL_GAME_FLIPPING_MSEC);
            pqaaSlipping->start(QAbstractAnimation::DeleteWhenStopped);
            connect(pqaaSlipping, &QAbstractAnimation::finished, &qelLoop, &QEventLoop::quit);
            qelLoop.exec();
        }
        break;
    }
        //女仆
    case LVL_InterCard::handmaid: pstSelf->LVL_GameGuardPlayer(); break;

        //王子
    case LVL_InterCard::prince: {

        //显示动画
        QParallelAnimationGroup* pqsagThrowing = new QParallelAnimationGroup(this);
        QPoint qpOrigin = pstTarget->pstOldCard->pos();
        QPropertyAnimation *pqaaMoving = pstTarget->pstOldCard->LVL_InterCreateAnimation("pos", 1000, qpOrigin, qpOrigin + QPoint(0, 50));
        pqaaMoving->setKeyValueAt(0.3, qpOrigin - QPoint(0, 30));
        QAbstractAnimation *pqaaFading = pstTarget->pstOldCard->LVL_InterCreateAnimation("opacity", 1000, 1, 0, QEasingCurve::Linear);
        pqsagThrowing->addAnimation(pqaaMoving);
        pqsagThrowing->addAnimation(pqaaFading);
        pqsagThrowing->start(QAbstractAnimation::DeleteWhenStopped);
        connect(pqsagThrowing, &QAbstractAnimation::finished, &qelLoop, &QEventLoop::quit);
        qelLoop.exec();

        //恢复卡槽原属性
        pstTarget->pstOldCard->setProperty("pos", qpOrigin);
        pstTarget->pstOldCard->setProperty("opacity", 1);

        if (LVL_InterCard::princess == pstTarget->pstOldCard->LVL_InterGetCardType()) pstTarget->LVL_GameKickPlayer();//判定丢弃的牌是否为公主
        pstTarget->LVL_GameMakeDiscard(pstTarget->pstOldCard);
        pstDeckManager->LVL_GameMakeDeal(pstTarget, pstTarget->pstOldCard, true);
        break;
    }

        //国王
    case LVL_InterCard::king: {

        //显示手牌
        if (qobject_cast<LVL_GameHuman*>(pstSelf) || qobject_cast<LVL_GameHuman*>(pstTarget)) {
            pstShow = (qobject_cast<LVL_GameAI*>(pstSelf)) ? pstSelf : pstTarget;
            QSequentialAnimationGroup* pqsagWatching = new QSequentialAnimationGroup(this);
            pqsagWatching->addAnimation(pstShow->pstOldCard->LVL_InterCreateFilppingAnimation(LVL_GAME_FLIPPING_MSEC));
            pqsagWatching->addAnimation(new QPauseAnimation(LVL_GAME_WATCHING_MSEC, this));
            pqsagWatching->start(QAbstractAnimation::DeleteWhenStopped);
            connect(pqsagWatching, &QAbstractAnimation::finished, &qelLoop, &QEventLoop::quit);
            qelLoop.exec();
        }

        //交换手牌
        LVL_InterCard::LVL_InterCardType_E enTemp = pstSelf->pstOldCard->LVL_InterGetCardType();
        pstSelf->LVL_GameSetCard(pstSelf->pstOldCard, pstTarget->pstOldCard->LVL_InterGetCardType());
        pstTarget->LVL_GameSetCard(pstTarget->pstOldCard, enTemp);

        //移动动画
        QPoint qpTemp = pstSelf->pstOldCard->pos();
        QParallelAnimationGroup* pqlagSwapping = new QParallelAnimationGroup(this);
        pqlagSwapping->addAnimation(pstSelf->pstOldCard->LVL_InterCreateAnimation("pos", LVL_GAME_MOVING_MSEC, pstTarget->pstOldCard->pos(), pstSelf->pstOldCard->pos(), QEasingCurve::InOutCubic));
        pqlagSwapping->addAnimation(pstTarget->pstOldCard->LVL_InterCreateAnimation("pos", LVL_GAME_MOVING_MSEC, qpTemp, pstTarget->pstOldCard->pos(), QEasingCurve::InOutCubic));
        pqlagSwapping->start(QAbstractAnimation::DeleteWhenStopped);
        connect(pqlagSwapping, &QAbstractAnimation::finished, &qelLoop, &QEventLoop::quit);
        qelLoop.exec();
        
        //隐藏手牌
        if (NULL != pstShow) {
            QAbstractAnimation* pqaaSlipping = pstShow->pstOldCard->LVL_InterCreateFilppingAnimation(LVL_GAME_FLIPPING_MSEC);
            pqaaSlipping->start(QAbstractAnimation::DeleteWhenStopped);
            connect(pqaaSlipping, &QAbstractAnimation::finished, &qelLoop, &QEventLoop::quit);
            qelLoop.exec();
        }
        break;
    }

        //女伯爵
    case LVL_InterCard::countess: break;

        //公主
    case LVL_InterCard::princess: pstSelf->LVL_GameKickPlayer(); break;
    }
    bIsProcessing = false;
    emit sgnNoProcessing();
}

/*
    功能：在游戏结束时进行结算，决定胜者
    参数：无
    返回值：无
*/
void LVL_GameCtrller::LVL_GameCalculatePoint() {
    int asHandPoint[LVL_GAME_MAXIMUM_PLAYER_NUM];//四位玩家的手牌分数
    int asDiscardPoint[LVL_GAME_MAXIMUM_PLAYER_NUM];//四位玩家的弃牌堆分数
    int sWinner = -1;//胜者
    
    //找到手牌分数最高的玩家
    for (int i = 0; i < sizeof(apstPlayers)/sizeof(apstPlayers[0]); i++) {
        apstPlayers[i]->pstOldCard->LVL_InterSetCardKnown(true);//展示玩家手牌
        if (LVL_GamePlayer::Out == apstPlayers[i]->LVL_GameGetPlayerState()) continue;//出局者不参与判定
        asHandPoint[i] = apstPlayers[i]->pstOldCard->LVL_InterGetCardType();//获得玩家手牌分数
        asDiscardPoint[i] = apstPlayers[i]->sDiscardPilePoint;//获得玩家弃牌堆分数
        if (-1 == sWinner || asHandPoint[sWinner] < asHandPoint[i]) sWinner = i;
        else if (asHandPoint[sWinner] == asHandPoint[i] && asDiscardPoint[sWinner] < asDiscardPoint[i]) sWinner = i;
    }
    if (0 == sWinner) QMessageBox::information(NULL, QStringLiteral("游戏结束"), QStringLiteral("你赢得了公主的芳心！"));
    else QMessageBox::information(NULL, QStringLiteral("游戏结束"), QString(QStringLiteral("你的情书被丢弃了，玩家%1获得了胜利。")).arg(sWinner));
}

void LVL_GameCtrller::closeEvent(QCloseEvent *event) {
    event->accept();
    if (!bIsProcessing || pstGuesser->isVisible()) deleteLater();
    else connect(this, &LVL_GameCtrller::sgnNoProcessing, this, &QObject::deleteLater);
}

///////////////////////////////////////////////////////////////

const LVL_InterCard::LVL_InterCardType_E LVL_GameDeckManager::m_aenCardSet[LVL_GAME_TOTAL_CARD_NUM] = //所有卡牌
{ LVL_InterCard::guard, LVL_InterCard::guard, LVL_InterCard::guard, LVL_InterCard::guard, LVL_InterCard::guard,
  LVL_InterCard::priest, LVL_InterCard::priest, LVL_InterCard::baron, LVL_InterCard::baron,
  LVL_InterCard::handmaid, LVL_InterCard::handmaid, LVL_InterCard::prince, LVL_InterCard::prince,
  LVL_InterCard::king, LVL_InterCard::countess, LVL_InterCard::princess };

/*
    功能：发牌器构造
    内容：洗牌，移除必要的牌，并在屏幕上显示剩余卡牌数
    参数：parent-父窗口的指针；sPlayerNum-玩家数量
*/
LVL_GameDeckManager::LVL_GameDeckManager(QWidget *parent) : QWidget(parent), sCurCard(0) {

    //读取游戏设置
    const LVL_GameSetting &stSettings = LVL_GameSetting::LVL_SystemGetSettings();
    int sPlayerNum = stSettings.LVL_GameReadSetting<int>("Player Number");

    //洗牌
    for (int i = 0; i < sizeof(m_aenCardSet) / sizeof(m_aenCardSet[0]); i++) aenDrawPile[i] = m_aenCardSet[i];
    for (int i = 0; i < sizeof(m_aenCardSet) / sizeof(m_aenCardSet[0]); i++) {
        int sPos = qrand() % (LVL_GAME_TOTAL_CARD_NUM - i);
        LVL_InterCard::LVL_InterCardType_E enTemp = aenDrawPile[sPos];
        aenDrawPile[sPos] = aenDrawPile[LVL_GAME_TOTAL_CARD_NUM - i - 1];
        aenDrawPile[LVL_GAME_TOTAL_CARD_NUM - i - 1] = enTemp;
    }

    //取出并显示可见的牌
    for (; sCurCard < LVL_GAME_SEEN_REMOVE_CARD_NUM; sCurCard++) {
        LVL_InterCard *pstRemove = new LVL_InterCard(this, aenDrawPile[sCurCard], true,
            LVL_GAME_REMOVECARD_POS + QPoint(sCurCard * LVL_GAME_REMOVECARD_WIDTH, 0), QSize(LVL_GAME_REMOVECARD_WIDTH, LVL_GAME_REMOVECARD_HEIGHT));
        pstRemove->show();
    }

    //显示剩余卡牌数
    pqlRest = new QLabel(this);
    pqlRest->setGeometry(LVL_GAME_DECK_TEXT_GEO);
    QPalette qpalText = pqlRest->palette();
    qpalText.setColor(QPalette::WindowText, LVL_GAME_DECK_TEXT_COLOR);
    pqlRest->setPalette(qpalText);
    pqlRest->setFont(QFont(LVL_GAME_DECK_TEXT_FONT, LVL_GAME_DECK_TEXT_SIZE));
    pqlRest->setAlignment(Qt::AlignCenter);
}

/*
    功能：发牌器发牌
    参数：apstPlayers-获取牌的玩家指针；pstCard-要发的牌；bDealUnseen-是否允许发不可见的移除牌
    返回值：int-牌堆已发牌数；如果牌已发完，返回0
*/
int LVL_GameDeckManager::LVL_GameMakeDeal(LVL_GamePlayer *pstPlayer, LVL_InterCard *pstCard, bool bDealUnseen){
    if (NULL == pstPlayer || NULL == pstCard) exit(-1);
    if (false == bDealUnseen && sCurCard >= LVL_GAME_TOTAL_CARD_NUM - LVL_GAME_UNSEEN_REMOVE_CARD_NUM) return 0;//判断卡牌是否发完

    pstPlayer->LVL_GameSetCard(pstCard, aenDrawPile[sCurCard++]);//发牌
    pqlRest->setText(QString(QStringLiteral("剩余卡牌：%1(+%2)"))
        .arg(LVL_GAME_TOTAL_CARD_NUM - LVL_GAME_UNSEEN_REMOVE_CARD_NUM - sCurCard > 0 ? LVL_GAME_TOTAL_CARD_NUM - LVL_GAME_UNSEEN_REMOVE_CARD_NUM - sCurCard : 0)
        .arg((int)(sCurCard <= LVL_GAME_TOTAL_CARD_NUM - LVL_GAME_UNSEEN_REMOVE_CARD_NUM)));//更新剩余卡牌数
    return sCurCard;
}

///////////////////////////////////////////////////////////////

/*
    功能：绘制猜牌界面
    参数：parent-父窗口的指针；qpPos-猜牌页面的左上角坐标；{ width,height }-猜牌页面大小
*/
LVL_GameGuesser::LVL_GameGuesser(QWidget *parent, const QPoint &qpPos, const QSize &qsSize) : QWidget(parent), enGuess(LVL_InterCard::none) {
    setGeometry(QRect(qpPos, qsSize));

    //设置背景图片
    setAutoFillBackground(true);
    QPalette qpalBackground = palette();
    qpalBackground.setBrush(QPalette::Background, QBrush(QPixmap(LVL_PIC_GUESSER).scaled(size())));
    setPalette(qpalBackground);

    //创建按钮布局管理
    QGridLayout *pqloGuesserUI = new QGridLayout(this);
    pqloGuesserUI->setMargin(LVL_GAME_GUEESER_MARGIN);
    setLayout(pqloGuesserUI);

    //绘制选择按钮
    LVL_InterBtn *pstConfirm = new LVL_InterBtn(this);
    pstConfirm->LVL_InterSetBtnPic(LVL_PIC_CONFIRM_BTN, LVL_PIC_CONFIRM_BTN + LVL_PIC_FOCUSED, LVL_PIC_CONFIRM_BTN + LVL_PIC_CLICKED);
    pstConfirm->setFixedSize(LVL_GAME_GUESSER_CONFIRM_SIZE);
    pstConfirm->hide();
    connect(pstConfirm, &QPushButton::clicked, [this] { if (LVL_InterCard::none != enGuess) emit sgnGuessFinished(enGuess); });
    pqloGuesserUI->addWidget(pstConfirm, (LVL_InterCard::CardTypeNum - 2) / LVL_GAME_GUESSER_COLUMN, (LVL_InterCard::CardTypeNum - 2) % LVL_GAME_GUESSER_COLUMN);

    //绘制所有选项
    LVL_InterBtn *apstChoice[LVL_InterCard::CardTypeNum - 2];
    for (int i = 0; i < sizeof(apstChoice)/sizeof(apstChoice[0]); i++) {
        apstChoice[i] = new LVL_InterBtn(LVL_InterCard(this, (LVL_InterCard::LVL_InterCardType_E)(i + 2)));
        apstChoice[i]->setFixedSize(apstChoice[i]->width(), apstChoice[i]->height());
        apstChoice[i]->setCheckable(true);
        apstChoice[i]->setAutoExclusive(true);
        connect(apstChoice[i], &QPushButton::toggled, [=]() { enGuess = (LVL_InterCard::LVL_InterCardType_E)(i + 2); pstConfirm->show(); });
        pqloGuesserUI->addWidget(apstChoice[i], i / LVL_GAME_GUESSER_COLUMN, i % LVL_GAME_GUESSER_COLUMN);
    }
}