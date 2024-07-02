/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <stdint.h>
#include <string>
#include <js_native_api.h>
#include <js_native_api_types.h>
#include <hilog/log.h>
#include "WindowContextFactory_ohos.h"
#include "common/common.h"
#include "include/gpu/gl/GrGLInterface.h"
#include "plugin_render.h"
#include "../manager/plugin_manager.h"
#include "WindowContextFactory_ohos.h"
#include "include/core/SkPicture.h"
#include "include/gpu/ganesh/gl/GrGLDirectContext.h"
#include "include/gpu/GrDirectContext.h"
#include "include/core/SkFontMgr.h"
#include <include/core/SkFont.h>
#include "include/core/SkTextBlob.h"

std::unordered_map<std::string, PluginRender *> PluginRender::m_instance;
OH_NativeXComponent_Callback PluginRender::m_callback;

void OnSurfaceCreatedCB(OH_NativeXComponent *component, void *window) {
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "Callback", "OnSurfaceCreatedCB");
    if ((nullptr == component) || (nullptr == window)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "Callback",
                     "OnSurfaceCreatedCB: component or window is null");
        return;
    }

    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NATIVEXCOMPONENT_RESULT_SUCCESS != OH_NativeXComponent_GetXComponentId(component, idStr, &idSize)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "Callback",
                     "OnSurfaceCreatedCB: Unable to get XComponent id");
        return;
    }

    std::string id(idStr);
    auto render = PluginRender::GetInstance(id);
    uint64_t width;
    uint64_t height;
    int32_t xSize = OH_NativeXComponent_GetXComponentSize(component, window, &width, &height);
    if ((OH_NATIVEXCOMPONENT_RESULT_SUCCESS == xSize) && (nullptr != render)) {
        if (PluginRender::isTest) {
            if (render->m_eglCore->EglContextInit(window, width, height)) {
                render->m_eglCore->Background();
            }
        }
    }
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "Callback",
                 "OnSurfaceCreatedCB: canvas size, gl width = %{public}d  height = %{public}d", width, height);
    render->onSurfaceCreated(component, window);
    render->setXComponentAndNativeWindow(id, component, window);
}

void OnSurfaceChangedCB(OH_NativeXComponent *component, void *window) {
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "Callback", "OnSurfaceChangedCB");
    if ((nullptr == component) || (nullptr == window)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "Callback",
                     "OnSurfaceChangedCB: component or window is null");
        return;
    }

    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NATIVEXCOMPONENT_RESULT_SUCCESS != OH_NativeXComponent_GetXComponentId(component, idStr, &idSize)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "Callback",
                     "OnSurfaceChangedCB: Unable to get XComponent id");
        return;
    }

    std::string id(idStr);
    auto render = PluginRender::GetInstance(id);
    if (nullptr != render) {
        OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "Callback", "surface changed");
    }
    render->onSurfaceChanged(component, window);
}

void OnSurfaceDestroyedCB(OH_NativeXComponent *component, void *window) {
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "Callback", "OnSurfaceDestroyedCB");
    if ((nullptr == component) || (nullptr == window)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "Callback",
                     "OnSurfaceDestroyedCB: component or window is null");
        return;
    }

    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NATIVEXCOMPONENT_RESULT_SUCCESS != OH_NativeXComponent_GetXComponentId(component, idStr, &idSize)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "Callback",
                     "OnSurfaceDestroyedCB: Unable to get XComponent id");
        return;
    }

    std::string id(idStr);
    auto render = PluginRender::GetInstance(id);
    if (nullptr != render) {
        OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "Callback", "surface destroyed");
    }
    render->onSurfaceDestroyed(component, window);
    PluginRender::Release(id);
}

