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
#define LVL_INTER_CARD_WIDTH 120//���ƿ��
#define LVL_INTER_CARD_HEIGHT 180//���Ƹ߶�
#define LVL_INTER_CARD_SPACE 0//���Ƽ��

class LVL_GamePlayer;//�����ǰ������

///////////////////////////////////////////////////////////////
//                       ��������                            //
///////////////////////////////////////////////////////////////

class LVL_InterCard : public QLabel
{
    Q_OBJECT

public:
    enum LVL_InterCardType_E { none = 0, guard, priest, baron, handmaid, prince, king, countess, princess, CardTypeNum };//���п�������
    const static QString m_qstrCardNames[CardTypeNum];//���п�������
    const static bool m_bNeedTarget[CardTypeNum];//ÿ�ſ��Ƽ����Ƿ���Ҫָ��Ŀ��

    LVL_InterCard(QWidget *parent = Q_NULLPTR, LVL_InterCardType_E enType = none, bool bKnown = true,
        int x = 0, int y = 0, int width = LVL_INTER_CARD_WIDTH, int height = LVL_INTER_CARD_HEIGHT);

    /*
        ���ܣ��ƶ����Ƶ�ָ��λ��
        ������{x,y}-��Ҫ�ƶ�����λ�ã��������Ļ���Ͻ�
        ����ֵ����
    */
    inline void LVL_InterMoveCard(const int x, const int y) { setGeometry(x, y, width(), height()); }
    /*
        ���ܣ���ȡ��������
        ��������
        ����ֵ��LVL_InterCardType_E-��������
    */
    inline LVL_InterCardType_E LVL_InterGetCardType() const { return enType; }
    void LVL_InterSetCardType(LVL_InterCardType_E enType);

    /*
        ���ܣ���ȡ���ƿɼ���
        ��������
        ����ֵ��bool-trueΪ�ɼ���falseΪ���ɼ�
    */
    inline bool LVL_InterGetCardKnown() const { return bKnown; }
    void LVL_InterSetCardKnown(bool bKnown);

protected:
    void mousePressEvent(QMouseEvent *event);

private:
    LVL_InterCardType_E enType;//��������
    bool bKnown;//�����Ƿ�ɼ�

signals:
    void sgnClicked();//�źź�������ʾ������������

public slots:
    void LVL_InterPopCard() { setGeometry(x(), y() - LVL_INTER_CARD_SHIFT, width(), height()); }//�ۺ������������ƣ���ʾ���Ʊ�ѡ��
    void LVL_InterWithdrawCard() { setGeometry(x(), y() + LVL_INTER_CARD_SHIFT, width(), height()); }//�ۺ������Żؿ��ƣ���ʾ���Ʊ�ȡ��ѡ��

    friend class LVL_GamePlayer;
};

///////////////////////////////////////////////////////////////
//                       ��ť����                            //
///////////////////////////////////////////////////////////////

class LVL_InterBtn : public QPushButton
{
    Q_OBJECT
public:
    LVL_InterBtn(QWidget *parent = Q_NULLPTR, const QString &text = "", const QRect &qpGeo = { 0, 0, 80, 20 });//���캯��
    LVL_InterBtn(const LVL_InterCard &stCard);//�ӿ��ƹ��찴ť
    
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