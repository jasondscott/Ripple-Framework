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

#ifndef PlatformInputEvents_h
#define PlatformInputEvents_h

#include <vector>
#include <QPoint>

namespace BlackBerry {
namespace Starbuck {

// NOTE:  Conversion between values in HTMLInputElement and these values is in WebPage.cpp and any changes made
// here will require a corresponding update.

enum OlympiaInputType {
    InputTypeText = 0,
    InputTypePassword,
    InputTypeIsIndex,
    InputTypeSearch,
    InputTypeEmail,
    InputTypeNumber,
    InputTypeTelephone,
    InputTypeURL,
    InputTypeColor,
    InputTypeTextArea
};

// Scroll directions, this maps to WebCore::ScrollDirection defined in ScrollTypes.h.
// Currently used in link-to-link navigation.

enum ScrollDirection { ScrollUp, ScrollDown, ScrollLeft, ScrollRight };

class TouchPoint {
public:
    int m_id;
    enum State { TouchReleased, TouchMoved, TouchPressed, TouchStationary };
    State m_state;
    QPoint m_screenPos;
    QPoint m_pos;
};

class TouchEvent {
public:
    enum Type { TouchStart, TouchMove, TouchEnd, TouchCancel };
    enum SingleType { SingleReleased, SingleMoved, SinglePressed, SingleNone };
    Type m_type;
    SingleType m_singleType;
    bool m_altKey;
    bool m_shiftKey;
    std::vector<TouchPoint> m_points;
};

} // namespace Starbuck
} // namespace BlackBerry

#endif // PlatformInputEvents_h
