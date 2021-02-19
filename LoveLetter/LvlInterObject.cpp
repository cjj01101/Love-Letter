////////////////////////////////////////////////
//File Name           : LvlInterObject.cpp
//2nd Model Name      : Inter
//Description         : Implement all the interactable objects, including CARDS, BUTTONS.
//Lastest Update Time : 2021/2/14 16:36
//Version             : 1.4
////////////////////////////////////////////////

#include "LvlInterObject.h"

const QString LVL_InterCard::m_aqstrCardNames[(int)LVL_InterCardType_E::CardTypeNum] =
                            { "none", "guard", "priest", "baron", "handmaid", "prince", "king", "countess", "princess" };

const QString LVL_InterCard::m_aqstrCardHints[(int)LVL_InterCardType_E::CardTypeNum] = { "",
                              QStringLiteral("守卫（1）\n[弃置、选择]选择一名玩家，并说出一张不是守卫的卡牌。若对方持有该卡牌，则对方出局。\n不能选择出局或受保护的玩家。\n当没有可选择的玩家时，效果不会发生。"),
                              QStringLiteral("牧师（2）\n[弃置、选择]选择一名玩家，查看其手牌。只需要给你看。\n不能选择出局或受保护的玩家。\n当没有可选择的玩家时，效果不会发生。"),
                              QStringLiteral("男爵（3）\n[弃置、选择]选择一名玩家，你们比拼手牌点数，点数较小的一方出局；若点数相同则无事发生。\n比拼点数时双方交换手牌检视。\n不能选择出局或受保护的玩家。\n当没有可选择的玩家时，效果不会发生。"),
                              QStringLiteral("女仆（4）\n[弃置]你将被保护，直至你下一回合开始前，其他玩家的卡牌弃置效果不能对你生效。"),
                              QStringLiteral("王子（5）\n[弃置、选择]选择一名玩家，被指定的玩家弃掉手牌并重摸一张牌。\n此时卡牌的弃置效果不会生效。\n当牌库中没有牌时，摸游戏开始时弃掉的一张牌。\n不能选择出局或受保护的玩家。\n当没有可选择的玩家时，效果不会发生。"),
                              QStringLiteral("国王（6）\n[弃置、选择]选择一名玩家，你们交换手牌。\n不能选择出局或受保护的玩家。\n当没有可选择的玩家时，效果不会发生。"),
                              QStringLiteral("女伯爵（7）\n[被动]当你手上另外还有王子（5）或国王（6）时，你只能打出女伯爵。"),
                              QStringLiteral("公主（8）\n[被动]当你因任何原因弃置这张牌时，你将直接出局。") };

const bool LVL_InterCard::m_abNeedTarget[(int)LVL_InterCardType_E::CardTypeNum] =
                          { false, true, true, true, false, true, true, false, false };

/*
    功能：卡牌构造函数
    参数：parent-卡牌所属窗口的指针；enType-卡牌类型；bKnown-卡牌可见性；{x,y}-卡牌所在位置；{width, height}-卡牌大小
*/
LVL_InterCard::LVL_InterCard(QWidget *parent, LVL_InterCardType_E enType, bool bKnown, const QPoint &qpPos, const QSize &qsSize)
    : QLabel(parent), enType(enType), bKnown(bKnown), qgoeOpacity(this) {
    LVL_InterSetCardType(enType);//种类
    setGeometry(QRect(qpPos, qsSize));//大小位置
    setGraphicsEffect(&qgoeOpacity);
    qgoeOpacity.setOpacity(1);//不透明度
    if(true == bKnown) setToolTip(m_aqstrCardHints[(int)enType]);//若可见，增加鼠标提示
}

/*
    功能：设置卡牌种类
    内容：设置卡牌种类并更改卡面图片和鼠标悬浮提示
    参数：enType-需要设置的种类
    返回值：无
*/
void LVL_InterCard::LVL_InterSetCardType(LVL_InterCardType_E enType) {
    this->enType = enType;
    setToolTip(m_aqstrCardHints[bKnown ? (int)enType : 0]);
    if (bKnown || enType == LVL_InterCardType_E::none) {
        QString qstrPicName(LVL_PIC_DIR + m_aqstrCardNames[(int)enType]);
        setStyleSheet(QString("QLabel{border-image:url(%1);}""QLabel:hover{border-image:url(%2);}").arg(qstrPicName).arg(qstrPicName + LVL_PIC_FOCUSED));
    }
    else setStyleSheet(QString("QLabel{border-image:url(%1);}""QLabel:hover{border-image:url(%1);}").arg(LVL_PIC_CARD_BACK));
}

