/*
 * Copyright 2021 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#include "Surface.h"

#include "include/gpu/ganesh/SkSurfaceGanesh.h"
#include "include/core/SkPicture.h"
#include "DisplayParams.h"
#include "WindowContextFactory_ohos.h"

WindowSurface::WindowSurface(NativeWindow* win, std::unique_ptr<skwindow::WindowContext> wctx)
    : fWindow(win)
    , fWindowContext(std::move(wctx))
{
    SkASSERT(fWindow);
    SkASSERT(fWindowContext);

    fSurface = fWindowContext->getBackbufferSurface();
}

void WindowSurface::release(napi_env env) {
    fWindowContext.reset();
    // NativeWindow_release(fWindow);
    fSurface.release();
    fWindowContext.release();
}

SkCanvas* WindowSurface::getCanvas() {
    if (fSurface) {
        return fSurface->getCanvas();
    }
    return nullptr;
}

void WindowSurface::flushAndSubmit() {
    skgpu::ganesh::FlushAndSubmit(fSurface);
    fWindowContext->swapBuffers();
    fSurface = fWindowContext->getBackbufferSurface();
}
