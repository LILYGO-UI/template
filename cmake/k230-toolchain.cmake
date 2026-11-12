if(NOT LilyGoUI_DIR)
    set(LilyGoUI_DIR "${CMAKE_CURRENT_LIST_DIR}/../third_party/cm0-appkit")
endif()
if(NOT EXISTS "${LilyGoUI_DIR}/cmake/k230-toolchain.cmake")
    message(FATAL_ERROR
        "The selected AppKit has no K230 profile. Update the pinned AppKit or "
        "set LilyGoUI_DIR to a source SDK containing cmake/k230-toolchain.cmake")
endif()
list(APPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES LilyGoUI_DIR)
include("${LilyGoUI_DIR}/cmake/k230-toolchain.cmake")