/*
    功能：设置卡牌可见性
    内容：设置卡牌可见性并更改卡面图片和鼠标悬浮提示
    参数：bKnown-是否可见
    返回值：无
*/
void LVL_InterCard::LVL_InterSetCardKnown(bool bKnown) {
    this->bKnown = bKnown;
    setToolTip(m_aqstrCardHints[bKnown ? (int)enType : 0]);
    if (bKnown) {
        QString qstrPicName(LVL_PIC_DIR + m_aqstrCardNames[(int)enType]);
        setStyleSheet(QString("QLabel{border-image:url(%1);}""QLabel:hover{border-image:url(%2);}").arg(qstrPicName).arg(qstrPicName + LVL_PIC_FOCUSED));
    }
    else setStyleSheet(QString("QLabel{border-image:url(%1);}""QLabel:hover{border-image:url(%1);}").arg(LVL_PIC_CARD_BACK));
}

/*
    功能：创建卡牌属性动画
    内容：创建一个卡牌属性动画，指定属性、持续时间、开始值、结束值与插值曲线
    参数：propertyName-属性；msecs-持续时间；startValue-开始值；endValue-结束值；easing-插值曲线
    返回值：QPropertyAnimation*-所创建的动画的指针
*/
QPropertyAnimation *LVL_InterCard::LVL_InterCreateAnimation(const QByteArray& propertyName, int msecs, const QVariant &startValue, const QVariant &endValue, const QEasingCurve &easing) {
    QPropertyAnimation *pqpaAnime = new QPropertyAnimation(this, propertyName, this);
    pqpaAnime->setDuration(msecs);
    pqpaAnime->setStartValue(startValue);
    pqpaAnime->setEndValue(endValue);
    pqpaAnime->setEasingCurve(easing);
    return pqpaAnime;
}

/*
    功能：创建翻转动画
    内容：创建动画，翻转卡牌正反面
    参数：sMsecs-动画持续时间；bWait-是否阻塞以等待动画结束
    返回值：无
*/
QSequentialAnimationGroup* LVL_InterCard::LVL_InterCreateFilppingAnimation(int sMsecs) {
    QRect qrShrink = geometry();
    qrShrink.setX(qrShrink.x() + qrShrink.width() / 2);
    qrShrink.setWidth(0);
    QSequentialAnimationGroup *pqsagFilpping = new QSequentialAnimationGroup(this);

    QPropertyAnimation *pqpaShrink = LVL_InterCreateAnimation("geometry", sMsecs, geometry(), qrShrink, QEasingCurve::InQuad);
    QPropertyAnimation *pqpaExpand = LVL_InterCreateAnimation("geometry", sMsecs, qrShrink, geometry(), QEasingCurve::OutQuad);
    connect(pqpaShrink, &QAbstractAnimation::finished, [this] { LVL_InterSetCardKnown(!bKnown); });

    pqsagFilpping->addAnimation(pqpaShrink);
    pqsagFilpping->addAnimation(pqpaExpand);
    return pqsagFilpping;
}

/*
    功能：处理鼠标在卡牌处按下的事件
    参数：event-鼠标事件的具体内容
    返回值：无
*/
void LVL_InterCard::mousePressEvent(QMouseEvent *event) {
    if (Qt::LeftButton == event->button()) emit sgnClicked();
}

///////////////////////////////////////////////////////////////

/*
    功能：按钮构造函数
    参数：parent-卡牌所属窗口的指针；text-按钮上的字；QRect-按钮位置和大小
*/
LVL_InterBtn::LVL_InterBtn(QWidget *parent, const QString &text, const QRect &qrGeo) : QPushButton(text, parent) { setGeometry(qrGeo); setCursor(Qt::PointingHandCursor); }

/*
    功能：按钮构造函数，从卡牌对象构造按钮
    参数：stCard-用于构造的卡牌对象
*/
LVL_InterBtn::LVL_InterBtn(const LVL_InterCard &stCard) : QPushButton(qobject_cast<QWidget*>(stCard.parent())) {
    setGeometry(stCard.geometry());
    setToolTip(LVL_InterCard::m_aqstrCardHints[(int)(stCard.LVL_InterGetCardType())]);
    QString qstrPicName(LVL_PIC_DIR + LVL_InterCard::m_aqstrCardNames[(int)(stCard.LVL_InterGetCardType())]);
    setStyleSheet(QString("QPushButton{border-image:url(%1);}"
        "QPushButton:hover{border-image:url(%2);}"
        "QPushButton:checked{border-image:url(%3);}")
        .arg(qstrPicName).arg(qstrPicName + LVL_PIC_FOCUSED).arg(qstrPicName + LVL_PIC_FOCUSED));
}