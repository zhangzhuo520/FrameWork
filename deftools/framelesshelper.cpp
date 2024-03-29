#include "framelesshelper.h"

namespace tools {
int CursorPosCalculator::m_nBorderWidth = 5;
int CursorPosCalculator::m_nTitleHeight = 30;
/***** CursorPosCalculator *****/
CursorPosCalculator::CursorPosCalculator()
{
    reset();
}

void CursorPosCalculator::reset()
{
    m_bOnEdges = false;
    m_bOnLeftEdge = false;
    m_bOnRightEdge = false;
    m_bOnTopEdge = false;
    m_bOnBottomEdge = false;
    m_bOnTopLeftEdge = false;
    m_bOnBottomLeftEdge = false;
    m_bOnTopRightEdge  = false;
    m_bOnBottomRightEdge = false;
}

void CursorPosCalculator::recalculate(const QPoint &gMousePos, const QRect &frameRect)
{
    int globalMouseX = gMousePos.x();
    int globalMouseY = gMousePos.y();

    int frameX = frameRect.x();
    int frameY = frameRect.y();

    int frameWidth = frameRect.width();
    int frameHeight = frameRect.height();

    m_bOnLeftEdge = (globalMouseX >= frameX &&
                  globalMouseX <= frameX + m_nBorderWidth );


    m_bOnRightEdge = (globalMouseX >= frameX + frameWidth - m_nBorderWidth &&
                   globalMouseX <= frameX + frameWidth);

    m_bOnTopEdge = (globalMouseY >= frameY &&
                 globalMouseY <= frameY + m_nBorderWidth );

    m_bOnBottomEdge = (globalMouseY >= frameY + frameHeight - m_nBorderWidth &&
                    globalMouseY <= frameY + frameHeight);

    m_bOnTopLeftEdge = m_bOnTopEdge && m_bOnLeftEdge;
    m_bOnBottomLeftEdge = m_bOnBottomEdge && m_bOnLeftEdge;
    m_bOnTopRightEdge = m_bOnTopEdge && m_bOnRightEdge;
    m_bOnBottomRightEdge = m_bOnBottomEdge && m_bOnRightEdge;

    m_bOnEdges = m_bOnLeftEdge || m_bOnRightEdge || m_bOnTopEdge || m_bOnBottomEdge;
}

/***** WidgetData *****/
WidgetData::WidgetData(FramelessHelperPrivate *_d, QWidget *pTopLevelWidget)
{
    d = _d;
    m_pWidget = pTopLevelWidget;
    m_bLeftButtonPressed = false;
    m_bCursorShapeChanged = false;
    m_bLeftButtonTitlePressed = false;

    m_windowFlags = m_pWidget->windowFlags();
    m_pWidget->setMouseTracking(true);
    m_pWidget->setAttribute(Qt::WA_Hover, true);

    updateRubberBandStatus();
}

WidgetData::~WidgetData()
{
    m_pWidget->setMouseTracking(false);
    m_pWidget->setWindowFlags(m_windowFlags);
    m_pWidget->setAttribute(Qt::WA_Hover, false);
}

QWidget* WidgetData::widget()
{
    return m_pWidget;
}

void WidgetData::handleWidgetEvent(QEvent *event)
{
    switch (event->type())
    {
    default:
        break;
    case QEvent::MouseButtonPress:
        handleMousePressEvent(static_cast<QMouseEvent*>(event));
        break;
    case QEvent::MouseButtonRelease:
        handleMouseReleaseEvent(static_cast<QMouseEvent*>(event));
        break;
    case QEvent::MouseMove:
        handleMouseMoveEvent(static_cast<QMouseEvent*>(event));
        break;
    case QEvent::Leave:
        handleLeaveEvent(static_cast<QMouseEvent*>(event));
        break;
    case QEvent::HoverMove:
        handleHoverMoveEvent(static_cast<QHoverEvent*>(event));
        break;
    }
}

void WidgetData::updateRubberBandStatus()
{

}

void WidgetData::updateCursorShape(const QPoint &gMousePos)
{
    if (m_pWidget->isFullScreen() || m_pWidget->isMaximized())
    {
        if (m_bCursorShapeChanged)
        {
            m_pWidget->unsetCursor();
        }
        return;
    }

    m_moveMousePos.recalculate(gMousePos, m_pWidget->frameGeometry());

    if(m_moveMousePos.m_bOnTopLeftEdge || m_moveMousePos.m_bOnBottomRightEdge)
    {
        m_pWidget->setCursor( Qt::SizeFDiagCursor );
        m_bCursorShapeChanged = true;
    }
    else if(m_moveMousePos.m_bOnTopRightEdge || m_moveMousePos.m_bOnBottomLeftEdge)
    {
        m_pWidget->setCursor( Qt::SizeBDiagCursor );
        m_bCursorShapeChanged = true;
    }
    else if(m_moveMousePos.m_bOnLeftEdge || m_moveMousePos.m_bOnRightEdge)
    {
        m_pWidget->setCursor( Qt::SizeHorCursor );
        m_bCursorShapeChanged = true;
    }
    else if(m_moveMousePos.m_bOnTopEdge || m_moveMousePos.m_bOnBottomEdge)
    {
        m_pWidget->setCursor( Qt::SizeVerCursor );
        m_bCursorShapeChanged = true;
    }
    else
    {
        if (m_bCursorShapeChanged)
        {
            m_pWidget->unsetCursor();
            m_bCursorShapeChanged = false;
        }
    }
}

void WidgetData::resizeWidget(const QPoint &gMousePos)
{
    QRect origRect;

    if (d->m_bRubberBandOnResize)
        origRect = m_pWidget->frameGeometry();

    int left = origRect.left();
    int top = origRect.top();
    int right = origRect.right();
    int bottom = origRect.bottom();
    origRect.getCoords(&left, &top, &right, &bottom);

    int minWidth = 40;
    int minHeight = 40;

    if (m_pressedMousePos.m_bOnTopLeftEdge)
    {
        left = gMousePos.x();
        top = gMousePos.y();
    }
    else if (m_pressedMousePos.m_bOnBottomLeftEdge)
    {
        left = gMousePos.x();
        bottom = gMousePos.y();
    }
    else if (m_pressedMousePos.m_bOnTopRightEdge)
    {
        right = gMousePos.x();
        top = gMousePos.y();
    }
    else if (m_pressedMousePos.m_bOnBottomRightEdge)
    {
        right = gMousePos.x();
        bottom = gMousePos.y();
    }
    else if (m_pressedMousePos.m_bOnLeftEdge)
    {
        left = gMousePos.x();
    }
    else if (m_pressedMousePos.m_bOnRightEdge)
    {
        right = gMousePos.x();
    }
    else if (m_pressedMousePos.m_bOnTopEdge)
    {
        top = gMousePos.y();
    }
    else if (m_pressedMousePos.m_bOnBottomEdge)
    {
        bottom = gMousePos.y();
    }

    QRect newRect(QPoint(left, top), QPoint(right, bottom));

    if (newRect.isValid())
    {
        if (minWidth > newRect.width())
        {
            if (left != origRect.left())
                newRect.setLeft(origRect.left());
            else
                newRect.setRight(origRect.right());
        }
        if (minHeight > newRect.height())
        {
            if (top != origRect.top())
                newRect.setTop(origRect.top());
            else
                newRect.setBottom(origRect.bottom());
        }

        if (d->m_bRubberBandOnResize)
        {
            m_pWidget->setGeometry(newRect);
        }
    }
}

void WidgetData::moveWidget(const QPoint& gMousePos)
{
    if (d->m_bRubberBandOnMove)
    {
        m_pWidget->move(gMousePos - m_ptDragPos);
    }
}

void WidgetData::handleMousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_bLeftButtonPressed = true;
        m_bLeftButtonTitlePressed = event->pos().y() < m_moveMousePos.m_nTitleHeight;

