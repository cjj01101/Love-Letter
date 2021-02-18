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
    ���ܣ����ƹ��캯��
    ������parent-�����������ڵ�ָ�룻enType-�������ͣ�{x,y}-��������λ�ã�{width, heigh}-���ƴ�С
*/
LVL_InterCard::LVL_InterCard(QWidget *parent, LVL_InterCardType_E enType, bool bKnown, int x, int y, int width, int height)
    : QLabel(parent), enType(enType), bKnown(bKnown) {
    LVL_InterSetCardType(enType);//����
    setFixedSize(width, height);//��С
    setGeometry(x, y, this->width(), this->height());//λ��
    setAlignment(Qt::AlignCenter);//���뷽ʽ
}

/*
    ���ܣ����ÿ�������
    ���ݣ����ÿ������ಢ���Ŀ���ͼƬ
    ������enType-��Ҫ���õ�����
    ����ֵ����
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
    ���ܣ����ÿ��ƿɼ���
    ���ݣ����ÿ��ƿɼ��Բ����Ŀ���ͼƬ
    ������b-�Ƿ�ɼ�
    ����ֵ����
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
    ���ܣ���������ڿ��ƴ����µ��¼�
    ������event-����¼��ľ�������
    ����ֵ����
*/
void LVL_InterCard::mousePressEvent(QMouseEvent *event) {
    if (Qt::LeftButton == event->button()) emit sgnClicked();
}

///////////////////////////////////////////////////////////////

/*
    ���ܣ���ť���캯��
    ������parent-�����������ڵ�ָ�룻text-��ť�ϵ��֣�{x,y}-��ť����λ�ã�{width, heigh}-��ť��С
*/
LVL_InterBtn::LVL_InterBtn(QWidget *parent, const QString &text, const QRect &qpGeo) : QPushButton(text, parent) { setGeometry(qpGeo); }

/*
    ���ܣ���ť���캯�����ӿ��ƶ����찴ť
    ������pstCard-���ڹ���Ŀ��ƶ���
*/
LVL_InterBtn::LVL_InterBtn(const LVL_InterCard &stCard) : QPushButton(dynamic_cast<QWidget*>(stCard.parent())) {
    setGeometry(stCard.geometry());
    QString str(LVL_PIC_DIR + LVL_InterCard::m_qstrCardNames[stCard.LVL_InterGetCardType()]);
    setStyleSheet(QString("QPushButton{border-image:url(%1);}"
        "QPushButton:hover{border-image:url(%2);}"
        "QPushButton:checked{border-image:url(%3);}")
        .arg(str).arg(str + LVL_PIC_FOCUSED).arg(str + LVL_PIC_FOCUSED));
}