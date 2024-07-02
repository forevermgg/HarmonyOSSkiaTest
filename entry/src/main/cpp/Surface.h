/*
 * Copyright 2021 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef HarmonyOSKit_Surface_DEFINED
#define HarmonyOSKit_Surface_DEFINED

#include "WindowContext.h"
#include "include/core/SkRefCnt.h"
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkImage.h"
#include "include/core/SkPictureRecorder.h"
#include "include/core/SkRefCnt.h"
#include "include/core/SkSurface.h"
#include "include/core/SkTypes.h"
#include <napi/native_api.h>
#include <native_window/external_window.h>

class Surface : public SkRefCnt {
public:
    virtual void release(napi_env env) = 0;
    virtual void flushAndSubmit() = 0;
    virtual SkCanvas* getCanvas() = 0;

    int width()  const { return fSurface ? fSurface->width()  : 0; }
    int height() const { return fSurface ? fSurface->height() : 0; }

    sk_sp<SkImage> makeImageSnapshot() const {
        return fSurface ? fSurface->makeImageSnapshot() : nullptr;
    }

protected:
    sk_sp<SkSurface> fSurface;
};

class WindowSurface final : public Surface {
public:
    WindowSurface(NativeWindow* win, std::unique_ptr<skwindow::WindowContext> wctx);

private:
    void release(napi_env env) override;
    SkCanvas* getCanvas() override;
    void flushAndSubmit() override;

    NativeWindow*                         fWindow;
    std::unique_ptr<skwindow::WindowContext> fWindowContext;
};

#endif
