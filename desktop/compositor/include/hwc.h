// Hollywood Wayland Compositor
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#ifndef HWC_H
#define HWC_H

#include "application.h"
#include "compositor.h"

#if defined(hwComp)
#undef hwComp
#endif
#define hwComp (CompositorApp::compositorInstance())


#endif // HWC_H
