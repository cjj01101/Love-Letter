////////////////////////////////////////////////
//File Name           : LvlGamePlayer.cpp
//2nd Model Name      : Game
//Description         : Implement the Players.
//Lastest Update Time : 2021/2/14 16:36
//Version             : 1.2
////////////////////////////////////////////////

#include "LvlGamePlayer.h"
#include "LvlGameCtrl.h"

int LVL_GamePlayer::sActivePlayerNum = 0;//��ʼ�ڳ������Ϊ0

/*
    ���ܣ���һ��๹�캯��
    ������parent-����������ڵ�ָ�룻bKnown-��ҿ����Ƿ�ɼ���qpSlot-�������Ͻ����ꣻqpDiscard-���ƶ����Ͻ����ꣻsCard{Width, Height}-���ƴ�С��sCardSpace-���ſ��Ƶļ��
*/
LVL_GamePlayer::LVL_GamePlayer(LVL_GameCtrller *parent, bool bKnown, const QPoint &qpSlot, const QPoint &qpDiscard, const QSize &qsCardSize, int sCardSpace)
    : QWidget(parent), pstCtrller(parent), enState(LVL_GamePlayerState_E::Normal), bIsOwnRound(false), sDiscardPileSize(0), sDiscardPilePoint(0), qpDiscardPilePos(qpDiscard)
    , pstOldCard(new LVL_InterCard(parent, LVL_InterCardType_E::none, bKnown, qpSlot, qsCardSize))
    , pstNewCard(new LVL_InterCard(parent, LVL_InterCardType_E::none, bKnown, qpSlot + QPoint(qsCardSize.width() + sCardSpace, 0), qsCardSize))
    , pstSelect(NULL), pstTarget(NULL), pstPortrait(new LVL_InterBtn(parent))
{
    sActivePlayerNum++;//�ڳ��������һ
    setGeometry(0, 0, 0, 0);
    connect(pstPortrait, &QPushButton::clicked, [this] { if (LVL_GamePlayerState_E::Normal == enState) emit sgnPortraitClicked(); });//ͷ��������
}

/*
     ���ܣ�ʹ��ҳ���
     ��������
     ����ֵ����
*/
void LVL_GamePlayer::LVL_GameKickPlayer() {
    pstPortrait->LVL_InterSetBtnPic(qstrPortraitName + LVL_PIC_OUT, qstrPortraitName + LVL_PIC_OUT, qstrPortraitName + LVL_PIC_OUT);//����ͷͼ
    enState = LVL_GamePlayerState_E::Out;//����״̬Ϊ����
    pstOldCard->LVL_InterSetCardKnown(true);//չʾ����
    pstCtrller->LVL_GameAddDiscardEntry(this, pstOldCard->LVL_InterGetCardType());
    sActivePlayerNum--;//�ڳ��������һ
}

/*
    ���ܣ���������
    ���ݣ������ƶ���ʾ���ƣ����������ƺ����ƶ�״̬
    ������pstCard-Ҫ���õ����Ƶ�ָ��
    ����ֵ����
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
    ���ܣ����һ�ſ���
    ���ݣ������ƶ���ʾ���ƣ��������Ƽ��ܣ�����������״̬�����ƺͼ��ܶ�����pstSelect��pstTargetȷ��
    ��������
    ����ֵ����
*/
void LVL_GamePlayer::LVL_GamePlayCard() {
    if (NULL == pstSelect) return;
    
    LVL_InterCardType_E enType = pstSelect->LVL_InterGetCardType();
    pstSelect->LVL_InterWithdrawCard();//�ջؿ���
    QPoint qpOrigin = pstOldCard->pos();

    LVL_GameMakeDiscard(pstSelect);//��ʾ����
    if (pstOldCard == pstSelect) {//��������Ǿ��ƣ������Ʊ�Ϊ����
        pstOldCard->LVL_InterSetCardType(pstNewCard->LVL_InterGetCardType());
        pstOldCard->LVL_InterSetCardPos(pstNewCard->pos());
    }
    pstNewCard->LVL_InterSetCardType(LVL_InterCardType_E::none);//�������Ŀ�����none

    pstCtrller->LVL_GameTriggerEffect(enType, this, pstTarget);//��������Ч��
    emit sgnUntarget(pstTarget);//����ͷͼ
    pstTarget = NULL;//ȡ��Ŀ��ѡ��

    if (pstOldCard == pstSelect) {
        QAbstractAnimation* pqaaMoving = pstOldCard->LVL_InterCreateAnimation("pos", LVL_GAME_MOVING_MSEC, pstOldCard->pos(), qpOrigin, QEasingCurve::InOutQuad);
        pqaaMoving->start(QAbstractAnimation::DeleteWhenStopped);
        QEventLoop qelLoop;
        connect(pqaaMoving, &QAbstractAnimation::finished, &qelLoop, &QEventLoop::quit);
        qelLoop.exec();
    }
    pstSelect = NULL;//ȡ������ѡ��
}

