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

#ifndef TouchEventHandler_h
#define TouchEventHandler_h

#include <QObject>
#include "PlatformInputEvents.h"
#include <QPoint>
#include <QRect>
#include <QPointer>
#include <QTimer>
#include "QtStageWebView.h"

namespace BlackBerry {
namespace Starbuck {

  class TouchEventHandler : public QObject
{
  Q_OBJECT

public:
    TouchEventHandler(QtStageWebView* webview);
    ~TouchEventHandler();

    bool handleTouchEvent(TouchEvent& event);
    
    void setDidScrollPage(bool scrolled) { m_didScrollPage = scrolled; }
    void setDidZoomPage(bool zoomed) { m_didZoomPage = zoomed; }
    void setDidMoveFinger(bool moved) { m_didMoveFinger = moved; }
public slots:
    void touchPressedTimeout();

private:
    QPoint getFatFingerPos(QPoint& winPos);

    QtStageWebView* m_webView;

    bool m_didScrollPage;
    bool m_didMoveFinger;
    bool m_didZoomPage;
    bool m_didCancelTouch;

    QPoint m_touchDownPoint; // Screen Position
    QPoint m_fatFingerPoint; // Content Position
    QPoint m_lastTouchPoint; // Screen Position

    QTimer m_touchDownTimer;
    QPointer<QWebFrame> m_subframeNode;
};
}
}
#endif // TouchEventHandler_h
