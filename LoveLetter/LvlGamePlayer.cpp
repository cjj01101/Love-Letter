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

int LVL_GamePlayer::sActivePlayerNum = 0;//��ʼ�ڳ������Ϊ0

/*
    ���ܣ���һ��๹�캯��
    ������parent-����������ڵ�ָ�룻qpSlot-�������Ͻ����ꣻqpDiscard-���ƶ����Ͻ����ꣻsCard{Width, Height}-���ƴ�С��sCardSpace-���ſ��Ƶļ��
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
    ���ܣ���������
    ���ݣ������ƶ���ʾ���ƣ����������ƺ����ƶ�״̬
    ������pstCard-Ҫ���õ����Ƶ�ָ��
    ����ֵ����
*/
void LVL_GamePlayer::LVL_GameMakeDiscard(LVL_InterCard *pstCard) {
    int sDisCardWidth = pstCard->width() * LVL_GAME_DISCARD_SCALE, sDisCardHeight = pstCard->height() * LVL_GAME_DISCARD_SCALE;
    LVL_InterCard *pstDiscard = new LVL_InterCard(window(), pstCard->LVL_InterGetCardType(), true,
        qpDiscardPilePos.x() + sDiscardPileSize * (sDisCardWidth / 4.0), qpDiscardPilePos.y(), sDisCardWidth, sDisCardHeight);
    pstDiscard->show();
    sDiscardPileSize++;//���ƶ�+1
    sDiscardPilePoint += pstCard->LVL_InterGetCardType();
}

/*
    ���ܣ����һ�ſ���
    ���ݣ������ƶ���ʾ���ƣ��������Ƽ��ܣ�����������״̬�����ƺͼ��ܶ�����pstSelect��pstTargetȷ��
    ��������
    ����ֵ����
*/
void LVL_GamePlayer::LVL_GamePlayCard() {
    if (NULL == pstSelect) return;
    
    LVL_InterCard::LVL_InterCardType_E enType = pstSelect->enType;
    pstSelect->LVL_InterWithdrawCard();//�ջؿ���
    emit sgnUntarget(pstTarget);//����ͷͼ
    LVL_GameMakeDiscard(pstSelect);//��ʾ����
    if (pstOldCard == pstSelect) {//��ʾ����
        pstOldCard->LVL_InterSetCardType(pstNewCard->LVL_InterGetCardType());
        QPropertyAnimation *qpaMove = new QPropertyAnimation(pstOldCard, "geometry");
        qpaMove->setDuration(500);
        qpaMove->setStartValue(pstNewCard->geometry());
        qpaMove->setEndValue(pstOldCard->geometry());
        qpaMove->setEasingCurve(QEasingCurve::InOutQuad);
        qpaMove->start();
    }
    pstNewCard->LVL_InterSetCardType(LVL_InterCard::none);//�������Ŀ�����none

    dynamic_cast<LVL_GameCtrller*>(parent())->LVL_GameTriggerEffect(enType, this, pstTarget);//��������Ч��
    pstSelect = NULL;//ȡ������ѡ��
    pstTarget = NULL;//ȡ��Ŀ��ѡ��
}

