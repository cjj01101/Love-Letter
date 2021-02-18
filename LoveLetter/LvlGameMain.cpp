////////////////////////////////////////////////
//File Name           : LVLGameMain.cpp
//2nd Model Name      : Game
//Description         : Implement the GameController that controls the game.
//Lastest Update Time : 2020/12/30 12:35
//Version             : 1.3
////////////////////////////////////////////////

#include "LvlGameMain.h"
#include "LvlGamePlayer.h"

/*
    功能：主窗口构造函数
    参数：parent-父窗口的指针
    内容：设置背景，创建游戏控制器
*/
LoveLetter::LoveLetter(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoveLetterClass)
{
    ui->setupUi(this);
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));//种子
    
    //设置背景
    QPalette qpalBackground = this->palette();
    qpalBackground.setBrush(QPalette::Background, QBrush(QPixmap(LVL_PIC_BACKGROUND)));
    setPalette(qpalBackground);

    //加入游戏控制器
    LVL_GameCtrller *pstGameCtrl = new LVL_GameCtrller(this);
    pstGameCtrl->hide();

    //退出按钮
    LVL_InterBtn *pstQuit = new LVL_InterBtn(this, QStringLiteral("退出"), QRect(0, 0, 80, 40));
    pstQuit->LVL_InterSetBtnPic(LVL_PIC_BUTTON, LVL_PIC_BUTTON LVL_PIC_FOCUSED, LVL_PIC_BUTTON LVL_PIC_CLICKED);
    connect(pstQuit, &LVL_InterBtn::clicked, this, &QWidget::close);
}

LoveLetter::~LoveLetter()
{
    delete ui;
}

///////////////////////////////////////////////////////////////

/*
    功能：游戏控制器构造函数
    参数：parent-父窗口的指针
    内容：创建四位玩家，创建游戏过程状态机
*/
LVL_GameCtrller::LVL_GameCtrller(QWidget *parent) : LVL_GameDealer(parent), pstGuesser(new LVL_GameGuesser(this, LVL_GAME_GUESSER_POS))
{
    //初始化玩家
    pstHuman = new LVL_GameHuman(this, LVL_GAME_PLAYER_SLOT, LVL_GAME_PLAYER_DISCARD);
    pstAI1 = new LVL_GameAI(this, LVL_GAME_AI1_SLOT, LVL_GAME_AI1_DISCARD);
    pstAI2 = new LVL_GameAI(this, LVL_GAME_AI2_SLOT, LVL_GAME_AI2_DISCARD);
    pstAI3 = new LVL_GameAI(this, LVL_GAME_AI3_SLOT, LVL_GAME_AI3_DISCARD);
    pstHuman->LVL_GameSetPortrait(LVL_PIC_PLAYER_PORT, LVL_GAME_PLAYERPORT_GEO);
    pstAI1->LVL_GameSetPortrait(LVL_PIC_AI1_PORT, LVL_GAME_AI1PORT_GEO);
    pstAI2->LVL_GameSetPortrait(LVL_PIC_AI2_PORT, LVL_GAME_AI2PORT_GEO);
    pstAI3->LVL_GameSetPortrait(LVL_PIC_AI3_PORT, LVL_GAME_AI3PORT_GEO);

    //状态与状态机
    QStateMachine *qsmGameCtrl = new QStateMachine(this);//游戏状态机
    QFinalState *qsEnd = new QFinalState(qsmGameCtrl);//游戏结束状态
    QState *qsHuman = new QState(qsmGameCtrl);//人类回合
    QState *qsAI1 = new QState(qsmGameCtrl);//AI1回合
    QState *qsAI2 = new QState(qsmGameCtrl);//AI2回合
    QState *qsAI3 = new QState(qsmGameCtrl);//AI3回合
    
    LVL_GamePlayer *pstPlayer[LVL_GAME_TOTAL_PLAYER_NUM] = { pstHuman, pstAI1, pstAI2, pstAI3 };
    QState *qsPlayer[LVL_GAME_TOTAL_PLAYER_NUM] = { qsHuman, qsAI1, qsAI2, qsAI3 };
    for (int i = 0; i < LVL_GAME_TOTAL_PLAYER_NUM; i++) {
        pstPlayer[i]->show();
        qsPlayer[i]->addTransition(pstPlayer[i], &LVL_GamePlayer::sgnDeactive, qsPlayer[(i + 1) % LVL_GAME_TOTAL_PLAYER_NUM]);
        qsPlayer[i]->addTransition(pstPlayer[i], &LVL_GamePlayer::sgnGameEnd, qsEnd);
        connect(qsPlayer[i], &QState::entered, pstPlayer[i], &LVL_GamePlayer::LVL_GameActivateHand);
        connect(pstPlayer[i], &LVL_GamePlayer::sgnTarget, [this](LVL_GamePlayer *Target) {
            if (NULL != Target) Target->LVL_GameSetPortraitPic(Target->qstrPortraitName + LVL_PIC_TARGETED,
                Target->qstrPortraitName + LVL_PIC_FOCUSED, Target->qstrPortraitName + LVL_PIC_FOCUSED);
        });
        connect(pstPlayer[i], &LVL_GamePlayer::sgnUntarget, [this](LVL_GamePlayer *Target) {
            if (NULL != Target) Target->LVL_GameSetDefaultPortraitPic();
        });
        connect(pstPlayer[i], &LVL_GamePlayer::sgnPortraitClicked, dynamic_cast<LVL_GameHuman*>(pstHuman), &LVL_GameHuman::LVL_GameClickPortrait);
    }

    connect(qsmGameCtrl, &QState::finished, this, &LVL_GameCtrller::LVL_GameCalculatePoint);

    qsmGameCtrl->setInitialState(qsHuman);
    qsmGameCtrl->start();
}

