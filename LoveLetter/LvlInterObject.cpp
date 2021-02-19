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
                              QStringLiteral("������1��\n[���á�ѡ��]ѡ��һ����ң���˵��һ�Ų��������Ŀ��ơ����Է����иÿ��ƣ���Է����֡�\n����ѡ����ֻ��ܱ�������ҡ�\n��û�п�ѡ������ʱ��Ч�����ᷢ����"),
                              QStringLiteral("��ʦ��2��\n[���á�ѡ��]ѡ��һ����ң��鿴�����ơ�ֻ��Ҫ���㿴��\n����ѡ����ֻ��ܱ�������ҡ�\n��û�п�ѡ������ʱ��Ч�����ᷢ����"),
                              QStringLiteral("�о���3��\n[���á�ѡ��]ѡ��һ����ң����Ǳ�ƴ���Ƶ�����������С��һ�����֣���������ͬ�����·�����\n��ƴ����ʱ˫���������Ƽ��ӡ�\n����ѡ����ֻ��ܱ�������ҡ�\n��û�п�ѡ������ʱ��Ч�����ᷢ����"),
                              QStringLiteral("Ů�ͣ�4��\n[����]�㽫��������ֱ������һ�غϿ�ʼǰ��������ҵĿ�������Ч�����ܶ�����Ч��"),
                              QStringLiteral("���ӣ�5��\n[���á�ѡ��]ѡ��һ����ң���ָ��������������Ʋ�����һ���ơ�\n��ʱ���Ƶ�����Ч��������Ч��\n���ƿ���û����ʱ������Ϸ��ʼʱ������һ���ơ�\n����ѡ����ֻ��ܱ�������ҡ�\n��û�п�ѡ������ʱ��Ч�����ᷢ����"),
                              QStringLiteral("������6��\n[���á�ѡ��]ѡ��һ����ң����ǽ������ơ�\n����ѡ����ֻ��ܱ�������ҡ�\n��û�п�ѡ������ʱ��Ч�����ᷢ����"),
                              QStringLiteral("Ů������7��\n[����]�����������⻹�����ӣ�5���������6��ʱ����ֻ�ܴ��Ů������"),
                              QStringLiteral("������8��\n[����]�������κ�ԭ������������ʱ���㽫ֱ�ӳ��֡�") };

const bool LVL_InterCard::m_abNeedTarget[(int)LVL_InterCardType_E::CardTypeNum] =
                          { false, true, true, true, false, true, true, false, false };

/*
    ���ܣ����ƹ��캯��
    ������parent-�����������ڵ�ָ�룻enType-�������ͣ�bKnown-���ƿɼ��ԣ�{x,y}-��������λ�ã�{width, height}-���ƴ�С
*/
LVL_InterCard::LVL_InterCard(QWidget *parent, LVL_InterCardType_E enType, bool bKnown, const QPoint &qpPos, const QSize &qsSize)
    : QLabel(parent), enType(enType), bKnown(bKnown), qgoeOpacity(this) {
    LVL_InterSetCardType(enType);//����
    setGeometry(QRect(qpPos, qsSize));//��Сλ��
    setGraphicsEffect(&qgoeOpacity);
    qgoeOpacity.setOpacity(1);//��͸����
    if(true == bKnown) setToolTip(m_aqstrCardHints[(int)enType]);//���ɼ������������ʾ
}

/*
    ���ܣ����ÿ�������
    ���ݣ����ÿ������ಢ���Ŀ���ͼƬ�����������ʾ
    ������enType-��Ҫ���õ�����
    ����ֵ����
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
    ���ܣ����ÿ��ƿɼ���
    ���ݣ����ÿ��ƿɼ��Բ����Ŀ���ͼƬ�����������ʾ
    ������bKnown-�Ƿ�ɼ�
    ����ֵ����
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
    ���ܣ������������Զ���
    ���ݣ�����һ���������Զ�����ָ�����ԡ�����ʱ�䡢��ʼֵ������ֵ���ֵ����
    ������propertyName-���ԣ�msecs-����ʱ�䣻startValue-��ʼֵ��endValue-����ֵ��easing-��ֵ����
    ����ֵ��QPropertyAnimation*-�������Ķ�����ָ��
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
    ���ܣ�������ת����
    ���ݣ�������������ת����������
    ������sMsecs-��������ʱ�䣻bWait-�Ƿ������Եȴ���������
    ����ֵ����
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
    ������parent-�����������ڵ�ָ�룻text-��ť�ϵ��֣�QRect-��ťλ�úʹ�С
*/
LVL_InterBtn::LVL_InterBtn(QWidget *parent, const QString &text, const QRect &qrGeo) : QPushButton(text, parent) { setGeometry(qrGeo); setCursor(Qt::PointingHandCursor); }

/*
    ���ܣ���ť���캯�����ӿ��ƶ����찴ť
    ������stCard-���ڹ���Ŀ��ƶ���
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