////////////////////////////////////////////////
//File Name           : LVLInterObject.h
//2nd Model Name      : Inter
//Description         : Define all the interactable objects, including CARDS, BUTTONS.
//Lastest Update Time : 2020/12/30 12:35
//Version             : 1.3
////////////////////////////////////////////////

#pragma once

#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include <QImage>
#include <QMetaObject>
#include <QState>
#include <QStateMachine>

#define LVL_PIC_DIR          "./Pictures/"
#define LVL_PIC_BACKGROUND   LVL_PIC_DIR"background"
#define LVL_PIC_BUTTON       LVL_PIC_DIR"button"
#define LVL_PIC_CARD_BACK    LVL_PIC_DIR"back"
#define LVL_PIC_GUESSER      LVL_PIC_DIR"guesser"
#define LVL_PIC_FOCUSED      "_focused"
#define LVL_PIC_CLICKED      "_clicked"
#define LVL_PIC_TARGETED     "_targeted"
#define LVL_PIC_GUARDED      "_guarded"
#define LVL_PIC_OUT          "_out"
#define LVL_PIC_PLAYER_PORT  LVL_PIC_DIR"avatar_black"
#define LVL_PIC_AI1_PORT     LVL_PIC_DIR"avatar_gold"
#define LVL_PIC_AI2_PORT     LVL_PIC_DIR"avatar_green"
#define LVL_PIC_AI3_PORT     LVL_PIC_DIR"avatar_silver"

#define LVL_INTER_CARD_SHIFT 50
#define LVL_INTER_CARD_WIDTH 120//卡牌宽度
#define LVL_INTER_CARD_HEIGHT 180//卡牌高度
#define LVL_INTER_CARD_SPACE 0//卡牌间隔

class LVL_GamePlayer;//玩家类前向声明

///////////////////////////////////////////////////////////////
//                       卡牌类型                            //
///////////////////////////////////////////////////////////////

class LVL_InterCard : public QLabel
{
    Q_OBJECT

public:
    enum LVL_InterCardType_E { none = 0, guard, priest, baron, handmaid, prince, king, countess, princess, CardTypeNum };//所有卡牌种类
    const static QString m_qstrCardNames[CardTypeNum];//所有卡牌名称
    const static bool m_bNeedTarget[CardTypeNum];//每张卡牌技能是否需要指定目标

    LVL_InterCard(QWidget *parent = Q_NULLPTR, LVL_InterCardType_E enType = none, bool bKnown = true,
        int x = 0, int y = 0, int width = LVL_INTER_CARD_WIDTH, int height = LVL_INTER_CARD_HEIGHT);

    /*
        功能：移动卡牌到指定位置
        参数：{x,y}-需要移动到的位置，相对于屏幕左上角
        返回值：无
    */
    inline void LVL_InterMoveCard(const int x, const int y) { setGeometry(x, y, width(), height()); }
    /*
        功能：获取卡牌种类
        参数：无
        返回值：LVL_InterCardType_E-卡牌种类
    */
    inline LVL_InterCardType_E LVL_InterGetCardType() const { return enType; }
    void LVL_InterSetCardType(LVL_InterCardType_E enType);

    /*
        功能：获取卡牌可见性
        参数：无
        返回值：bool-true为可见，false为不可见
    */
    inline bool LVL_InterGetCardKnown() const { return bKnown; }
    void LVL_InterSetCardKnown(bool bKnown);

protected:
    void mousePressEvent(QMouseEvent *event);

private:
    LVL_InterCardType_E enType;//卡牌种类
    bool bKnown;//牌面是否可见

signals:
    void sgnClicked();//信号函数，表示鼠标左键被按下

public slots:
    void LVL_InterPopCard() { setGeometry(x(), y() - LVL_INTER_CARD_SHIFT, width(), height()); }//槽函数，弹出卡牌，表示卡牌被选择
    void LVL_InterWithdrawCard() { setGeometry(x(), y() + LVL_INTER_CARD_SHIFT, width(), height()); }//槽函数，放回卡牌，表示卡牌被取消选择

    friend class LVL_GamePlayer;
};

///////////////////////////////////////////////////////////////
//                       按钮类型                            //
///////////////////////////////////////////////////////////////

class LVL_InterBtn : public QPushButton
{
    Q_OBJECT
public:
    LVL_InterBtn(QWidget *parent = Q_NULLPTR, const QString &text = "", const QRect &qpGeo = { 0, 0, 80, 20 });//构造函数
    LVL_InterBtn(const LVL_InterCard &stCard);//从卡牌构造按钮
    
    inline void LVL_InterSetBtnPic(const QString &qstrDefaultPic = "", const QString &qstrHoverPic = "", const QString &qstrPressedPic = "");//设置按钮图片
};

/*
    功能：设置按钮图片
    参数：qstrDefaultPic-默认图片；qstrHoverPic-鼠标悬停时的图片；qstrPressedPic-鼠标按下时的图片
    返回值：无
*/
inline void LVL_InterBtn::LVL_InterSetBtnPic(const QString &qstrDefaultPic, const QString &qstrHoverPic, const QString &qstrPressedPic) {
    setStyleSheet(QString("QPushButton{border-image:url(%1);}"
        "QPushButton:hover{border-image:url(%2);}"
        "QPushButton:pressed{border-image:url(%3);}")
        .arg(qstrDefaultPic).arg(qstrHoverPic).arg(qstrPressedPic));
}