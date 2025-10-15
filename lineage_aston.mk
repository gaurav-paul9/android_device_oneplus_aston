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

# Lunaris-Specific Flags
AXION_MAINTAINER := Gaurav_Paul
AXION_PROCESSOR := Snapdragon_8_Gen_2_(4_nm)
PRODUCT_NO_CAMERA:= false
TARGET_BOOT_ANIMATION_RES := 1080
TARGET_DISABLE_EPPE := true
WITH_GMS := true
TARGET_USES_CORE_GAPPS := true
WITH_BCR := true
TARGET_SUPPORTS_QUICK_TAP := true

PRODUCT_NAME := lineage_aston
PRODUCT_DEVICE := aston
PRODUCT_MANUFACTURER := OnePlus
PRODUCT_BRAND := OnePlus
PRODUCT_MODEL := CPH2609

PRODUCT_GMS_CLIENTID_BASE := android-oneplus

PRODUCT_BUILD_PROP_OVERRIDES += \
    BuildDesc="qssi-user 15 AP3A.240617.008 1758859159691 release-keys" \
    BuildFingerprint=OnePlus/CPH2609EEA/OP5D35L1:15/TP1A.220905.001/U.R4T3.1632d6d_9d6f90_9e012b:user/release-keys \
    DeviceName=OP5D35L1 \
    DeviceProduct=CPH2609 \
    SystemDevice=OP5D35L1 \
    SystemName=CPH2609
#sign
PRODUCT_DEFAULT_DEV_CERTIFICATE := vendor/lineage-priv/keys/releasekey