/*
    ���ܣ������ҳ����������غϣ�����ӷ��ƶ��г��Ʋ������������������ƶ����ƻ����һ�˴���������Ϸ����
    ��������
    ����ֵ����
*/
void LVL_GamePlayer::LVL_GameStartRound() {
    if (LVL_GamePlayerState_E::Out == enState) return emit sgnRoundEnded();
    emit sgnWantDeal(pstNewCard);
    if (LVL_InterCardType_E::none == pstNewCard->LVL_InterGetCardType()) return emit sgnRoundEnded();//û�еõ����ƣ������غ�
    if (LVL_GamePlayerState_E::Guarded == enState) LVL_GameUnguardPlayer();//����һ�غϱ���������ȡ������
    bIsOwnRound = true;
    emit sgnRoundStarted();
}

///////////////////////////////////////////////////////////////

/*
    ���ܣ�������ҹ��캯��
    ���ݣ�ʵ��������ҳ�����ع���
    ������parent-����������ڵ�ָ�룻qpSlot-�������Ͻ����ꣻqpDiscard-���ƶ����Ͻ����ꣻsCard{Width, Height}-���ƴ�С��sCardSpace-���ſ��Ƶļ��
*/
LVL_GameHuman::LVL_GameHuman(LVL_GameCtrller *parent, const QPoint &qpSlot, const QPoint &qpDiscard, const QSize &qsCardSize, int sCardSpace)
    : LVL_GamePlayer(parent, true, qpSlot, qpDiscard, qsCardSize, sCardSpace)
{ 
    LVL_InterBtn *pstConfirm = new LVL_InterBtn(parent, "", LVL_GAME_CONFIRM_BTN_GEO);//ȷ�ϳ��ư�ť
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
    ���ܣ����ݵ���Ŀ��Ƹ��ĳ���״̬
    ��������
    ����ֵ����
*/
void LVL_GameHuman::LVL_GameClickCard() {
    if (false == bIsOwnRound) return;//δ���Լ��غϲ���ѡ��
    
    //�ж�Ů��������
    LVL_InterCard* pstSender = qobject_cast<LVL_InterCard*>(sender());
    if (LVL_InterCardType_E::prince == pstSender->LVL_InterGetCardType() || LVL_InterCardType_E::king == pstSender->LVL_InterGetCardType()) {//ѡ����������ӻ����
        LVL_InterCard* pstOther = (pstNewCard == pstSender) ? pstOldCard : pstNewCard;
        if (LVL_InterCardType_E::countess == pstOther->LVL_InterGetCardType()) {//����һ�ſ���Ů����
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
        if (0 == pstCtrller->LVL_GameGetTargets(pstSelect->LVL_InterGetCardType(), this).length()) emit sgnTarget(NULL);//����Ŀ���û�п�ѡĿ�������ѡ��Ŀ��ʱ������ѡ��Ŀ��׶�
    }
}

/*
    ���ܣ����ݵ������Ҹ���Ŀ��ѡ��״̬
    ��������
    ����ֵ����
*/
void LVL_GameHuman::LVL_GameClickPortrait() {
    if (false == bIsOwnRound || NULL == pstSelect || false == LVL_InterCard::m_abNeedTarget[(int)(pstSelect->LVL_InterGetCardType())]) return;//δ���Լ��غϡ�δѡ�ƻ��������Ŀ��ʱ����ѡ�����
    
    LVL_GamePlayer* pstSender = dynamic_cast<LVL_GamePlayer*>(sender());//��ȡ��ѡ��Ŀ��
    if (this == pstSender && LVL_InterCardType_E::prince != pstSelect->LVL_InterGetCardType()) return;//�������ⲻ��ѡ���Լ���ΪĿ��
    if (NULL != pstTarget) emit sgnUntarget(pstTarget);//��ѡĿ��ʱ��ȡ����ǰѡ���Ŀ��
    if (pstSender == pstTarget) pstTarget = NULL;//������������ǰ�Ѿ�ѡ���Ŀ�꣬��ѡ��Ŀ���ÿ�
    else { pstTarget = pstSender; emit sgnTarget(pstSender);}//����ѡ������Ŀ��
}

///////////////////////////////////////////////////////////////

/*
    ���ܣ�AI��ҹ��캯��
    ���ݣ�ʵ��������ҳ�����ع���
    ������parent-����������ڵ�ָ�룻qpSlot-�������Ͻ����ꣻqpDiscard-���ƶ����Ͻ����ꣻsCard{Width, Height}-���ƴ�С��sCardSpace-���ſ��Ƶļ��
*/
LVL_GameAI::LVL_GameAI(LVL_GameCtrller *parent, const QPoint &qpSlot, const QPoint &qpDiscard, const QSize &qsCardSize, int sCardSpace)
    : LVL_GamePlayer(parent, false, qpSlot, qpDiscard, qsCardSize, sCardSpace)
{
    connect(this, &LVL_GameAI::sgnRoundStarted, [this] { QTimer::singleShot(LVL_GAME_AI_WAITING_MSEC, this, &LVL_GameAI::LVL_GameAIGetReady); });//ѡ�Ƶĵȴ�ʱ��
    connect(this, &LVL_GameAI::sgnFinish, [this] { QTimer::singleShot(LVL_GAME_AI_WAITING_MSEC, this, [this] {//ѡ�����
        pstSelect->LVL_InterSetCardKnown(false);//��ѡ���۱�ز��ɼ�
        LVL_GamePlayCard();//�������
        LVL_GameEndRound();//�غϽ���
    }); });
}

void LVL_GameAI::LVL_GameAIGetReady() {
    const LVL_GameRecord &stRecord = pstCtrller->LVL_GameGetRecord();

    //ѡ������
    LVL_InterCardType_E enOld = pstOldCard->LVL_InterGetCardType(), enNew = pstNewCard->LVL_InterGetCardType(), enSelect;
    if (LVL_InterCardType_E::princess == enNew//�����������
        || LVL_InterCardType_E::countess == enOld && (LVL_InterCardType_E::prince == enNew || LVL_InterCardType_E::king == enNew)) pstSelect = pstOldCard;//�ж�Ů����Ч��
    else if (LVL_InterCardType_E::princess == enOld//�����������
        || LVL_InterCardType_E::countess == enNew && (LVL_InterCardType_E::prince == enOld || LVL_InterCardType_E::king == enOld)) pstSelect = pstNewCard;//�ж�Ů����Ч��
    else pstSelect = (qrand() % HandCardNum) ? pstNewCard : pstOldCard;//������Ч��������ѡ����
    
    pstSelect->LVL_InterPopCard();//��������
    pstSelect->LVL_InterSetCardKnown(true);//ʹ���ƿ��ӻ�
    enSelect = pstSelect->LVL_InterGetCardType();

    //ѡ��Ŀ��
    QList<LVL_GamePlayer*> qlistTargets = pstCtrller->LVL_GameGetTargets(enSelect, this);
    if (LVL_InterCardType_E::prince == enSelect && qlistTargets.length() > 1) qlistTargets.removeAll(this);//���п��ܣ���������ʱ��ѡ�Լ�
    if (qlistTargets.length() > 0) pstTarget = qlistTargets.at(qrand() % qlistTargets.length());
    emit sgnTarget(pstTarget);//���Ŀ��
    emit sgnFinish();//AIѡ��׶ν���
}