/*
    功能：随机提供一个可以作为卡牌技能对象的目标
    参数：enSelect-要出的手牌；pstSelf-卡牌持有者的指针
    返回值：LVL_GamePlayer*-目标玩家的指针，如果没有玩家可以作为目标或不需要目标，则返回NULL
*/
LVL_GamePlayer *LVL_GameCtrller::LVL_GameGetTarget(LVL_InterCard::LVL_InterCardType_E enSelect, LVL_GamePlayer *pstSelf) {
    if (false == LVL_InterCard::m_bNeedTarget[enSelect]) return NULL;
    LVL_GamePlayer *pstPlayer[LVL_GAME_TOTAL_PLAYER_NUM] = { pstHuman, pstAI1, pstAI2, pstAI3 };
    LVL_GamePlayer *pstActivePlayer[LVL_GAME_TOTAL_PLAYER_NUM];
    int32_t sPtr = 0;
    for (int i = 0; i < LVL_GAME_TOTAL_PLAYER_NUM; i++)
        if ((LVL_InterCard::prince == enSelect || pstSelf != pstPlayer[i]) && LVL_GamePlayer::Normal == pstPlayer[i]->LVL_GameGetPlayerState())//除王子外，不能选择自己作为目标
            pstActivePlayer[sPtr++] = pstPlayer[i];
    if (sPtr == 0) return NULL;
    return pstActivePlayer[qrand() % sPtr];
}