void DispatchTouchEventCB(OH_NativeXComponent *component, void *window) {
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "Callback", "DispatchTouchEventCB");
    if ((nullptr == component) || (nullptr == window)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "Callback",
                     "DispatchTouchEventCB: component or window is null");
        return;
    }

    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NATIVEXCOMPONENT_RESULT_SUCCESS != OH_NativeXComponent_GetXComponentId(component, idStr, &idSize)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "Callback",
                     "DispatchTouchEventCB: Unable to get XComponent id");
        return;
    }

    std::string id(idStr);
    PluginRender *render = PluginRender::GetInstance(id);
    if (nullptr != render) {
        if (PluginRender::isTest) {
            render->m_eglCore->ChangeColor();
        }
    }
    render->dispatchTouchEvent(component, window);
}

PluginRender::PluginRender(std::string &id) {
    this->m_id = id;
    this->m_eglCore = new EGLCore();
    OH_NativeXComponent_Callback *renderCallback = &PluginRender::m_callback;
    renderCallback->OnSurfaceCreated = OnSurfaceCreatedCB;
    renderCallback->OnSurfaceChanged = OnSurfaceChangedCB;
    renderCallback->OnSurfaceDestroyed = OnSurfaceDestroyedCB;
    renderCallback->DispatchTouchEvent = DispatchTouchEventCB;
}

PluginRender *PluginRender::GetInstance(std::string &id) {
    if (m_instance.find(id) == m_instance.end()) {
        PluginRender *instance = new PluginRender(id);
        m_instance[id] = instance;
        return instance;
    } else {
        return m_instance[id];
    }
}

void PluginRender::Export(napi_env env, napi_value exports) {
    if ((nullptr == env) || (nullptr == exports)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender", "Export: env or exports is null");
        return;
    }

    napi_property_descriptor desc[] = {
        {"drawRectangle", nullptr, PluginRender::NapiDrawRectangle, nullptr, nullptr, nullptr, napi_default, nullptr}};
    if (napi_ok != napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender", "Export: napi_define_properties failed");
    }
}

// NAPI registration method type napi_callback. If no value is returned, nullptr is returned.
napi_value PluginRender::NapiDrawRectangle(napi_env env, napi_callback_info info) {
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "PluginRender", "NapiDrawRectangle");
    if ((nullptr == env) || (nullptr == info)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender", "NapiDrawRectangle: env or info is null");
        return nullptr;
    }

    napi_value thisArg;
    if (napi_ok != napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, nullptr)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender", "NapiDrawRectangle: napi_get_cb_info fail");
        return nullptr;
    }

    napi_value exportInstance;
    if (napi_ok != napi_get_named_property(env, thisArg, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender",
                     "NapiDrawRectangle: napi_get_named_property fail");
        return nullptr;
    }

    OH_NativeXComponent *nativeXComponent = nullptr;
    if (napi_ok != napi_unwrap(env, exportInstance, reinterpret_cast<void **>(&nativeXComponent))) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender", "NapiDrawRectangle: napi_unwrap fail");
        return nullptr;
    }

    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NATIVEXCOMPONENT_RESULT_SUCCESS != OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &idSize)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender",
                     "NapiDrawRectangle: Unable to get XComponent id");
        return nullptr;
    }

    std::string id(idStr);
    PluginRender *render = PluginRender::GetInstance(id);
    if (render) {
        render->m_eglCore->Draw();
        OH_LOG_Print(LOG_APP, LOG_INFO, LOG_PRINT_DOMAIN, "PluginRender", "render->m_eglCore->Draw() executed");
    }
    return nullptr;
}

void PluginRender::Release(std::string &id) {
    PluginRender *render = PluginRender::GetInstance(id);
    if (nullptr != render) {
        if (PluginRender::isTest) {
            render->m_eglCore->Release();
            delete render->m_eglCore;
            render->m_eglCore = nullptr;
        }
        delete render;
        render = nullptr;
        m_instance.erase(m_instance.find(id));
    }
}


OH_NativeXComponent_Callback *PluginRender::getNXComponentCallback() {
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender", "getNXComponentCallback");
    return &PluginRender::m_callback;
}

