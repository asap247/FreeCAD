#include "TrackpadNavigationStyle.h"

#include <Inventor/events/SoEvent.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/events/SoMouseWheelEvent.h>

#include <Gui/View3DInventorViewer.h>
#include <Gui/NavigationStyle.h>

using namespace Gui;

/* ------------------------------------------------------------
 * Static registration
 * ------------------------------------------------------------ */
static NavigationStyle::Type
    TrackpadNavigationStyleType(
        "Trackpad",
        []() { return new TrackpadNavigationStyle; }
    );

TrackpadNavigationStyle::TrackpadNavigationStyle()
{
}

const char* TrackpadNavigationStyle::getName() const
{
    return "Trackpad";
}

bool TrackpadNavigationStyle::processSoEvent(const SoEvent* ev)
{
    auto* viewer = getViewer();
    if (!viewer)
        return false;

    // One-finger drag: cursor move only
    if (const SoLocation2Event* loc =
            dynamic_cast<const SoLocation2Event*>(ev)) {

        lastPos = loc->getPosition();
        hasLastPos = true;
        return true;
    }

    // Two-finger gestures
    if (const SoMouseWheelEvent* wheel =
            dynamic_cast<const SoMouseWheelEvent*>(ev)) {

        SbVec2f delta = wheel->getDelta();
        delta *= 0.5f;

        const bool shift =
            wheel->getModifiers() & SoEvent::SHIFT_DOWN;
        const bool ctrl =
            wheel->getModifiers() & SoEvent::CTRL_DOWN;

        // Pinch zoom
        if (ctrl) {
            viewer->zoomCamera(delta[1] * 0.01f);
            return true;
        }

        // Rotate
        if (shift) {
            viewer->rotateCamera(delta[0] * 0.01f,
                                 delta[1] * 0.01f);
            return true;
        }

        // Pan
        viewer->panCamera(delta[0], delta[1]);
        return true;
    }

    return false;
}
