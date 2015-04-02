APP_ABI := all
APP_CPPFLAGS := -fno-rtti -fno-exceptions
APP_PLATFORM := android-8

ifndef WEBP_BACKPORT_DEBUG_NATIVE
# Force release compilation in release optimizations, even if application is debuggable by manifest
APP_OPTIM := release
endif