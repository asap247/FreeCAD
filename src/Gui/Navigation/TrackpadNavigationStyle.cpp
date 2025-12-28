// SPDX-License-Identifier: LGPL-2.1-or-later

#include "TrackpadNavigationStyle.h"

#include <Gui/NavigationStyle.h>
#include <Gui/View3DInventorViewer.h>

#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/events/SoMouseWheelEvent.h>

using namespace Gui;

TYPESYSTEM_SOURCE(Gui::TrackpadNavigationStyle, Gui::UserNavigationStyle)

// --------------------------------------------------------------------

TrackpadNavigationStyle::TrackpadNavigationStyle()
{
    lastPos.setValue(0.f, 0.f);
}

TrackpadNavigationStyle::~TrackpadNavigationStyle() = default;

const char* TrackpadNavigationStyle::getName() const
{
    return "Trackpad";
}

// --------------------------------------------------------------------

void TrackpadNavigationStyle::init()
{
    NavigationStyle::addType(
        "Trackpad",
        []() { return new TrackpadNavigationStyle; }
    );
}

// --------------------------------------------------------------------

SbBool TrackpadNavigationStyle::processSoEvent(const SoEvent* ev)
{
    // Let base class handle seek / selection / keyboard, etc.
    if (this->isSeekMode()) {
        return inherited::processSoEvent(ev);
    }

    const SoType type(ev->getTypeId());

    // ------------------------------------------------------------
    // Cursor movement (one-finger drag)
    // ------------------------------------------------------------
    if (type.isDerivedFrom(SoLocation2Event::getClassTypeId())) {
        const auto* event = static_cast<const SoLocation2Event*>(ev);
        const SbVec2f posn = normalizePixelPos(event->getPosition());
        lastPos = posn;

        // Do NOT consume the event – allow normal hover / selection logic
        return inherited::processSoEvent(ev);
    }

    // ------------------------------------------------------------
    // Trackpad gestures (two-finger pan, shift+pan rotate, pinch zoom)
    // ------------------------------------------------------------
    if (type.isDerivedFrom(SoMouseWheelEvent::getClassTypeId())) {
        const auto* wheel = static_cast<const SoMouseWheelEvent*>(ev);

        const SbVec2f delta(
            static_cast<float>(wheel->getDelta().getValue()[0]),
            static_cast<float>(wheel->getDelta().getValue()[1])
        );

        // Ignore tiny noise
        if (fabs(delta[0]) < 0.01f && fabs(delta[1]) < 0.01f) {
            return true;
        }

        const SbViewportRegion& vp =
            viewer->getSoRenderManager()->getViewportRegion();

        const float aspect = vp.getViewportAspectRatio();
        SoCamera* cam = viewer->getSoRenderManager()->getCamera();

        // --------------------------------------------------------
        // Pinch zoom (Ctrl + wheel)
        // --------------------------------------------------------
        if (wheel->getModifiers() & SoEvent::CTRL_DOWN) {
            const float zoomFactor = 1.0f + (-delta[1] * 0.0025f);
            if (zoomFactor > 0.0f) {
                viewer->zoomCamera(zoomFactor);
            }
            return true;
        }

        // --------------------------------------------------------
        // Rotate (Shift + two-finger drag)
        // --------------------------------------------------------
        if (wheel->getModifiers() & SoEvent::SHIFT_DOWN) {
            const SbVec2f rot(
                -delta[0] * 0.0025f,
                -delta[1] * 0.0025f
            );
            viewer->spinCamera(rot);
            return true;
        }

        // --------------------------------------------------------
        // Pan (two-finger drag)
        // --------------------------------------------------------
        const SbVec2f pan(
            -delta[0] * 0.0025f,
             delta[1] * 0.0025f
        );

        panCamera(
            cam,
            aspect,
            this->panningplane,
            SbVec2f(0.f, 0.f),
            pan
        );

        return true;
    }

    // ------------------------------------------------------------
    // Fallback
    // ------------------------------------------------------------
    return inherited::processSoEvent(ev);
}