void LVL_GameCtrller::LVL_GameTriggerEffect(LVL_InterCard::LVL_InterCardType_E enType, LVL_GamePlayer *pstSelf, LVL_GamePlayer *pstTarget) {
    if (true == LVL_InterCard::m_bNeedTarget[enType] && NULL == pstTarget) return;//如果没有可行的目标，就不执行卡牌效果
    
    LVL_InterCard::LVL_InterCardType_E enGuess;
    QEventLoop qelLoop;
    LVL_InterCard::LVL_InterCardType_E enTemp = pstSelf->pstOldCard->LVL_InterGetCardType();
    switch (enType) {

        //守卫
    case LVL_InterCard::guard: {
        if (pstHuman == pstSelf) {
            raise();  show();
            connect(pstGuesser, &LVL_GameGuesser::sgnGuessFinished, [&](LVL_InterCard::LVL_InterCardType_E enReceive) {
                hide();
                enGuess = enReceive;
                qelLoop.quit();
            });
            qelLoop.exec();
        }
        else enGuess = (LVL_InterCard::LVL_InterCardType_E)(qrand() % 7 + 2);
        LVL_InterCard *pstGuess = new LVL_InterCard(window(), enGuess, true,
            (LVL_GAME_WINDOW_WIDTH - LVL_INTER_CARD_WIDTH) / 2, (LVL_GAME_WINDOW_HEIGHT - LVL_INTER_CARD_HEIGHT) / 2, LVL_INTER_CARD_WIDTH, LVL_INTER_CARD_HEIGHT);
        pstGuess->show();
        QTimer::singleShot(LVL_GAME_WATCHING_MSEC, &qelLoop, &QEventLoop::quit);
        qelLoop.exec();
        delete pstGuess;
        if (enGuess == pstTarget->pstOldCard->LVL_InterGetCardType()) pstTarget->LVL_GameKickPlayer();
        break;
    }

        //牧师
    case LVL_InterCard::priest:
        if (pstHuman == pstSelf) {
            pstTarget->pstOldCard->LVL_InterSetCardKnown(true);
            QTimer::singleShot(LVL_GAME_WATCHING_MSEC, &qelLoop, &QEventLoop::quit);
            qelLoop.exec();
            pstTarget->pstOldCard->LVL_InterSetCardKnown(false);
        }
        break;

        //男爵
    case LVL_InterCard::baron:
        if (pstHuman == pstSelf || pstHuman == pstTarget) {
            LVL_GamePlayer *pstShow = (pstHuman == pstSelf) ? pstTarget : pstSelf;
            pstShow->pstOldCard->LVL_InterSetCardKnown(true);
            QTimer::singleShot(LVL_GAME_WATCHING_MSEC, &qelLoop, &QEventLoop::quit);
            qelLoop.exec();
            pstShow->pstOldCard->LVL_InterSetCardKnown(false);
        }
        if (pstSelf->pstOldCard->LVL_InterGetCardType() > pstTarget->pstOldCard->LVL_InterGetCardType()) pstTarget->LVL_GameKickPlayer();
        else if (pstSelf->pstOldCard->LVL_InterGetCardType() < pstTarget->pstOldCard->LVL_InterGetCardType()) pstSelf->LVL_GameKickPlayer();
        break;

        //女仆
    case LVL_InterCard::handmaid: pstSelf->LVL_GameGuardPlayer(); break;

        //王子
    case LVL_InterCard::prince:
        pstTarget->LVL_GameMakeDiscard(pstTarget->pstOldCard);
        LVL_GameMakeDeal(pstTarget, pstTarget->pstOldCard, true);
        break;

        //国王
    case LVL_InterCard::king:
        if (pstHuman == pstSelf || pstHuman == pstTarget) {
            LVL_GamePlayer *pstShow = (pstHuman == pstSelf) ? pstTarget : pstSelf;
            pstShow->pstOldCard->LVL_InterSetCardKnown(true);
            QTimer::singleShot(LVL_GAME_WATCHING_MSEC, &qelLoop, &QEventLoop::quit);
            qelLoop.exec();
            pstShow->pstOldCard->LVL_InterSetCardKnown(false);
        }
        pstSelf->LVL_GameSetCard(pstSelf->pstOldCard, pstTarget->pstOldCard->LVL_InterGetCardType());
        pstTarget->LVL_GameSetCard(pstTarget->pstOldCard, enTemp);
        break;

        //女伯爵
    case LVL_InterCard::countess: break;

        //公主
    case LVL_InterCard::princess: pstSelf->LVL_GameKickPlayer(); break;
    }
}

