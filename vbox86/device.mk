#
# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# This is a build configuration for the product aspects that
# are specific to the emulator.

PRODUCT_PROPERTY_OVERRIDES := \
    ro.ril.hsxpa=1 \
    ro.ril.gprsclass=10 \
    wifi.interface=eth1
  
LOCAL_KERNEL := device/androVM/vbox86/kernel-vbox86

PRODUCT_COPY_FILES := \
    frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
    frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:system/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.xml \
    frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
    frameworks/native/data/etc/android.software.sip.voip.xml:system/etc/permissions/android.software.sip.voip.xml \
    frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
    device/androVM/vbox86/fstab.vbox86:root/fstab.vbox86 \
    development/data/etc/apns-conf.xml:system/etc/apns-conf.xml \
    device/androVM/vbox86/vold.conf:system/etc/vold.conf \
    device/androVM/vbox86/media_profiles.xml:system/etc/media_profiles.xml \
    device/androVM/vbox86/media_codecs.xml:system/etc/media_codecs.xml \
    device/androVM/vbox86/init.rc:root/init.rc \
    device/androVM/vbox86/init.vbox86.rc:root/init.vbox86.rc \
    device/androVM/vbox86/ueventd.vbox86.rc:root/ueventd.vbox86.rc \
    device/androVM/vbox86/init.androVM.sh:system/etc/init.androVM.sh \
    $(LOCAL_KERNEL):kernel \
    device/androVM/vbox86/dhcpcd.conf:system/etc/dhcpcd/dhcpcd.conf \
    device/androVM/vbox86/vbox/vboxguest.ko:system/lib/vboxguest.ko \
    device/androVM/vbox86/vbox/vboxsf.ko:system/lib/vboxsf.ko \
    device/androVM/vbox86/vbox/vboxvideo.ko:system/lib/vboxvideo.ko \
    device/androVM/vbox86/uvesafb.ko:system/lib/uvesafb.ko \
    device/androVM/vbox86/cfbcopyarea.ko:system/lib/cfbcopyarea.ko \
    device/androVM/vbox86/cfbfillrect.ko:system/lib/cfbfillrect.ko \
    device/androVM/vbox86/cfbimgblt.ko:system/lib/cfbimgblt.ko \
    frameworks/base/data/keyboards/qwerty.kl:system/usr/keylayout/AT_Translated_Set_2_keyboard.kl \
    device/androVM/vbox86/androVM_Virtual_Input.kl:system/usr/keylayout/androVM_Virtual_Input.kl \
    device/androVM/vbox86/wpa_supplicant.conf:data/misc/wifi/wpa_supplicant.conf \
    packages/wallpapers/LivePicker/android.software.live_wallpaper.xml:system/etc/permissions/android.software.live_wallpaper.xml \
    device/androVM/vbox86/audio_policy.conf:system/etc/audio_policy.conf \
    device/androVM/vbox86/androVM_Virtual_Input.idc:system/usr/idc/androVM_Virtual_Input.idc \
    device/androVM/vbox86/libicuuc_46.so:system/lib/libicuuc_46.so \
    device/androVM/vbox86/libicui18n_46.so:system/lib/libicui18n_46.so \
    #device/androVM/common/google/com.google.android.maps.jar:system/framework/com.google.android.maps.jar \
    #device/androVM/common/google/com.google.android.maps.xml:system/etc/permissions/com.google.android.maps.xml \
    #device/androVM/common/google/com.android.location.provider.jar:system/framework/com.android.location.provider.jar \
    #device/androVM/common/google/com.android.location.provider.xml:system/etc/permissions/com.android.location.provider.xml \
    #device/androVM/common/libhoudini/libhoudini.so:system/lib/libhoudini.so \
    #device/androVM/common/libhoudini/libdvm_houdini.so:system/lib/libdvm_houdini.so \

#arm_files := $(call all-files-under,device/androVM/common/libhoudini/arm)
#arm_files := check.xml libandroid_runtime.so libandroid.so libbinder.so libcamera_client.so libc_orig.so libcrypto.so libc.so libcutils.so libdl.so libEGL.so libemoji.so libETC1.so libexpat.so libgcomm_jni.so libGLESv1_CM.so libGLESv2.so libgui.so libhardware_legacy.so libhardware.so libicui18n.so libicuuc.so libjnigraphics.so libjpeg.so liblog.so libmedia.so libm.so libnativehelper.so libnetutils.so libnfc_ndef.so libOpenSLES.so libpixelflinger.so libskiagl.so libskia.so libsonivox.so libsqlite.so libssl.so libstdc++.so libtalk_jni.so libui.so libutils.so libvoicesearch.so libwpa_client.so libz.so linker
#PRODUCT_COPY_FILES += $(foreach arm_file,$(arm_files), device/androVM/common/libhoudini/arm/$(arm_file):system/lib/arm/$(arm_file))

PRODUCT_PACKAGES += \
    audio.primary.vbox86 \
    libaudioutils \
    mount.vboxsf \
    hwcomposer.default  \
    AstroFileManager \
    Camera \
    camera.default \
    busybox \
    v86d \
    tinyplay \
    tinycap \
    tinymix \
    LiveWallpapersPicker \
    ApiDemos \
    GestureBuilder \
    CubeLiveWallpapers \
    androVM-prop \
    vmconfig \
    vinput \
    get_androVM_host \
    local_opengl \
    netperf \
    netserver \
    tcpdump \
    sensors.vbox86 \
    test_vmci \
    setdpi \
    androVM_setprop \
    make_ext4fs \
    Superuser \
    busybox \


$(call inherit-product-if-exists,frameworks/base/build/tablet-dalvik-heap.mk)
