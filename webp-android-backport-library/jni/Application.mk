APP_ABI := all
APP_CPPFLAGS := -fno-rtti -fno-exceptions

ifdef WEBP_BACKPORT_X86_ONLY
APP_ABI := x86
else
ifdef WEBP_BACKPORT_ARM_ONLY
APP_ABI := armeabi armeabi-v7a
endif
ifdef WEBP_BACKPORT_ARMv5_ONLY
APP_ABI := armeabi
endif
ifdef WEBP_BACKPORT_ARMv7a_ONLY
APP_ABI := armeabi-v7a
endif
endif

ifndef WEBP_BACKPORT_DEBUG_NATIVE
# Force release compilation in release optimizations, even if application is debuggable by manifest
APP_OPTIM := release
endif