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

PRODUCT_NAME := yaap_aston
PRODUCT_DEVICE := aston
PRODUCT_MANUFACTURER := OnePlus
PRODUCT_BRAND := OnePlus
PRODUCT_MODEL := CPH2609

# Boot animation
TARGET_SCREEN_HEIGHT := 2780
TARGET_SCREEN_WIDTH := 1264

# Build info
PRODUCT_BUILD_PROP_OVERRIDES += \
    TARGET_DEVICE=$(PRODUCT_SYSTEM_DEVICE) \
    TARGET_NAME=$(PRODUCT_SYSTEM_NAME)

PRODUCT_GMS_CLIENTID_BASE := android-oneplus