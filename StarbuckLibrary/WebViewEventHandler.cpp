/*
* Copyright 2010-2011 Research In Motion Limited.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "WebViewEventHandler.h"
#include <QApplication>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QtGui/QPainter>
#include <QWheelEvent>

namespace BlackBerry {
namespace Starbuck {
// Unit: ms
const static int MeasureInterval = 500;
const static int AnimationInteval = 50;
const static int MaxTrackPointSize = 100;
const static int MouseMoveThreshold = 5;
const static int MuiltiTouchRoamInteval = 50;
    }
}

using namespace BlackBerry::Starbuck;

EventHandler::EventHandler(QtStageWebView* view) : QObject(view), m_webView(view), m_touchEventHandler(new TouchEventHandler(view))
{
    Q_ASSERT(m_webView);
}

EventHandler::~EventHandler()
{
}

bool EventHandler::handleTouchEvent(TouchEvent te)
{
  return m_touchEventHandler->handleTouchEvent(te);
}


/////////////////////////////////////////////////////////////////////////////////////
/// normal browse event handler's implementations.
NormalEventHandler::NormalEventHandler(QtStageWebView* view) : EventHandler(view)
{
    connect(&m_touchReleaseDispatcher, SIGNAL(sigTimeout(TouchEvent)), this, SLOT(handleTouchEvent(TouchEvent)));
}

NormalEventHandler::~NormalEventHandler()
{
    m_touchReleaseDispatcher.cancel();
    m_touchReleaseDispatcher.disconnect(SIGNAL(sigTimeout(TouchEvent)));
}

void NormalEventHandler::mouseDoubleClickEvent(QMouseEvent* e)
{
    if (!m_webView->page())
        return;
    //m_webView->page()->blockZoom(e->x(), e->y());
    m_touchEventType = TouchEvent::TouchCancel;
}

void NormalEventHandler::mousePressEvent(QMouseEvent* e)
{
    if (!m_webView->page())
        return;

    m_lastTouchPoint = e->pos();
    m_webView->m_scrollTimer.stop();
    m_lastMousePos = e->pos();
    m_mouseTrack.clear();

    TouchPoint tp;
    tp.m_state = TouchPoint::TouchPressed;
    tp.m_screenPos = QPoint(e->x(), e->y());
    tp.m_pos = tp.m_screenPos;

    TouchEvent te;
    te.m_type = TouchEvent::TouchStart;
    m_touchEventType = TouchEvent::TouchStart;
    te.m_singleType = TouchEvent::SinglePressed;
    te.m_points.push_back(tp);
    handleTouchEvent(te);
}

void NormalEventHandler::mouseReleaseEvent(QMouseEvent* e)
{
    if (!m_webView->page())
        return;

    // Handle inertial scrolling
    if (m_mouseTrack.size() > 1) {
        QTime now = m_mouseTrack[m_mouseTrack.size() - 1].second;
        int i;
        for (i = m_mouseTrack.size() - 1; i > 0; i--)
            if (m_mouseTrack[i].second.msecsTo(now) > MeasureInterval)
                break;

        // m_trackTimes[i] is first one which larger than MeasureInterval ms to now, or the last on in the vector.
        int elapsed = m_mouseTrack[i].second.msecsTo(now);
        if (elapsed) {
            QPoint delta = m_mouseTrack[i].first - e->pos();
            m_webView->m_nextScrollStepLength = QPoint(delta.x() * AnimationInteval / elapsed, delta.y() * AnimationInteval / elapsed);
            m_webView->m_scrollTimer.start(AnimationInteval);
        }
    }

    // Handle normal mouse release event:
    // Send touch event:
    m_lastTouchPoint = e->pos();
    TouchPoint tp;
    tp.m_state = TouchPoint::TouchReleased;
    tp.m_screenPos = QPoint(e->x(), e->y());
    tp.m_pos = tp.m_screenPos;

    TouchEvent te;
    if (m_touchEventType == TouchEvent::TouchStart)
        m_touchEventType = TouchEvent::TouchEnd;
    te.m_type = m_touchEventType;
    te.m_singleType = TouchEvent::SingleReleased;
    te.m_points.push_back(tp);
    m_touchReleaseDispatcher.setTouchEvent(te);
    m_touchReleaseDispatcher.start(QApplication::doubleClickInterval());
}

void NormalEventHandler::mouseMoveEvent(QMouseEvent* e)
{
    if (!m_webView->page())
        return;

    static const int treatAsZeroDistance = 4;
    if (e->buttons() == Qt::LeftButton) {
        QPoint newPoint = m_webView->validateScrollPoint(m_webView->m_lastContentPos + m_lastMousePos - e->pos());
        m_webView->m_lastContentPos = newPoint;
        m_lastMousePos = e->pos();
        if (m_mouseTrack.size() > MaxTrackPointSize) {
            m_mouseTrack.pop_front();
        }
        m_mouseTrack.push_back(std::make_pair(e->pos(), QTime::currentTime()));

        QPoint delta = e->pos() - m_lastTouchPoint;
        if (delta.manhattanLength() <= treatAsZeroDistance)
            return;

        m_lastTouchPoint = e->pos();
        m_webView->scrollBy(delta.x(), delta.y());

        TouchPoint tp;
        tp.m_state = TouchPoint::TouchMoved;
        tp.m_screenPos = QPoint(e->x(), e->y());
        tp.m_pos = tp.m_screenPos;

        TouchEvent te;
        te.m_type = TouchEvent::TouchMove;
        te.m_singleType = TouchEvent::SingleMoved;
        te.m_points.push_back(tp);       
        handleTouchEvent(te);
        m_touchEventType = TouchEvent::TouchCancel;
    }
}

void NormalEventHandler::wheelEvent(QWheelEvent* e)
{
}

void NormalEventHandler::keyPressEvent(QKeyEvent* e)
{
}

void NormalEventHandler::extraPaintJob()
{
}

////////////////////////////////////////////////////////////////////////////////////
/// temp class implementation.
TouchEventDispatcher::TouchEventDispatcher()
{
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(fire()));
}

TouchEventDispatcher::~TouchEventDispatcher()
{
    m_timer.stop();
}

void TouchEventDispatcher::setTouchEvent(TouchEvent event)
{
    m_te = event;
}

void TouchEventDispatcher::start(int timeout)
{
    if (timeout < 0)
        return;
    m_timer.start(timeout);
}

void TouchEventDispatcher::cancel()
{
    if (isPending())
        m_timer.stop();
}

void TouchEventDispatcher::fire()
{
    m_timer.stop();
    emit sigTimeout(m_te);
}

bool TouchEventDispatcher::isPending()
{
    return m_timer.isActive();
}