/*
    功能：在游戏结束时进行结算，决定胜者
    参数：无
    返回值：无
*/
void LVL_GameCtrller::LVL_GameCalculatePoint() {
    LVL_GamePlayer *pstPlayer[LVL_GAME_TOTAL_PLAYER_NUM] = { pstHuman, pstAI1, pstAI2, pstAI3 };

    int sPoint[LVL_GAME_TOTAL_PLAYER_NUM];//四位玩家的手牌分数
    int sDiscardPoint[LVL_GAME_TOTAL_PLAYER_NUM];//四位玩家的弃牌堆分数
    int sWinner = -1;//最高手牌分数玩家
    
    //找到手牌分数最高的玩家
    for (int i = 0; i < LVL_GAME_TOTAL_PLAYER_NUM; i++) {
        pstPlayer[i]->pstOldCard->LVL_InterSetCardKnown(true);
        if (LVL_GamePlayer::Out == pstPlayer[i]->LVL_GameGetPlayerState()) continue;
        sPoint[i] = pstPlayer[i]->pstOldCard->LVL_InterGetCardType();//获得玩家手牌分数
        sDiscardPoint[i] = pstPlayer[i]->sDiscardPilePoint;//获得玩家弃牌堆分数
        if (-1 == sWinner || sPoint[sWinner] < sPoint[i]) sWinner = i;
        else if (sPoint[sWinner] == sPoint[i] && sDiscardPoint[sWinner] < sDiscardPoint[i]) sWinner = i;
    }
    if (0 == sWinner) QMessageBox::information(NULL, QStringLiteral("游戏结束"), QStringLiteral("你赢得了公主的芳心！"));
    else QMessageBox::information(NULL, QStringLiteral("游戏结束"), QString(QStringLiteral("你的情书被丢弃了，玩家%1获得了胜利。")).arg(sWinner));
}

///////////////////////////////////////////////////////////////

LVL_GameGuesser::LVL_GameGuesser(QWidget *parent, const QPoint &qpPos, int width, int height) : QWidget(parent), enGuess(LVL_InterCard::none) {
    setGeometry(qpPos.x(), qpPos.y(), width, height);

    this->setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(QString("border-image:url(%1);").arg(LVL_PIC_GUESSER));

    QGridLayout *qlGuessUI = new QGridLayout(this);
    qlGuessUI->setMargin(100);
    
    LVL_InterBtn *pstChoice[LVL_InterCard::CardTypeNum - 2];
    for (int i = 0; i < LVL_InterCard::CardTypeNum - 2; i++) {
        pstChoice[i] = new LVL_InterBtn(LVL_InterCard(this, (LVL_InterCard::LVL_InterCardType_E)(i + 2)));
        pstChoice[i]->setFixedSize(pstChoice[i]->width(), pstChoice[i]->height());
        pstChoice[i]->setCheckable(true);
        pstChoice[i]->setAutoExclusive(true);
        connect(pstChoice[i], &QPushButton::toggled, [this, i](bool bChecked) {
            if (true == bChecked) enGuess = (LVL_InterCard::LVL_InterCardType_E)(i + 2);
            else enGuess = LVL_InterCard::none;
        });
        qlGuessUI->addWidget(pstChoice[i], i / 4, i % 4);
    }
    LVL_InterBtn *pstConfirm = new LVL_InterBtn(this, QStringLiteral("确定选择"));
    pstConfirm->LVL_InterSetBtnPic(LVL_PIC_BUTTON, LVL_PIC_BUTTON LVL_PIC_FOCUSED, LVL_PIC_BUTTON LVL_PIC_CLICKED);
    connect(pstConfirm, &QPushButton::clicked, [this] { if (LVL_InterCard::none != enGuess) emit sgnGuessFinished(enGuess); });
    qlGuessUI->addWidget(pstConfirm, 1, 3);

    setLayout(qlGuessUI);
}

///////////////////////////////////////////////////////////////

