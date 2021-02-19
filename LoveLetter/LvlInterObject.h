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
#define LVL_INTER_CARD_WIDTH   (120)//���ƿ��
#define LVL_INTER_CARD_HEIGHT  (180)//���Ƹ߶�
#define LVL_INTER_CARD_SPACE   (0)//���Ƽ��

////////////////////////////////////////////////////////////
//                       ��������                         //
////////////////////////////////////////////////////////////

enum class LVL_InterCardType_E { none = 0, guard, priest, baron, handmaid, prince, king, countess, princess, CardTypeNum };//���п�������

class LVL_InterCard : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(bool known MEMBER bKnown READ LVL_InterGetCardKnown WRITE LVL_InterSetCardKnown)
    Q_PROPERTY(qreal opacity READ LVL_InterGetCardOpacity WRITE LVL_InterSetCardOpacity)

public:
    const static QString m_aqstrCardNames[(int)LVL_InterCardType_E::CardTypeNum];//���п�������
    const static QString m_aqstrCardHints[(int)LVL_InterCardType_E::CardTypeNum];//������ʾ
    const static bool m_abNeedTarget[(int)LVL_InterCardType_E::CardTypeNum];//ÿ�ſ��Ƽ����Ƿ���Ҫָ��Ŀ��

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
        const QEasingCurve &easing = QEasingCurve::Linear);//��������
    QSequentialAnimationGroup* LVL_InterCreateFilppingAnimation(int sMsecs);//��ת���ƶ���

protected:
    void mousePressEvent(QMouseEvent *event);

private:
    LVL_InterCardType_E enType;//��������
    bool bKnown;//����ɼ���
    QGraphicsOpacityEffect qgoeOpacity;//��͸����

signals:
    void sgnClicked();//�źź�������ʾ������������

public slots:
    void LVL_InterPopCard() { move(pos() - QPoint(0, LVL_INTER_CARD_SHIFT)); }//�ۺ������������ƣ���ʾ���Ʊ�ѡ��
    void LVL_InterWithdrawCard() { move(pos() + QPoint(0, LVL_INTER_CARD_SHIFT)); }//�ۺ������Żؿ��ƣ���ʾ���Ʊ�ȡ��ѡ��
};

/*
     ���ܣ��ƶ����Ƶ�ָ��λ��
     ������qpPos/{x,y}-��Ҫ�ƶ�����λ�ã�����ڴ������Ͻ�
     ����ֵ����
 */
inline void LVL_InterCard::LVL_InterSetCardPos(const QPoint &qpPos) { move(qpPos); }
inline void LVL_InterCard::LVL_InterSetCardPos(int x, int y) { move(x, y); }

/*
    ���ܣ���ȡ��������
    ��������
    ����ֵ��LVL_InterCardType_E-��������
*/
inline LVL_InterCardType_E LVL_InterCard::LVL_InterGetCardType() const { return enType; }

/*
    ���ܣ���ȡ���ƿɼ���
    ��������
    ����ֵ��bool-trueΪ�ɼ���falseΪ���ɼ�
*/
inline bool LVL_InterCard::LVL_InterGetCardKnown() const { return bKnown; }

/*
    ���ܣ���ȡ���Ʋ�͸����
    ��������
    ����ֵ��qreal-0��1֮��Ĳ�͸���ȣ�0Ϊ��ȫ͸����1Ϊ��ȫ��͸��
*/
inline qreal LVL_InterCard::LVL_InterGetCardOpacity() const { return qgoeOpacity.opacity(); }

/*
    ���ܣ����ÿ��Ʋ�͸����
    ������rOpacity-0��1֮��Ĳ�͸���ȣ�0Ϊ��ȫ͸����1Ϊ��ȫ��͸��
    ����ֵ����
*/
inline void LVL_InterCard::LVL_InterSetCardOpacity(qreal rOpacity) { qgoeOpacity.setOpacity(rOpacity); }

////////////////////////////////////////////////////////////
//                       ��ť����                         //
////////////////////////////////////////////////////////////

class LVL_InterBtn : public QPushButton
{
    Q_OBJECT

public:
    LVL_InterBtn(QWidget *parent = Q_NULLPTR, const QString &text = "", const QRect &qrGeo = { 0, 0, 80, 20 });//���캯��
    explicit LVL_InterBtn(const LVL_InterCard &stCard);//�ӿ��ƹ��찴ť
    inline void LVL_InterSetBtnPic(const QString &qstrDefaultPic = "", const QString &qstrHoverPic = "", const QString &qstrPressedPic = "");//���ð�ťͼƬ
};

/*
    ���ܣ����ð�ťͼƬ
    ������qstrDefaultPic-Ĭ��ͼƬ��qstrHoverPic-�����ͣʱ��ͼƬ��qstrPressedPic-��갴��ʱ��ͼƬ
    ����ֵ����
*/
inline void LVL_InterBtn::LVL_InterSetBtnPic(const QString &qstrDefaultPic, const QString &qstrHoverPic, const QString &qstrPressedPic) {
    setStyleSheet(QString("QPushButton{border-image:url(%1);}"
        "QPushButton:hover{border-image:url(%2);}"
        "QPushButton:pressed{border-image:url(%3);}")
        .arg(qstrDefaultPic).arg(qstrHoverPic).arg(qstrPressedPic));
}