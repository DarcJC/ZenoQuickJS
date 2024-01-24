
add_library(quickjs INTERFACE)
set_target_properties(quickjs PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include"
)

if(WIN32)
    file(GLOB DLL_FILES "${CMAKE_CURRENT_LIST_DIR}/library/windows-x86_64/*.dll")
    target_link_libraries(quickjs INTERFACE "${CMAKE_CURRENT_LIST_DIR}/library/windows-x86_64/libquickjs-dyn.a")
else ()
    MESSAGE(FATAL_ERROR "Unsupported platform")
endif()

file(COPY ${DLL_FILES} DESTINATION ${CMAKE_BINARY_DIR})
