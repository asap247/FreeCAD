#include "TrackpadNavigationStyle.h"

#include <Inventor/events/SoEvent.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/events/SoMouseWheelEvent.h>
#include <Inventor/events/SoKeyboardEvent.h>

#include <Gui/View3DInventorViewer.h>

using namespace Gui;

TrackpadNavigationStyle::TrackpadNavigationStyle()
{
}

const char* TrackpadNavigationStyle::getName() const
{
    return "Trackpad";
}

bool TrackpadNavigationStyle::processSoEvent(const SoEvent* ev)
{
    auto* viewer = this->getViewer();
    if (!viewer)
        return false;

    /* ------------------------------------------------------------
     * One-finger drag: cursor move only (no camera motion)
     * ------------------------------------------------------------ */
    if (const SoLocation2Event* loc =
            dynamic_cast<const SoLocation2Event*>(ev)) {

        SbVec2s pos = loc->getPosition();

        lastPos = pos;
        hasLastPos = true;

        return true; // consume event, no click required
    }

    /* ------------------------------------------------------------
     * Two-finger gestures (Qt sends these as wheel events on macOS)
     * ------------------------------------------------------------ */
    if (const SoMouseWheelEvent* wheel =
            dynamic_cast<const SoMouseWheelEvent*>(ev)) {

        SbVec2f delta = wheel->getDelta();
        delta *= 0.5f; // macOS sensitivity tuning

        const bool shift =
            wheel->getModifiers() & SoEvent::SHIFT_DOWN;

        const bool ctrl =
            wheel->getModifiers() & SoEvent::CTRL_DOWN;

        /* --------------------------------------------------------
         * Pinch zoom (Ctrl modifier on macOS Qt)
         * -------------------------------------------------------- */
        if (ctrl) {
            viewer->zoomCamera(delta[1] * 0.01f);
            return true;
        }

        /* --------------------------------------------------------
         * Shift + two-finger drag = rotate
         * -------------------------------------------------------- */
        if (shift) {
            viewer->rotateCamera(delta[0] * 0.01f,
                                 delta[1] * 0.01f);
            return true;
        }

        /* --------------------------------------------------------
         * Two-finger drag = pan
         * -------------------------------------------------------- */
        viewer->panCamera(delta[0], delta[1]);
        return true;
    }

    return false;
}