/*
    ���ܣ������ҳ����������غϣ�����ӷ��ƶ��г��Ʋ������������������ƶ����ƻ����һ�˴���������Ϸ����
    ��������
    ����ֵ����
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
    ���ܣ�������ҹ��캯��
    ������parent-����������ڵ�ָ�룻qpSlot-�������Ͻ����ꣻqpDiscard-���ƶ����Ͻ����ꣻsCard{Width, Height}-���ƴ�С��sCardSpace-���ſ��Ƶļ��
    ���ݣ�ʵ��������ҳ�����ع���
*/
LVL_GameHuman::LVL_GameHuman(LVL_GameCtrller *parent, const QPoint &qpSlot, const QPoint &qpDiscard, int sCardWidth, int sCardHeight, int sCardSpace)
    : LVL_GamePlayer(parent, true, qpSlot, qpDiscard, sCardWidth, sCardHeight, sCardSpace)
{ 
    LVL_InterBtn *pstConfirm = new LVL_InterBtn(window(), QStringLiteral("ȷ�ϳ���"), QRect(720, 600, 80, 20));
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
    ���ܣ����ݵ���Ŀ��Ƹ��ĳ���״̬
    ��������
    ����ֵ����
*/
void LVL_GameHuman::LVL_GameClickCard() {
    if (false == bIsOwnRound) return;//δ���Լ��غϲ���ѡ��
    
    //�ж�Ů��������
    LVL_InterCard* pstSender = dynamic_cast<LVL_InterCard*>(sender());
    if (LVL_InterCard::prince == pstSender->LVL_InterGetCardType() || LVL_InterCard::king == pstSender->LVL_InterGetCardType()) {
        LVL_InterCard* pstOther = (pstNewCard == pstSender) ? pstOldCard : pstNewCard;
        if (LVL_InterCard::countess == pstOther->LVL_InterGetCardType()) {
            QMessageBox::information(NULL, QStringLiteral("������Ч"), QStringLiteral("�������������ӻ����ʱ����ֻ�ܴ��Ů������"));
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
        if (NULL == dynamic_cast<LVL_GameCtrller*>(parent())->LVL_GameGetTarget(pstSelect->LVL_InterGetCardType(), this)) emit sgnTarget(NULL);//����Ŀ���û�п�ѡĿ�������ѡ��Ŀ��
    }
}

/*
    ���ܣ����ݵ������Ҹ���Ŀ��ѡ��״̬
    ��������
    ����ֵ����
*/
void LVL_GameHuman::LVL_GameClickPortrait() {
    if (false == bIsOwnRound || NULL == pstSelect || false == LVL_InterCard::m_bNeedTarget[pstSelect->LVL_InterGetCardType()]) return;//δ���Լ��غϡ�δѡ�ƻ��������Ŀ��ʱ����ѡ�����
    
    LVL_GamePlayer* pstSender = dynamic_cast<LVL_GamePlayer*>(sender());
    if (this == pstSender && LVL_InterCard::prince != pstSelect->LVL_InterGetCardType()) return;//�������ⲻ��ѡ���Լ���ΪĿ��
    if (NULL != pstTarget) emit sgnUntarget(pstTarget);
    if (pstSender == pstTarget) pstTarget = NULL;
    else { pstTarget = pstSender; emit sgnTarget(pstSender);}
}

///////////////////////////////////////////////////////////////

/*
    ���ܣ�AI��ҹ��캯��
    ������parent-����������ڵ�ָ�룻qpSlot-�������Ͻ����ꣻqpDiscard-���ƶ����Ͻ����ꣻsCard{Width, Height}-���ƴ�С��sCardSpace-���ſ��Ƶļ��
    ���ݣ�ʵ��������ҳ�����ع���
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
    if (LVL_InterCard::princess == enNew//�����������
        || LVL_InterCard::countess == enOld && (LVL_InterCard::prince == enNew || LVL_InterCard::king == enNew)) pstSelect = pstOldCard;//�ж�Ů����Ч��
    else if (LVL_InterCard::princess == enOld//�����������
        || LVL_InterCard::countess == enNew && (LVL_InterCard::prince == enOld || LVL_InterCard::king == enOld)) pstSelect = pstNewCard;//�ж�Ů����Ч��
    else pstSelect = (qrand() % HandCardNum) ? pstNewCard : pstOldCard;//������Ч��������ѡ����
    pstSelect->LVL_InterPopCard();//��������
    pstSelect->LVL_InterSetCardKnown(true);//ʹ���ƿ��ӻ�
    pstTarget = dynamic_cast<LVL_GameCtrller*>(parent())->LVL_GameGetTarget(pstSelect->LVL_InterGetCardType(), this);//ѡ��Ŀ��
    emit sgnTarget(pstTarget);//���Ŀ��
    emit sgnFinish();
}