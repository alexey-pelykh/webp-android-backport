# Small trick to disallow samples building
BUILD_EXECUTABLE := $(CLEAR_VARS)

include $(call all-subdir-makefiles)
