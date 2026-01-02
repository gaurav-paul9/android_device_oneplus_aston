#
# Copyright (C) 2021-2025 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit_only.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)

# Inherit from aston device
$(call inherit-product, device/oneplus/aston/device.mk)

# Inherit some common Lineage stuff.
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)

# Evox-Specific Flags
PRODUCT_NO_CAMERA:= false
TARGET_DISABLE_EPPE := true
TARGET_SUPPORTS_QUICK_TAP := true
BUILD_BCR := true
WITH_GMS := true
TARGET_HAS_UDFPS := true

# Flashlght_strength
TORCH_STR_SUPPORTED := true

# Charging
BYPASS_CHARGE_SUPPORTED := true

# Blur
TARGET_ENABLE_BLUR := true

#Boot Animation
TARGET_SCREEN_HEIGHT := 2400
TARGET_SCREEN_WIDTH := 1080

#ScrollOptimizer
persist.sys.perf.scroll_opt = true
persist.sys.perf.scroll_opt.heavy_app = 2

PRODUCT_NAME := lineage_aston
PRODUCT_DEVICE := aston
PRODUCT_MANUFACTURER := OnePlus
PRODUCT_BRAND := OnePlus
PRODUCT_MODEL := CPH2585

PRODUCT_GMS_CLIENTID_BASE := android-oneplus

PRODUCT_BUILD_PROP_OVERRIDES += \
    BuildDesc="qssi-user 16 BP2A.250605.015 1763371040443 release-keys" \
    BuildFingerprint=OnePlus/CPH2585IN/OP5D35L1:16/TP1A.220905.001/U.R4T3.2d10abf-c190f3-c190f4:user/release-keys \
    DeviceName=OP5D35L1 \
    DeviceProduct=CPH2585 \
    SystemDevice=OP5D35L1 \
    SystemName=CPH2585
