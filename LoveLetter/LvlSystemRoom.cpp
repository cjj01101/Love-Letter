////////////////////////////////////////////////
//File Name           : LvlSystemRoom.cpp
//2nd Model Name      : System
//Description         : Implement all the rooms in the program.
//Lastest Update Time : 2021/2/14 16:36
//Version             : 1.0
////////////////////////////////////////////////

#include "LvlSystemRoom.h"

///////////////////////////////////////////////////////////////

/*
    ���ܣ������ڹ��캯��
    ������parent-�����ڵ�ָ��
    ���ݣ����ñ�����������Ϸ������
*/
LoveLetter::LoveLetter(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::LoveLetterClass)
{
    ui->setupUi(this);
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));//��������

    //��ʼ������
    LVL_SystemRoomManager *pqswRooms = new LVL_SystemRoomManager(this);
    LVL_SYSTEM_CREATE_ROOM(pqswRooms, StartRoom);
    LVL_SYSTEM_CREATE_ROOM(pqswRooms, GameRoom);
    LVL_SYSTEM_CREATE_ROOM(pqswRooms, OptionRoom);
    pqswRooms->setCurrentWidget(pstStartRoom);
}

LoveLetter::~LoveLetter()
{
    delete ui;
}

///////////////////////////////////////////////////////////////

/*
    ���ܣ�������๹�캯��
    ���ݣ����Ʒ�����汳��
    ������parent-�����ڵ�ָ��
*/
LVL_SystemAbstractRoom::LVL_SystemAbstractRoom(QWidget *parent, const QString &qstrBackground) : QWidget(parent) {
    setGeometry(0, 0, LVL_GAME_WINDOW_WIDTH, LVL_GAME_WINDOW_HEIGHT);

    //���ñ���
    setAutoFillBackground(true);
    QPalette qpalBackground = palette();
    qpalBackground.setBrush(QPalette::Background, QBrush(QPixmap(qstrBackground).scaled(size())));
    setPalette(qpalBackground);
}

///////////////////////////////////////////////////////////////

/*
    ���ܣ���ʼ���乹�캯��
    ���ݣ����ƿ�ʼ���汳�������������ť
    ������parent-�����ڵ�ָ��
*/
LVL_SystemStartRoom::LVL_SystemStartRoom(QWidget *parent) : LVL_SystemAbstractRoom(parent, LVL_PIC_START_BGD) {
    LVL_InterBtn *pstPlay = new LVL_InterBtn(this, QStringLiteral("��ʼ��Ϸ"), LVL_SYSTEM_START_PLAY_GEO);//
    pstPlay->LVL_InterSetBtnPic(LVL_PIC_MENU_BTN, LVL_PIC_MENU_BTN + LVL_PIC_FOCUSED, LVL_PIC_MENU_BTN + LVL_PIC_CLICKED);
    connect(pstPlay, &LVL_InterBtn::clicked, [this] { emit sgnChangeRoom("GameRoom"); });

    LVL_InterBtn *pstSetting = new LVL_InterBtn(this, QStringLiteral("����"), LVL_SYSTEM_START_OPTION_GEO);
    pstSetting->LVL_InterSetBtnPic(LVL_PIC_MENU_BTN, LVL_PIC_MENU_BTN + LVL_PIC_FOCUSED, LVL_PIC_MENU_BTN + LVL_PIC_CLICKED);
    connect(pstSetting, &LVL_InterBtn::clicked, [this] { emit sgnChangeRoom("OptionRoom"); });

    LVL_InterBtn *pstExit = new LVL_InterBtn(this, QStringLiteral("�˳�"), LVL_SYSTEM_START_EXIT_GEO);
    pstExit->LVL_InterSetBtnPic(LVL_PIC_MENU_BTN, LVL_PIC_MENU_BTN + LVL_PIC_FOCUSED, LVL_PIC_MENU_BTN + LVL_PIC_CLICKED);
    connect(pstExit, &LVL_InterBtn::clicked, window(), &QWidget::close);

    //QVBoxLayout *pqloButtons = new QVBoxLayout(this);
    //pqloButtons->setContentsMargins(
    //    (LVL_GAME_WINDOW_WIDTH - LVL_SYSTEM_START_BTN_WIDTH) / 2, (LVL_GAME_WINDOW_HEIGHT / 2 - 80), (LVL_GAME_WINDOW_WIDTH - LVL_SYSTEM_START_BTN_WIDTH) / 2, 0);
    //pqloButtons->setSpacing(120);
    //pqloButtons->addWidget(pstPlay);
    //pqloButtons->addWidget(pstSetting);
    //pqloButtons->addWidget(pstExit);
    //pqloButtons->addStretch();
    //setLayout(pqloButtons);
}