        QRect frameRect = m_pWidget->frameGeometry();
        m_pressedMousePos.recalculate(event->globalPos(), frameRect);
        m_ptDragPos = event->globalPos() - frameRect.topLeft();
    }
}

void WidgetData::handleMouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_bLeftButtonPressed = false;
        m_bLeftButtonTitlePressed = false;
        m_pressedMousePos.reset();
#ifdef WINDOWS_RAIDS
            QBitmap bmp(m_pWidget->size());
            bmp.fill();
            QPainter p(&bmp);
            p.setBrush(Qt::black);
            p.drawRoundedRect(bmp.rect(), 10, 10);
            m_pWidget->setMask(bmp);
            m_pWidget->setStyleSheet("background: rgb(230, 230, 230)");
#endif
    }
}

void WidgetData::handleMouseMoveEvent(QMouseEvent *event)
{
    if (m_bLeftButtonPressed)
    {
        if (widget()->isMaximized())
        {
            return;
        }

        if (d->m_bWidgetResizable && m_pressedMousePos.m_bOnEdges)
        {
            resizeWidget(event->globalPos());
        }
        else if (d->m_bWidgetMovable && m_bLeftButtonTitlePressed)
        {
            moveWidget(event->globalPos());
        }
    }
    else if (d->m_bWidgetResizable)
    {
        updateCursorShape(event->globalPos());
    }
}

