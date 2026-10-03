# Optional shared tablet implementation

Phones retain the existing sm8150-common product. TARGET_IS_TABLET selects the tablet settings and hardware-control modules.

Device-owned configuration:

- Set SOONG_CONFIG_XIAOMI_SM8150_TABLET_settings_config to a qualified android_library module providing settings resource overrides. Devices can depend on sm8150_tablet_settings_defaults and override only the supported features and values.
- Feature booleans: config_support_dirac, stylus, keyboard, tap2wake, thermal and refresh. Defaults are disabled.
- Settings data: config_dirac_effect_uuid; config_thermal_node; seven config_thermal_profile_values in default/benchmark/browser/camera/dialer/gaming/streaming order; config_refresh_rate_default/standard/extreme; optional refresh profile icons.
- Hardware-control paths: ro.vendor.sm8150.tablet.keyboard_node, stylus_node and double_tap_node. Missing paths report an unsupported operation. The backend writes normalized 0/1 and reads trimmed 0/1 states; device init and SELinux supply access to the actual nodes.
- TARGET_POWER_HINT_CONFIG selects the device's libperfmgr JSON. The standard Xiaomi Power HAL implementation is shared.
- TARGET_USES_DEVICE_ROOTDIR disables only the common modules overridden by a device rootdir. Common scripts and libraries remain available without requiring duplicate implementations.

The Binder descriptor and application package are unchanged from the original tablet implementation. Device-specific nodes, clocks, audio calibration, partition layout and sysfs policy are not defined by this directory.
