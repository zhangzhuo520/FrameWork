#include "frame.h"
#include <QLabel>
#include <QPushButton>
#include <QBoxLayout>
#include "deftools/apphelper.h"
#include "deftools/framelesshelper.h"
#include "mainwidget.h"

Frame::Frame(QWidget *parent) :
    QDialog(parent)
{
    initUi();
    initSysIcon();
    initStyle();
}

Frame::~Frame()
{
}

void Frame::initUi()
{
    setObjectName("main_frame");
    m_title_widget = new QWidget(this);
    m_title_widget->setObjectName("widget_title");
    m_title_widget->setFixedHeight(32);
    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->setSpacing(0);
    hlayout->setMargin(0);
    m_icon_label = new QLabel(this);
    m_icon_label->setObjectName("lab_icon");
    m_icon_label->setFixedSize(32, 32);
    m_title_label = new QLabel(this);
    m_title_label->setText("Title");
    m_title_label->setObjectName("lab_title");
    m_max_button = new QPushButton(this);
    m_max_button->setObjectName("btn_max");
    m_max_button->setFixedSize(32, 32);
    m_min_button = new QPushButton(this);
    m_min_button->setObjectName("btn_min");
    m_min_button->setFixedSize(32, 32);
    m_close_button = new QPushButton(this);
    m_close_button->setObjectName("btn_close");
    m_close_button->setFixedSize(32, 32);
    hlayout->addWidget(m_icon_label);
    hlayout->addWidget(m_title_label);
    hlayout->addWidget(m_min_button);
    hlayout->addWidget(m_max_button);
    hlayout->addWidget(m_close_button);
    m_title_widget->setLayout(hlayout);

    m_main_widget = new MainWidget(this);
    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setSpacing(0);
    vlayout->setMargin(0);
    vlayout->addWidget(m_title_widget);
    vlayout->addWidget(m_main_widget);
    setLayout(vlayout);
    resize(800, 600);
}

void Frame::initStyle()
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
    this->setSizeGripEnabled(true);
#ifndef AROUND_STRETCH_FLAG
    this->setProperty("CanMove", true);
#else
    m_framelessHelper = new tools::FramelessHelper(this);
    m_framelessHelper->activateOn(this);
    m_framelessHelper->setTitleHeight(32);
    m_framelessHelper->setWidgetMovable(true);
    m_framelessHelper->setWidgetResizable(true);
    m_framelessHelper->setRubberBandOnMove(true);
    m_framelessHelper->setRubberBandOnResize(true);
    connect(m_framelessHelper, SIGNAL(signal_double_click()), this, SLOT(slot_max_clicked()));
#endif

    this->setWindowTitle(m_title_label->text());

    m_title_label->installEventFilter(this);
    tools::IconHelper::getInstance()->setIcon(m_close_button, QChar(0xf00d));
    tools::IconHelper::getInstance()->setIcon(m_max_button, QChar(0xf096));
    tools::IconHelper::getInstance()->setIcon(m_min_button, QChar(0xf068));

    QIcon title_icon(":/pic/picture/serial.png");
    QPixmap pixmap = title_icon.pixmap(QSize(16, 16));
    m_icon_label->setPixmap(pixmap);
   setWindowIcon(title_icon);

    tools::Helper::moveFormToCenter(this);

    connect(m_close_button, SIGNAL(clicked()), this, SLOT(close()));
    connect(m_min_button, SIGNAL(clicked()), this, SLOT(showMinimized()));
    connect(m_max_button, SIGNAL(clicked()), this, SLOT(slot_max_clicked()));
    connect(m_min_button, SIGNAL(clicked()), m_systrayIcon, SLOT(show()));
    connect(m_systrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(slot_systrayIcon_active(QSystemTrayIcon::ActivationReason)));
}

void Frame::initSysIcon()
{
    m_systrayIcon = new QSystemTrayIcon(this);
    QIcon icon = QIcon(":/pic/picture/serial.png");
    m_systrayIcon->setIcon(icon);
    m_systrayIcon->setToolTip("Frame");
}

bool Frame::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        this->slot_max_clicked();
        return true;
    }
    return QObject::eventFilter(obj, event);
}

void Frame::slot_max_clicked()
{
    if (!isMaximized()) {
        showMaximized();
        tools::IconHelper::getInstance()->setIcon(m_max_button, QChar(0xf096));
        m_max_button->setToolTip("最大化");
#ifndef AROUND_STRETCH_FLAG
        this->setProperty("CanMove", false);
#endif
    } else {
        showNormal();
        tools::IconHelper::getInstance()->setIcon(m_max_button, QChar(0xf079));
        m_max_button->setToolTip("还原");
#ifndef AROUND_STRETCH_FLAG
        this->setProperty("CanMove", true);
#endif
    }
}

void Frame::slot_systrayIcon_active(QSystemTrayIcon::ActivationReason reson)
{
    if(reson == QSystemTrayIcon::Trigger)
        this->showNormal();
}