void PluginRender::setNativeXComponent(OH_NativeXComponent *component) {
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender", "setNativeXComponent");
    component_ = component;
    OH_NativeXComponent_RegisterCallback(component_, &PluginRender::m_callback);
}

void PluginRender::onSurfaceCreated(OH_NativeXComponent *component, void *window) {
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender", "OnSurfaceCreated");
    int32_t ret = OH_NativeXComponent_GetXComponentSize(component, window, &width_, &height_);
    if (ret == OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender", "OnSurfaceCreated-->success");
    }
    skwindow::DisplayParams params;
    auto winctx = skwindow::MakeGLForOhos(static_cast<NativeWindow *>(window), component, params);
    fWindowSurface = new WindowSurface(static_cast<NativeWindow *>(window), std::move(winctx));
    if (auto *windowSurface = reinterpret_cast<Surface *>(fWindowSurface)) {
        draw(windowSurface->getCanvas());
        windowSurface->flushAndSubmit();
    }
}

void PluginRender::onSurfaceChanged(OH_NativeXComponent *component, void *window) {
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender", "OnSurfaceChanged");
}

void PluginRender::onSurfaceDestroyed(OH_NativeXComponent *component, void *window) {
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender", "OnSurfaceDestroyed");
    if (auto *windowSurface = reinterpret_cast<Surface *>(fWindowSurface)) {
        windowSurface->release(nullptr);
        delete windowSurface;
    }
}

