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

# Euclid Stuff
#For GMS
WITH_GMS := true
TARGET_BUILD_DOTGALLERY := true
TARGET_BUILD_BCR := true

#For maintainer
PRODUCT_SYSTEM_PROPERTIES += \
    ro.lineage.maintainer=Gaurav Paul       

# Maintainer
EUCLID_BUILD_TYPE := UNOFFICIAL
EUCLID_MAINTAINER := Gaurav Paul

#Gapps
EUCLID_GAPPS := true
TARGET_SUPPORTS_GOOGLE_RECORDER := true
TARGET_INCLUDE_STOCK_ARCORE := true
TARGET_INCLUDE_LIVE_WALLPAPERS := true
TARGET_INCLUDE_PIXEL_LAUNCHER := true

#UDFPS
EXTRA_UDFPS_ANIMATIONS := true
TARGET_HAS_UDFPS := true

#Misc.
TARGET_SUPPORTS_TOUCHGESTURES := true

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
