////////////////////////////////////////////////
//File Name           : LVLGameMain.cpp
//2nd Model Name      : Game
//Description         : Implement all the interactable objects, including CARDS, BUTTONS.
//Lastest Update Time : 2020/12/30 12:35
//Version             : 1.3
////////////////////////////////////////////////

#include "LvlInterObject.h"
#include "LvlGamePlayer.h"

const QString LVL_InterCard::m_qstrCardNames[CardTypeNum] =
                            { "none", "guard", "priest", "baron", "handmaid", "prince", "king", "countess", "princess" };

const bool LVL_InterCard::m_bNeedTarget[CardTypeNum] =
                          { false, true, true, true, false, true, true, false, false };

/*
    功能：卡牌构造函数
    参数：parent-卡牌所属窗口的指针；enType-卡牌类型；{x,y}-卡牌所在位置；{width, heigh}-卡牌大小
*/
LVL_InterCard::LVL_InterCard(QWidget *parent, LVL_InterCardType_E enType, bool bKnown, int x, int y, int width, int height)
    : QLabel(parent), enType(enType), bKnown(bKnown) {
    LVL_InterSetCardType(enType);//种类
    setFixedSize(width, height);//大小
    setGeometry(x, y, this->width(), this->height());//位置
    setAlignment(Qt::AlignCenter);//对齐方式
}

/*
    功能：设置卡牌种类
    内容：设置卡牌种类并更改卡面图片
    参数：enType-需要设置的种类
    返回值：无
*/
void LVL_InterCard::LVL_InterSetCardType(LVL_InterCardType_E enType) {
    this->enType = enType;
    if (bKnown || enType == none) {
        QString str(LVL_PIC_DIR + m_qstrCardNames[enType]);
        setStyleSheet(QString("QLabel{border-image:url(%1);}""QLabel:hover{border-image:url(%2);}").arg(str).arg(str + LVL_PIC_FOCUSED));
    }
    else setStyleSheet(QString("QLabel{border-image:url(%1);}""QLabel:hover{border-image:url(%1);}").arg(LVL_PIC_CARD_BACK));
}

/*
    功能：设置卡牌可见性
    内容：设置卡牌可见性并更改卡面图片
    参数：b-是否可见
    返回值：无
*/
void LVL_InterCard::LVL_InterSetCardKnown(bool bKnown) {
    this->bKnown = bKnown;
    if (bKnown) {
        QString str(LVL_PIC_DIR + m_qstrCardNames[enType]);
        setStyleSheet(QString("QLabel{border-image:url(%1);}""QLabel:hover{border-image:url(%2);}").arg(str).arg(str + LVL_PIC_FOCUSED));
    }
    else setStyleSheet(QString("QLabel{border-image:url(%1);}""QLabel:hover{border-image:url(%1);}").arg(LVL_PIC_CARD_BACK));
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
    参数：parent-卡牌所属窗口的指针；text-按钮上的字；{x,y}-按钮所在位置；{width, heigh}-按钮大小
*/
LVL_InterBtn::LVL_InterBtn(QWidget *parent, const QString &text, const QRect &qpGeo) : QPushButton(text, parent) { setGeometry(qpGeo); }

/*
    功能：按钮构造函数，从卡牌对象构造按钮
    参数：pstCard-用于构造的卡牌对象
*/
LVL_InterBtn::LVL_InterBtn(const LVL_InterCard &stCard) : QPushButton(dynamic_cast<QWidget*>(stCard.parent())) {
    setGeometry(stCard.geometry());
    QString str(LVL_PIC_DIR + LVL_InterCard::m_qstrCardNames[stCard.LVL_InterGetCardType()]);
    setStyleSheet(QString("QPushButton{border-image:url(%1);}"
        "QPushButton:hover{border-image:url(%2);}"
        "QPushButton:checked{border-image:url(%3);}")
        .arg(str).arg(str + LVL_PIC_FOCUSED).arg(str + LVL_PIC_FOCUSED));
}