////////////////////////////////////////////////
//File Name           : LvlInterObject.h
//2nd Model Name      : Inter
//Description         : Define all the interactable objects, including CARDS, BUTTONS.
//Lastest Update Time : 2021/2/14 16:36
//Version             : 1.4
////////////////////////////////////////////////

#pragma once

#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QPauseAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QMouseEvent>

#define LVL_PIC_DIR          (QString("./Pictures/"))
#define LVL_PIC_START_BGD    (LVL_PIC_DIR + QString("start_background"))
#define LVL_PIC_GAME_BGD     (LVL_PIC_DIR + QString("game_background"))
#define LVL_PIC_DEFAULT_BGD  (LVL_PIC_DIR + QString("game_background"))
#define LVL_PIC_GUESSER      (LVL_PIC_DIR + QString("guesser"))
#define LVL_PIC_PLAYER_PORT  (LVL_PIC_DIR + QString("avatar_black"))
#define LVL_PIC_AI1_PORT     (LVL_PIC_DIR + QString("avatar_gold"))
#define LVL_PIC_AI2_PORT     (LVL_PIC_DIR + QString("avatar_green"))
#define LVL_PIC_AI3_PORT     (LVL_PIC_DIR + QString("avatar_silver"))
#define LVL_PIC_MENU_BTN     (LVL_PIC_DIR + QString("button"))
#define LVL_PIC_CONFIRM_BTN  (LVL_PIC_DIR + QString("confirm_button"))
#define LVL_PIC_CARD_BACK    (LVL_PIC_DIR + QString("back"))
#define LVL_PIC_FOCUSED      (QString("_focused"))
#define LVL_PIC_CLICKED      (QString("_clicked"))
#define LVL_PIC_TARGETED     (QString("_targeted"))
#define LVL_PIC_GUARDED      (QString("_guarded"))
#define LVL_PIC_OUT          (QString("_out"))

#define LVL_INTER_CARD_SHIFT   (50)
#define LVL_INTER_CARD_WIDTH   (120)//卡牌宽度
#define LVL_INTER_CARD_HEIGHT  (180)//卡牌高度
#define LVL_INTER_CARD_SPACE   (0)//卡牌间隔

////////////////////////////////////////////////////////////
//                       卡牌类型                         //
////////////////////////////////////////////////////////////

enum class LVL_InterCardType_E { none = 0, guard, priest, baron, handmaid, prince, king, countess, princess, CardTypeNum };//所有卡牌种类

class LVL_InterCard : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(bool known MEMBER bKnown READ LVL_InterGetCardKnown WRITE LVL_InterSetCardKnown)
    Q_PROPERTY(qreal opacity READ LVL_InterGetCardOpacity WRITE LVL_InterSetCardOpacity)

public:
    const static QString m_aqstrCardNames[(int)LVL_InterCardType_E::CardTypeNum];//所有卡牌名称
    const static QString m_aqstrCardHints[(int)LVL_InterCardType_E::CardTypeNum];//卡牌提示
    const static bool m_abNeedTarget[(int)LVL_InterCardType_E::CardTypeNum];//每张卡牌技能是否需要指定目标

    LVL_InterCard(QWidget *parent = Q_NULLPTR, LVL_InterCardType_E enType = LVL_InterCardType_E::none, bool bKnown = true,
        const QPoint &qpPos = { 0, 0 }, const QSize &qsSize = { LVL_INTER_CARD_WIDTH, LVL_INTER_CARD_HEIGHT });

    inline void LVL_InterSetCardPos(const QPoint &qpPos);
    inline void LVL_InterSetCardPos(int x, int y);

    inline LVL_InterCardType_E LVL_InterGetCardType() const;
    void LVL_InterSetCardType(LVL_InterCardType_E enType);

    inline bool LVL_InterGetCardKnown() const;
    void LVL_InterSetCardKnown(bool bKnown);

    inline qreal LVL_InterGetCardOpacity() const;
    inline void LVL_InterSetCardOpacity(qreal rOpacity);

    QPropertyAnimation *LVL_InterCreateAnimation(const QByteArray &propertyName, int msecs, const QVariant &startValue, const QVariant &endValue,
        const QEasingCurve &easing = QEasingCurve::Linear);//创建动画
    QSequentialAnimationGroup* LVL_InterCreateFilppingAnimation(int sMsecs);//翻转卡牌动画

protected:
    void mousePressEvent(QMouseEvent *event);

private:
    LVL_InterCardType_E enType;//卡牌种类
    bool bKnown;//牌面可见性
    QGraphicsOpacityEffect qgoeOpacity;//不透明度

signals:
    void sgnClicked();//信号函数，表示鼠标左键被按下

public slots:
    void LVL_InterPopCard() { move(pos() - QPoint(0, LVL_INTER_CARD_SHIFT)); }//槽函数，弹出卡牌，表示卡牌被选择
    void LVL_InterWithdrawCard() { move(pos() + QPoint(0, LVL_INTER_CARD_SHIFT)); }//槽函数，放回卡牌，表示卡牌被取消选择
};

/*
     功能：移动卡牌到指定位置
     参数：qpPos/{x,y}-需要移动到的位置，相对于窗口左上角
     返回值：无
 */
inline void LVL_InterCard::LVL_InterSetCardPos(const QPoint &qpPos) { move(qpPos); }
inline void LVL_InterCard::LVL_InterSetCardPos(int x, int y) { move(x, y); }

/*
    功能：获取卡牌种类
    参数：无
    返回值：LVL_InterCardType_E-卡牌种类
*/
inline LVL_InterCardType_E LVL_InterCard::LVL_InterGetCardType() const { return enType; }

/*
    功能：获取卡牌可见性
    参数：无
    返回值：bool-true为可见，false为不可见
*/
inline bool LVL_InterCard::LVL_InterGetCardKnown() const { return bKnown; }

/*
    功能：获取卡牌不透明度
    参数：无
    返回值：qreal-0至1之间的不透明度，0为完全透明，1为完全不透明
*/
inline qreal LVL_InterCard::LVL_InterGetCardOpacity() const { return qgoeOpacity.opacity(); }

/*
    功能：设置卡牌不透明度
    参数：rOpacity-0至1之间的不透明度，0为完全透明，1为完全不透明
    返回值：无
*/
inline void LVL_InterCard::LVL_InterSetCardOpacity(qreal rOpacity) { qgoeOpacity.setOpacity(rOpacity); }

////////////////////////////////////////////////////////////
//                       按钮类型                         //
////////////////////////////////////////////////////////////

class LVL_InterBtn : public QPushButton
{
    Q_OBJECT

public:
    LVL_InterBtn(QWidget *parent = Q_NULLPTR, const QString &text = "", const QRect &qrGeo = { 0, 0, 80, 20 });//构造函数
    explicit LVL_InterBtn(const LVL_InterCard &stCard);//从卡牌构造按钮
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