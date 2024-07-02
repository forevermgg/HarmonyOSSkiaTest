/*
 * Copyright 2023 Google LLC
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef WindowContextFactory_ohos_DEFINED
#define WindowContextFactory_ohos_DEFINED

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <memory>
#include <native_window/external_window.h>

namespace skwindow {

class WindowContext;
struct DisplayParams;

std::unique_ptr<WindowContext> MakeGLForOhos(NativeWindow*, OH_NativeXComponent *, const DisplayParams&);

}  // namespace skwindow

#endif // WindowContextFactory_ohos_DEFINED
