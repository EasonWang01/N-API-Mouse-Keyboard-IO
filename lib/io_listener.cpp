#include "napi.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <ApplicationServices/ApplicationServices.h>
#endif
#include <iostream>

Napi::FunctionReference eventCallbackRef; // Store JS callback

#ifdef _WIN32
// Windows-specific variables and hooks
HHOOK mouseHook;
HHOOK keyboardHook;

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        MSLLHOOKSTRUCT *mouse = (MSLLHOOKSTRUCT *)lParam;
        Napi::Env env = eventCallbackRef.Env();

        Napi::Object eventObj = Napi::Object::New(env);
        eventObj.Set("x", mouse->pt.x);
        eventObj.Set("y", mouse->pt.y);

        if (wParam == WM_MOUSEMOVE) {
            eventObj.Set("type", "mousemove");
        } else if (wParam == WM_LBUTTONDOWN) {
            eventObj.Set("type", "mousedown");
        } else if (wParam == WM_LBUTTONUP) {
            eventObj.Set("type", "mouseup");
        } else if (wParam == WM_MOUSEWHEEL) {
            eventObj.Set("type", "scroll");
            eventObj.Set("delta", GET_WHEEL_DELTA_WPARAM(mouse->mouseData));
        }

        // Emit the event to JS
        if (!eventCallbackRef.IsEmpty()) {
            eventCallbackRef.Call({eventObj});
        }
    }
    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT *kbd = (KBDLLHOOKSTRUCT *)lParam;
        Napi::Env env = eventCallbackRef.Env();

        Napi::Object eventObj = Napi::Object::New(env);
        eventObj.Set("type", "keydown");
        eventObj.Set("keycode", (int)kbd->vkCode);

        // Emit the event to JS
        if (!eventCallbackRef.IsEmpty()) {
            eventCallbackRef.Call({eventObj});
        }
    }
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

void StartWindowsHooks() {
    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnhookWindowsHookEx(mouseHook);
    UnhookWindowsHookEx(keyboardHook);
}
#endif

#ifdef __APPLE__
// macOS-specific code
CGEventRef mouseEventCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *userInfo) {
    CGPoint location = CGEventGetLocation(event);
    Napi::Env env = *(Napi::Env *)userInfo;

    Napi::Object eventObj = Napi::Object::New(env);
    eventObj.Set("x", location.x);
    eventObj.Set("y", location.y);

    if (type == kCGEventMouseMoved) {
        eventObj.Set("type", "mousemove");
    } else if (type == kCGEventLeftMouseDown) {
        eventObj.Set("type", "mousedown");
    } else if (type == kCGEventLeftMouseUp) {
        eventObj.Set("type", "mouseup");
    } else if (type == kCGEventScrollWheel) {
        int64_t scrollDelta = CGEventGetIntegerValueField(event, kCGScrollWheelEventDeltaAxis1);
        eventObj.Set("type", "scroll");
        eventObj.Set("delta", scrollDelta);
    }

    if (!eventCallbackRef.IsEmpty()) {
        eventCallbackRef.Call({eventObj});
    }
    return event;
}

CGEventRef keyboardEventCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *userInfo) {
    Napi::Env env = *(Napi::Env *)userInfo;

    if (type == kCGEventKeyDown) {
        CGKeyCode keycode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);

        Napi::Object eventObj = Napi::Object::New(env);
        eventObj.Set("type", "keydown");
        eventObj.Set("keycode", keycode);

        if (!eventCallbackRef.IsEmpty()) {
            eventCallbackRef.Call({eventObj});
        }
    }
    return event;
}

void StartMacOSHooks(Napi::Env env) {
    CGEventMask eventMask = (CGEventMaskBit(kCGEventMouseMoved) |
                             CGEventMaskBit(kCGEventLeftMouseDown) |
                             CGEventMaskBit(kCGEventLeftMouseUp) |
                             CGEventMaskBit(kCGEventScrollWheel) |
                             CGEventMaskBit(kCGEventKeyDown));

    CFMachPortRef eventTap = CGEventTapCreate(kCGHIDEventTap,
                                              kCGHeadInsertEventTap,
                                              kCGEventTapOptionDefault,
                                              eventMask,
                                              mouseEventCallback,
                                              (void *)&env);
    CFMachPortRef keyTap = CGEventTapCreate(kCGHIDEventTap,
                                            kCGHeadInsertEventTap,
                                            kCGEventTapOptionDefault,
                                            CGEventMaskBit(kCGEventKeyDown),
                                            keyboardEventCallback,
                                            (void *)&env);

    CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
    CFRunLoopSourceRef keyRunLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, keyTap, 0);

    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), keyRunLoopSource, kCFRunLoopCommonModes);

    CGEventTapEnable(eventTap, true);
    CGEventTapEnable(keyTap, true);

    CFRunLoopRun();
}
#endif

// N-API function wrapper for starting the event listener
Napi::Value StartListening(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsFunction()) {
        Napi::TypeError::New(env, "A callback function is required").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    // Store the JavaScript callback
    eventCallbackRef = Napi::Persistent(info[0].As<Napi::Function>());

#ifdef _WIN32
    StartWindowsHooks();
#elif defined(__APPLE__)
    StartMacOSHooks(env);
#endif

    return env.Undefined();
}

// Initialize the Node.js addon
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "startListening"), Napi::Function::New(env, StartListening));
    return exports;
}

NODE_API_MODULE(mouse_listener, Init)
