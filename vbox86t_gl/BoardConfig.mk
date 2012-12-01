# Use common part from vbox86

include device/androVM/vbox86/BoardConfig.mk
BOARD_EGL_CFG := device/androVM/vbox86t_gl/egl.cfg
TARGET_ARCH_VARIANT := x86
BUILDROID_X86_NOSSE2 := true
BUILD_EMULATOR_OPENGL := true
BUILD_EMULATOR_OPENGL_DRIVER := true
USE_OPENGL_RENDERER := true
