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
#ifndef PLUGIN_RENDER_H
#define PLUGIN_RENDER_H

#include <string>
#include <unordered_map>
#include <vector>
#include <ace/xcomponent/native_interface_xcomponent.h>
#include "../utils/napi/napi_utils.h"
#include "Surface.h"
#include "egl_core.h"
#include "DrawPoint.h"

class PluginRender {
public:
    explicit PluginRender(std::string &id);
    ~PluginRender()
    {
        if (nullptr != m_eglCore && isTest) {
            m_eglCore->Release();
            delete m_eglCore;
            m_eglCore = nullptr;
        }
    }
    static PluginRender *GetInstance(std::string &id);
    static void Release(std::string &id);
    static napi_value NapiDrawRectangle(napi_env env, napi_callback_info info);
    ////////////////////////XComponent////////////////////////////
    static OH_NativeXComponent_Callback *getNXComponentCallback();
    void setNativeXComponent(OH_NativeXComponent *component);
    void onSurfaceCreated(OH_NativeXComponent *component, void *window);
    void onSurfaceChanged(OH_NativeXComponent *component, void *window);
    void onSurfaceDestroyed(OH_NativeXComponent *component, void *window);
    void dispatchTouchEvent(OH_NativeXComponent *component, void *window);
    static std::string getXComponentId(napi_env env, napi_callback_info info);
    void setXComponentAndNativeWindow(std::string &id, OH_NativeXComponent *component, void *window);
    OH_NativeXComponent *getXComponent(std::string &id);
    void *getNativeWindow(std::string &id);
    void Export(napi_env env, napi_value exports);
    
    void draw(SkCanvas* skCanvas);

public:
    static std::unordered_map<std::string, PluginRender *> m_instance;
    static OH_NativeXComponent_Callback m_callback;
    std::unordered_map<std::string, OH_NativeXComponent *> nativeXComponentMap_;
    std::unordered_map<std::string, void *> nativeWindowMap_;
    OH_NativeXComponent *component_;
    std::string id_;
    uint64_t width_ = 0;
    uint64_t height_ = 0;
    /*typedef struct {
        int32_t id;                              // 触摸点的唯一标识符
        float screenX;                           // 触摸点相对于屏幕左边缘的 X 坐标
        float screenY;                           // 触摸点相对于屏幕上边缘的 Y 坐标
        float x;                                 // 触摸点相对于触摸元素左边缘的 X 坐标
        float y;                                 // 触摸点相对于触摸元素上边缘的 Y 坐标
        OH_NativeXComponent_TouchEventType type; // 触摸事件类型
        double size;                             // 手指触摸屏幕的接触面积
        float force;                             // 当前触摸事件的压力
        int64_t deviceId;                        // 生成当前触摸事件的设备 ID
        int64_t timeStamp;                       // 当前触摸事件的时间戳
        OH_NativeXComponent_TouchPoint touchPoints[OH_MAX_TOUCH_POINTS_NUMBER]; // 当前触摸点数组
        uint32_t numPoints;                                                     // 当前触摸点的数量
    } OH_NativeXComponent_TouchEvent;*/
    OH_NativeXComponent_TouchEvent touchEvent_;

    EGLCore *m_eglCore;
    std::string m_id;
    
    const static bool isTest = false;
    
    SkPictureRecorder fRecorder;
    WindowSurface *fWindowSurface = nullptr;
    std::vector<DrawPoint> currentPath;
};
#endif