void PluginRender::dispatchTouchEvent(OH_NativeXComponent *component, void *window) {
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender", "DispatchTouchEvent");
    int32_t ret = OH_NativeXComponent_GetTouchEvent(component, window, &touchEvent_);
    if (ret == OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender", "DispatchTouchEvent-->success");
        OH_LOG_Print(
            LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender",
            "DispatchTouchEvent Touch Info : x = %{public}f, y = %{public}f screenx = %{public}f, screeny = %{public}f",
            touchEvent_.x, touchEvent_.y, touchEvent_.screenX, touchEvent_.screenY);

        if (touchEvent_.type == OH_NativeXComponent_TouchEventType::OH_NATIVEXCOMPONENT_DOWN) {
            auto drawPoint = DrawPoint(touchEvent_.x, touchEvent_.y, DrawPoint::DrawPointType::START);
            currentPath.push_back(drawPoint);
        } else if (touchEvent_.type == OH_NativeXComponent_TouchEventType::OH_NATIVEXCOMPONENT_MOVE) {
            auto drawPoint = DrawPoint(touchEvent_.x, touchEvent_.y, DrawPoint::DrawPointType::LINE);
            currentPath.push_back(drawPoint);
        } else if (touchEvent_.type == OH_NativeXComponent_TouchEventType::OH_NATIVEXCOMPONENT_UP) {
            auto drawPoint = DrawPoint(touchEvent_.x, touchEvent_.y, DrawPoint::DrawPointType::LINE);
            currentPath.push_back(drawPoint);
        } else if (touchEvent_.type == OH_NativeXComponent_TouchEventType::OH_NATIVEXCOMPONENT_CANCEL) {
            /*// find the last "START" event
            auto lastIndex = std::find_if(currentPath.rbegin(), currentPath.rend(), [](const DrawPoint &point) {
                return point.type() == DrawPoint::DrawPointType::START;
            });

            // if found, keep elements from 0 until very last event before last MOVE event
            if (lastIndex != currentPath.rend()) {
                currentPath.erase(lastIndex.base(), currentPath.end());
            }*/
            int lastIndex =
                findLastIndex(currentPath, [](const DrawPoint &point) { return point.type() == DrawPoint::DrawPointType::START; });

            // if found, keep elements from 0 until very last event before last MOVE event
            if (lastIndex > 0) {
                currentPath.erase(currentPath.begin() + lastIndex + 1, currentPath.end());
            }
        }

        SkPath path;
        for (const auto &point : currentPath) {
            if (point.type() == DrawPoint::DrawPointType::START) {
                path.moveTo(point.x(), point.y());
            } else {
                path.lineTo(point.x(), point.y());
            }
        }
        SkPaint paint;
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setAntiAlias(true);
        if (auto *windowSurface = reinterpret_cast<Surface *>(fWindowSurface)) {
            windowSurface->getCanvas()->save();
            windowSurface->getCanvas()->drawPath(path, paint);
            windowSurface->getCanvas()->restore();
            windowSurface->flushAndSubmit();
        }

        switch (touchEvent_.type) {
        case OH_NATIVEXCOMPONENT_TOOL_TYPE_FINGER: {

        } break;
        case OH_NATIVEXCOMPONENT_TOOL_TYPE_PEN: {

        } break;
        case OH_NATIVEXCOMPONENT_TOOL_TYPE_RUBBER: {

        } break;
        case OH_NATIVEXCOMPONENT_TOOL_TYPE_BRUSH: {

        } break;
        case OH_NATIVEXCOMPONENT_TOOL_TYPE_PENCIL: {

        } break;
        case OH_NATIVEXCOMPONENT_TOOL_TYPE_AIRBRUSH: {

        } break;
        case OH_NATIVEXCOMPONENT_TOOL_TYPE_MOUSE: {

        } break;
        case OH_NATIVEXCOMPONENT_TOOL_TYPE_LENS: {

        } break;
        case OH_NATIVEXCOMPONENT_TOOL_TYPE_UNKNOWN: {

        } break;
        }
        for (uint32_t i = 0; i < touchEvent_.numPoints; i++) {
            OH_LOG_Print(
                LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender",
                "DispatchTouchEvent Touch Info : dots[%{public}d] id %{public}d x = %{public}f, y = %{public}f", i,
                touchEvent_.touchPoints[i].id, touchEvent_.touchPoints[i].x, touchEvent_.touchPoints[i].y);
            OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender",
                         "DispatchTouchEvent Touch Info : screenx = %{public}f, screeny = %{public}f",
                         touchEvent_.touchPoints[i].screenX, touchEvent_.touchPoints[i].screenY);
            OH_NativeXComponent_TouchPointToolType toolType =
                OH_NativeXComponent_TouchPointToolType::OH_NATIVEXCOMPONENT_TOOL_TYPE_UNKNOWN;
            OH_NativeXComponent_GetTouchPointToolType(component, i, &toolType);
            float tiltX = 0.0f;
            float tiltY = 0.0f;
            OH_NativeXComponent_GetTouchPointTiltX(component, 0, &tiltX);
            OH_NativeXComponent_GetTouchPointTiltY(component, 0, &tiltY);
            OH_LOG_Print(
                LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender",
                "DispatchTouchEvent Touch Info : [%{public}d] %{public}u tiltX = %{public}lf, tiltY = %{public}lf", i,
                toolType, tiltX, tiltY);
        }
    }
}

