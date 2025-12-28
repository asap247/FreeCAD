// SPDX-License-Identifier: LGPL-2.1-or-later
#pragma once

#include <Gui/UserNavigationStyle.h>

namespace Gui
{

class TrackpadNavigationStyle : public UserNavigationStyle
{
    TYPESYSTEM_HEADER();

public:
    static void init();

    TrackpadNavigationStyle();
    ~TrackpadNavigationStyle() override;

    const char* getName() const override;

protected:
    SbBool processSoEvent(const SoEvent* ev) override;

private:
    SbVec2f lastPos;
};

} // namespace Gui