/*
    功能：发牌器构造函数
    参数：parent-父窗口的指针
    内容：创建四位玩家，创建游戏过程状态机
*/
LVL_GameDealer::LVL_GameDealer(QWidget *parent) : QWidget(parent), sCurCard(0) {
    setGeometry(0, 0, LVL_GAME_WINDOW_WIDTH, LVL_GAME_WINDOW_HEIGHT);

    LVL_InterCard::LVL_InterCardType_E aenSet[LVL_GAME_TOTAL_CARD_NUM] = //所有卡牌
    { LVL_InterCard::guard, LVL_InterCard::guard, LVL_InterCard::guard, LVL_InterCard::guard, LVL_InterCard::guard,
      LVL_InterCard::priest, LVL_InterCard::priest, LVL_InterCard::baron, LVL_InterCard::baron,
      LVL_InterCard::handmaid, LVL_InterCard::handmaid, LVL_InterCard::prince, LVL_InterCard::prince,
      LVL_InterCard::king, LVL_InterCard::countess, LVL_InterCard::princess };

    for (int i = 0; i < LVL_GAME_TOTAL_CARD_NUM; i++) {//洗牌
        int sPos = qrand() % (LVL_GAME_TOTAL_CARD_NUM - i);
        aenDrawPile[i] = aenSet[sPos];
        LVL_InterCard::LVL_InterCardType_E enTemp = aenSet[sPos];
        aenSet[sPos] = aenSet[LVL_GAME_TOTAL_CARD_NUM - i - 1];
        aenSet[LVL_GAME_TOTAL_CARD_NUM - i - 1] = enTemp;
    }

    for (; sCurCard < LVL_GAME_SEEN_REMOVE_CARD_NUM; sCurCard++) {//取出可见的三张牌和不可见的一张牌
        LVL_InterCard *pstRemove = new LVL_InterCard(window(), aenDrawPile[sCurCard], true,
            LVL_GAME_REMOVECARD_X + sCurCard * LVL_GAME_REMOVECARD_WIDTH, LVL_GAME_REMOVECARD_Y, LVL_GAME_REMOVECARD_WIDTH, LVL_GAME_REMOVECARD_HEIGHT);
    }

    //显示剩余卡牌数
    QLabel *qlRest = new QLabel(window());
    qlRest->setObjectName("qlRest");
    qlRest->setGeometry(LVL_GAME_DECK_TEXT_GEO);
    QPalette qpalText = qlRest->palette();
    qpalText.setColor(QPalette::WindowText, QColor(255,195,14));
    qlRest->setPalette(qpalText);
    qlRest->setFont(QFont(LVL_GAME_DECK_TEXT_FONT, LVL_GAME_DECK_TEXT_SIZE));
    qlRest->setAlignment(Qt::AlignCenter);
}

/*
    功能：发牌器发牌
    参数：pstPlayer-获取牌的玩家指针；pstCard-要发的牌；bDealUnseen-是否允许发移除牌
    返回值：int-牌堆已发牌数；如果牌已发完，返回0
*/
int LVL_GameDealer::LVL_GameMakeDeal(LVL_GamePlayer *pstPlayer, LVL_InterCard *pstCard, bool bDealUnseen){
    if (false == bDealUnseen && sCurCard >= LVL_GAME_TOTAL_CARD_NUM - LVL_GAME_UNSEEN_REMOVE_CARD_NUM) return 0;//判断卡牌是否发完
    pstPlayer->LVL_GameSetCard(pstCard, aenDrawPile[sCurCard++]);//发牌
    window()->findChild<QLabel*>("qlRest")->setText(QString(QStringLiteral("剩余卡牌：%1(+%2)"))
        .arg(LVL_GAME_TOTAL_CARD_NUM - LVL_GAME_UNSEEN_REMOVE_CARD_NUM - sCurCard > 0 ? LVL_GAME_TOTAL_CARD_NUM - LVL_GAME_UNSEEN_REMOVE_CARD_NUM - sCurCard : 0)
        .arg((int)(sCurCard <= LVL_GAME_TOTAL_CARD_NUM - LVL_GAME_UNSEEN_REMOVE_CARD_NUM)));//更新剩余卡牌数
    return sCurCard;
}