///////////////////////////////////////////////////////////////

/*
    ���ܣ���Ϸ���乹�캯��
    ���ݣ�������Ϸ���汳�������������ť
    ������parent-�����ڵ�ָ��
*/
LVL_SystemGameRoom::LVL_SystemGameRoom(QWidget *parent) : LVL_SystemAbstractRoom(parent, LVL_PIC_GAME_BGD) {
    LVL_InterBtn *pstBack = new LVL_InterBtn(this, QStringLiteral("����"), LVL_SYSTEM_BACK_BTN_GEO);
    pstBack->LVL_InterSetBtnPic(LVL_PIC_MENU_BTN, LVL_PIC_MENU_BTN + LVL_PIC_FOCUSED, LVL_PIC_MENU_BTN + LVL_PIC_CLICKED);
    connect(pstBack, &LVL_InterBtn::clicked, [this] { emit sgnChangeRoom("StartRoom"); });
}

void LVL_SystemGameRoom::LVL_SystemInitGame() {
    pstGame = new LVL_GameCtrller(this);
    pstGame->lower();
    pstGame->show();
}

///////////////////////////////////////////////////////////////

/*
    ���ܣ����÷��乹�캯��
    ���ݣ�����ѡ����汳�������������ť
    ������parent-�����ڵ�ָ��
*/
LVL_SystemOptionRoom::LVL_SystemOptionRoom(QWidget *parent) : LVL_SystemAbstractRoom(parent, LVL_PIC_DEFAULT_BGD) {

    QLabel *pqlHint = new QLabel(this);
    pqlHint->setGeometry(LVL_SYSTEM_PLAYER_NUM_HINT_GEO);
    QPalette qpalText = pqlHint->palette();
    qpalText.setColor(QPalette::WindowText, LVL_GAME_DECK_TEXT_COLOR);
    pqlHint->setPalette(qpalText);
    pqlHint->setFont(QFont(LVL_GAME_DECK_TEXT_FONT, LVL_GAME_DECK_TEXT_SIZE));
    pqlHint->setAlignment(Qt::AlignRight);
    pqlHint->setText(QStringLiteral("���������"));
    
    pstPlayerNumSpinBox = new QSpinBox(this);
    pstPlayerNumSpinBox->setGeometry(LVL_SYSTEM_PLAYER_NUM_SPINBOX_GEO);
    pstPlayerNumSpinBox->setRange(LVL_GAME_MINIMUM_PLAYER_NUM, LVL_GAME_MAXIMUM_PLAYER_NUM);
    pstPlayerNumSpinBox->setSingleStep(1);
    pstPlayerNumSpinBox->setValue(4);

    LVL_InterBtn *pstBack = new LVL_InterBtn(this, QStringLiteral("����"), LVL_SYSTEM_BACK_BTN_GEO);
    pstBack->LVL_InterSetBtnPic(LVL_PIC_MENU_BTN, LVL_PIC_MENU_BTN + LVL_PIC_FOCUSED, LVL_PIC_MENU_BTN + LVL_PIC_CLICKED);
    connect(pstBack, &LVL_InterBtn::clicked, [this] { emit sgnChangeRoom("StartRoom"); });
}

void LVL_SystemOptionRoom::hideEvent(QHideEvent *event) {
    LVL_GameSetting &stSettings = LVL_GameSetting::LVL_SystemGetSettings();
    stSettings.LVL_GameWriteSetting<int>("Player Number", pstPlayerNumSpinBox->value());
}