// Copyright (C) 2026 The LineageOS Project
// SPDX-License-Identifier: Apache-2.0
#include <aidl/custom/hardware/hwcontrol/BnHwControl.h>
#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android-base/strings.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

using aidl::custom::hardware::hwcontrol::BnHwControl;
using aidl::custom::hardware::hwcontrol::HwType;

static std::string nodeFor(HwType type) {
    const char* property;
    switch (type) {
        case HwType::KEYBOARD: property = "ro.vendor.sm8150.tablet.keyboard_node"; break;
        case HwType::STYLUS: property = "ro.vendor.sm8150.tablet.stylus_node"; break;
        case HwType::TAP2WAKE: property = "ro.vendor.sm8150.tablet.double_tap_node"; break;
        default: return {};
    }
    return android::base::GetProperty(property, "");
}

class HwControl final : public BnHwControl {
    ndk::ScopedAStatus getHwState(HwType type, int32_t* result) override {
        std::string path = nodeFor(type), value;
        if (path.empty()) return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
        if (!android::base::ReadFileToString(path, &value)) {
            PLOG(ERROR) << "Cannot read " << path;
            return ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
        }
        *result = android::base::Trim(value) == "1";
        return ndk::ScopedAStatus::ok();
    }
    ndk::ScopedAStatus setHwState(HwType type, int32_t state) override {
        std::string path = nodeFor(type);
        if (path.empty()) return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
        if (!android::base::WriteStringToFile(state > 0 ? "1" : "0", path)) {
            PLOG(ERROR) << "Cannot write " << path;
            return ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
        }
        return ndk::ScopedAStatus::ok();
    }
};

int main() {
    ABinderProcess_setThreadPoolMaxThreadCount(8);
    auto service = ndk::SharedRefBase::make<HwControl>();
    const std::string instance = std::string(HwControl::descriptor) + "/default";
    CHECK(AServiceManager_addService(service->asBinder().get(), instance.c_str()) == STATUS_OK);
    ABinderProcess_joinThreadPool();
    return 1;
}
