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

#ifndef WebViewEventHandler_h
#define WebViewEventHandler_h

#include <cmath>
#include <deque>
#include "PlatformInputEvents.h"
#include <QDateTime>
#include <QObject>
#include <QPoint>
#include <QtGui/QPaintEvent>
#include <QtGui/QWidget>
#include <QTimer>
#include "qtstagewebview.h"
#include "TouchEventHandler.h"

class QKeyEvent;
class QMouseEvent;
class QWheelEvent;

namespace BlackBerry {
namespace Starbuck {

////////////////////////////////////////////////////////////////////////////
// temp class.
class TouchEventDispatcher : public QObject
{
    Q_OBJECT
public:
    TouchEventDispatcher();
    ~TouchEventDispatcher();
    void start(int);
    void cancel();
    bool isPending();
    void setTouchEvent(TouchEvent te);

signals:
    void sigTimeout(TouchEvent te);

public slots:
    void fire();

private:
    TouchEvent m_te;
    QTimer m_timer;
};

/////////////////////////////////////////////////////////////////////////////
// this is the interface class of webview event handler.
class EventHandler : public QObject
{
    Q_OBJECT
public:
    EventHandler(QtStageWebView* view);
    virtual ~EventHandler();

    virtual void terminateCurrentWork() = 0;

    virtual void mouseDoubleClickEvent(QMouseEvent* e) = 0;
    virtual void mousePressEvent(QMouseEvent* e) = 0;
    virtual void mouseReleaseEvent(QMouseEvent* e) = 0;
    virtual void mouseMoveEvent(QMouseEvent* e) = 0;
    virtual void wheelEvent(QWheelEvent* e) = 0;
    virtual void keyPressEvent(QKeyEvent* e) = 0;
    virtual void extraPaintJob() = 0;

protected slots:
    bool handleTouchEvent(TouchEvent te);
protected:
    QtStageWebView* m_webView;
    TouchEventHandler* m_touchEventHandler;
};

/////////////////////////////////////////////////////////////////////////////
// NormalEventHandler to handle  event in normal browse mode.
class NormalEventHandler : public EventHandler
{
public:
    NormalEventHandler(QtStageWebView* view);
    virtual ~NormalEventHandler();

    virtual void terminateCurrentWork() {}

    virtual void mouseDoubleClickEvent(QMouseEvent* e);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent* e);
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void wheelEvent(QWheelEvent* e);
    virtual void keyPressEvent(QKeyEvent* e);
    virtual void extraPaintJob();

private:
    QPoint m_lastTouchPoint;
    TouchEvent::Type m_touchEventType;
    QTime m_lastMouseTime;
    QPoint m_lastMousePos;
    std::deque< std::pair<QPoint, QTime> > m_mouseTrack;
    TouchEventDispatcher m_touchReleaseDispatcher;
};

} // namespace starbuck
} // namespace BlackBerry

#endif // WebViewEventHandler_h