void WidgetData::handleLeaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    if (!m_bLeftButtonPressed)
    {
        m_pWidget->unsetCursor();
    }
}

void WidgetData::handleHoverMoveEvent(QHoverEvent *event)
{
    if (d->m_bWidgetResizable)
    {
        updateCursorShape(m_pWidget->mapToGlobal(event->pos()));
    }
}

/*****FramelessHelper*****/
FramelessHelper::FramelessHelper(QObject *parent)
    : QObject(parent),
      d(new FramelessHelperPrivate())
{
    d->m_bWidgetMovable = true;
    d->m_bWidgetResizable = true;
    d->m_bRubberBandOnResize = false;
    d->m_bRubberBandOnMove = false;
}

FramelessHelper::~FramelessHelper()
{
    QList<QWidget*> keys = d->m_widgetDataHash.keys();
    int size = keys.size();
    for (int i = 0; i < size; ++i) {
        delete d->m_widgetDataHash.take(keys[i]);
    }
    delete d;
}

bool FramelessHelper::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type())
    {
    case QEvent::MouseMove:
    case QEvent::HoverMove:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::Leave:
    {
        WidgetData *data = d->m_widgetDataHash.value(static_cast<QWidget*>(obj));
        if (data)
        {
            data->handleWidgetEvent(event);
            return true;
        }
    }
    case QEvent::MouseButtonDblClick:
    {
        QMouseEvent *e = static_cast <QMouseEvent *>(event);
        if((uint)e->pos().y() <= titleHeight())
        {
            emit signal_double_click();
        }
    }
    default:
        break;
    }
    return QObject::eventFilter(obj, event);
}

void FramelessHelper::activateOn(QWidget *topLevelWidget)
{
    if (!d->m_widgetDataHash.contains(topLevelWidget))
    {
        WidgetData *data = new WidgetData(d, topLevelWidget);
        d->m_widgetDataHash.insert(topLevelWidget, data);

        topLevelWidget->installEventFilter(this);
    }
}

void FramelessHelper::removeFrom(QWidget *topLevelWidget)
{
    WidgetData *data = d->m_widgetDataHash.take(topLevelWidget);
    if (data)
    {
        topLevelWidget->removeEventFilter(this);
        delete data;
    }
}

void FramelessHelper::setRubberBandOnMove(bool movable)
{
    d->m_bRubberBandOnMove = movable;
    QList<WidgetData*> list = d->m_widgetDataHash.values();
    foreach (WidgetData *data, list)
    {
        data->updateRubberBandStatus();
    }
}

void FramelessHelper::setWidgetMovable(bool movable)
{
    d->m_bWidgetMovable = movable;
}

void FramelessHelper::setWidgetResizable(bool resizable)
{
    d->m_bWidgetResizable = resizable;
}

void FramelessHelper::setRubberBandOnResize(bool resizable)
{
    d->m_bRubberBandOnResize = resizable;
    QList<WidgetData*> list = d->m_widgetDataHash.values();
    foreach (WidgetData *data, list)
    {
        data->updateRubberBandStatus();
    }
}

void FramelessHelper::setBorderWidth(uint width)
{
    if (width > 0)
    {
        CursorPosCalculator::m_nBorderWidth = width;
    }
}

void FramelessHelper::setTitleHeight(uint height)
{
    if (height > 0)
    {
        CursorPosCalculator::m_nTitleHeight = height;
    }
}

bool FramelessHelper::widgetMovable()
{
    return d->m_bWidgetMovable;
}

bool FramelessHelper::widgetResizable()
{
    return d->m_bWidgetResizable;
}

bool FramelessHelper::rubberBandOnMove()
{
    return d->m_bRubberBandOnMove;
}

bool FramelessHelper::rubberBandOnResisze()
{
    return d->m_bRubberBandOnResize;
}

uint FramelessHelper::borderWidth()
{
    return CursorPosCalculator::m_nBorderWidth;
}

uint FramelessHelper::titleHeight()
{
    return CursorPosCalculator::m_nTitleHeight;
}
}