void PluginRender::draw(SkCanvas *skCanvas) {
    skCanvas->clear(SK_ColorWHITE);
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(SK_ColorBLUE);

    SkFont font;
    // 设置字体尺寸
    font.setSize(64);
    /*// 创建SkFontMgr对象
    sk_sp<SkFontMgr> fontMgr = SkFontMgr::RefDefault();

    // 从文件系统中的字体文件创建SkTypeface对象
    const char *fontPath = "/system/fonts/HarmonyOS_Sans_SC_Light.ttf";
    sk_sp<SkTypeface> typeface = fontMgr->makeFromFile(fontPath);

    if (!typeface) {
      OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender", "typeface == null");
    }*/
    // font.setTypeface(SkTypeface::MakeFromFile("/system/fonts/HarmonyOS_Sans_SC_Light.ttf")); // 设置字体
    SkString text("Hello, Skia! 中国");
    sk_sp<SkFontMgr> font_mgr = SkFontMgr::RefDefault();// SkFontStyle::Normal()
    /*SkFontStyle skFontStyle = SkFontStyle(900, 5, 0);
    font_mgr->matchFamilyStyleCharacter("", skFontStyle, nullptr, 0, 19968);*/
    sk_sp<SkTypeface> typeface = font_mgr->matchFamilyStyle(/*"HarmonyOS Sans"*/nullptr, SkFontStyle::Normal());
    font.setTypeface(typeface);
    font.setTypeface(SkTypeface::MakeFromFile("/system/fonts/HarmonyOS_Sans_SC_Regular.ttf"));
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender",
                 "typeface fname : %{public}s", "dddd");

    // font.setTypeface(typeface);
    skCanvas->clear(SK_ColorWHITE);
    skCanvas->drawSimpleText(text.c_str(), text.size(), SkTextEncoding::kUTF8, 300, 300, font, paint); // 在画布上绘制字体

    const char str[] = "Hello World! 中国";
    sk_sp<SkTextBlob> blob = SkTextBlob::MakeFromText(str, std::size(str), font, SkTextEncoding::kUTF8);
    skCanvas->drawTextBlob(blob, 40, 400, paint);

    skCanvas->save();
    skCanvas->translate(SkIntToScalar(128), SkIntToScalar(128));//HarmonyOS-Sans-Light
    skCanvas->rotate(SkIntToScalar(45));
    SkRect rect = SkRect::MakeXYWH(-90.5f, -90.5f, 181.0f, 181.0f);
    paint.setColor(SK_ColorBLUE);
    skCanvas->drawRect(rect, paint);
    skCanvas->restore();
}

std::string PluginRender::getXComponentId(napi_env env, napi_callback_info info) {
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender", "getXComponentId");
    napi_value exportInstance;
    napi_value thisArg;
    napi_status status;
    OH_NativeXComponent *nativeXComponent = nullptr;
    int32_t ret;
    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;

    NAPI_CALL(env, napi_get_cb_info(env, info, NULL, NULL, &thisArg, NULL));

    status = napi_get_named_property(env, thisArg, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance);
    if (status != napi_ok) {
        return nullptr;
    };

    status = napi_unwrap(env, exportInstance, reinterpret_cast<void **>(&nativeXComponent));
    if (status != napi_ok) {
        return nullptr;
    }

    ret = OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &idSize);
    if (ret != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        return nullptr;
    }

    std::string id(idStr);
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender", "getXComponentId id:%s", (char *)id.c_str());
    return id;
}

void PluginRender::setXComponentAndNativeWindow(std::string &id, OH_NativeXComponent *component, void *window) {
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender", "SetXComponentAndNativeWindow");
    if (nativeXComponentMap_.find(id) == nativeXComponentMap_.end()) {
        nativeXComponentMap_[id] = component;
    } else {
        if (nativeXComponentMap_[id] != component) {
            nativeXComponentMap_[id] = component;
        }
    }
    if (nativeWindowMap_.find(id) == nativeWindowMap_.end()) {
        nativeWindowMap_[id] = window;
    } else {
        if (nativeWindowMap_[id] != window) {
            nativeWindowMap_[id] = window;
        }
    }
}

OH_NativeXComponent *PluginRender::getXComponent(std::string &id) {
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender", "getXComponent");
    if (nativeXComponentMap_.find(id) == nativeXComponentMap_.end()) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender", "getXComponent null");
        return nullptr;
    } else {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender", "getXComponent success");
        return nativeXComponentMap_[id];
    }
}

void *PluginRender::getNativeWindow(std::string &id) {
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender", "getNativeWindow");
    if (nativeWindowMap_.find(id) == nativeWindowMap_.end()) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender", "getNativeWindow null");
        return nullptr;
    } else {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginRender", "getNativeWindow success");
        return nativeWindowMap_[id];
    }
}
