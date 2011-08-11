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

#include "TouchEventHandler.h"
#include <QEvent>
#include <QMouseEvent>
#include <cmath>

const float touchDownTimeout = 150.0; // By trial and error, msec
const unsigned fatFingerRad = 20; // Half of the width of the fat finger rect.
const unsigned halfFatFingerDiagonal = ceilf(fatFingerRad * sqrt(2.0));

namespace BlackBerry {
namespace Starbuck {

TouchEventHandler::TouchEventHandler(QtStageWebView* webview)
    : QObject(webview), m_webView(webview)
    , m_didScrollPage(0)
    , m_didMoveFinger(0)
    , m_didZoomPage(0)
    , m_didCancelTouch(0)
    , m_subframeNode(0)
{
    m_touchDownTimer.setSingleShot(true);
    connect(&m_touchDownTimer, SIGNAL(timeout()), this, SLOT(touchPressedTimeout()));
}

TouchEventHandler::~TouchEventHandler()
{
}

bool TouchEventHandler::handleTouchEvent(TouchEvent& ev)
{
    if (ev.m_type == TouchEvent::TouchStart)
        m_didCancelTouch = false;

    // Clear stuff if canceled and consume event.
    if (ev.m_type == TouchEvent::TouchCancel) {
        m_webView->clearFocusNode();
        m_didCancelTouch = true;
        m_subframeNode = 0;
        return true;
    }

    // Do not handle touch event if multi-touch
    if (ev.m_singleType == TouchEvent::SingleNone)
        return false;

    if (ev.m_singleType == TouchEvent::SinglePressed) {
        setDidScrollPage(false);
        setDidMoveFinger(false);
        setDidZoomPage(false);
        m_didCancelTouch = false;
        m_touchDownPoint = ev.m_points[0].m_screenPos;
        m_lastTouchPoint = ev.m_points[0].m_screenPos;
        m_fatFingerPoint = ev.m_points[0].m_pos;
        m_subframeNode = 0;
        m_touchDownTimer.start(touchDownTimeout);

    } else if (ev.m_singleType == TouchEvent::SingleReleased) {
        setDidScrollPage(true);
        m_subframeNode = 0;
        // Update the mouse position with a MouseMoved event
        QMouseEvent mouseEvent(QEvent::MouseMove, m_lastTouchPoint, m_fatFingerPoint, Qt::LeftButton,Qt::LeftButton, Qt::NoModifier);
        m_webView->handleMouseEvent(mouseEvent);
        // Send a Mouse press event if not already pressed. This can occur if we receive events before the touchPressed timeout occurs.
        if (m_webView->page()->mousePressed() ) {
            QMouseEvent mousePressedEvent(QEvent::MouseButtonPress, m_lastTouchPoint, m_fatFingerPoint, Qt::LeftButton, Qt::LeftButton,Qt::NoModifier);
            m_webView->handleMouseEvent(mousePressedEvent);
        }
        // Finally send a MouseReleased event.//m_webview->page()->handleMouseEvent(mouseEvent);
        QMouseEvent mouseReleaseEvent(QEvent::MouseButtonRelease, m_lastTouchPoint, m_fatFingerPoint, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        m_webView->handleMouseEvent(mouseReleaseEvent);

    } else if (ev.m_singleType == TouchEvent::SingleMoved) {
        // If there is a subframe or div to scroll try scrolling it otherwise returns false and let chrome scroll the page.
        if (!m_subframeNode)
            m_subframeNode = m_webView->page()->mainFrame()->hitTestContent( ev.m_points[0].m_pos).frame();
        QPoint delta = m_lastTouchPoint - ev.m_points[0].m_screenPos;
        m_lastTouchPoint = ev.m_points[0].m_screenPos;
        return m_webView->scrollNodeRecursively(m_subframeNode, QSize(delta.x(), delta.y()));
  }
  return true;
}

void TouchEventHandler::touchPressedTimeout()
{
    m_touchDownTimer.stop();
    if (!m_didZoomPage && !m_didScrollPage && !m_didCancelTouch) {

        QWebFrame* node = m_webView->page()->mainFrame()->hitTestContent(m_fatFingerPoint).frame();
        if (!(node && node->hasFocus()))
            m_fatFingerPoint = getFatFingerPos(m_fatFingerPoint);
        // First update the mouse position with a MouseMoved event
        QMouseEvent mouseEvent(QEvent::MouseMove, m_lastTouchPoint, m_fatFingerPoint, Qt::LeftButton,Qt::LeftButton, Qt::NoModifier);
        m_webView->handleMouseEvent(mouseEvent);
        // Then send the MousePressed event
        QMouseEvent mousePressEvent(QEvent::MouseButtonPress, m_lastTouchPoint, m_fatFingerPoint, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        m_webView->handleMouseEvent(mousePressEvent);
    }
}

QPoint TouchEventHandler::getFatFingerPos(QPoint& pos)
{
    return m_webView->getFatFingerPos(pos);
}
}//end Starbuck
}//end BlackBerry
