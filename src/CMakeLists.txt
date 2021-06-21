link_directories(${GTKMM_LIBRARY_DIRS})
link_directories(${PORTAUDIO2_LIBRARY_DIRS})
include_directories(${GTKMM_INCLUDE_DIRS})
include_directories(${PORTAUDIO2_INCLUDE_DIRS})

include_directories(include)
include_directories(maximilian)
include_directories(maximilian/libs)

find_program(GLIB_COMPILE_RESOURCES NAMES glib-compile-resources REQUIRED)

set(GRESOURCE_C ${CMAKE_CURRENT_SOURCE_DIR}/resource/kbi.gresourse.c)
set(GRESOURCE_XML ${CMAKE_CURRENT_SOURCE_DIR}/resource/kbi.gresourse.xml)

add_custom_command(
        OUTPUT ${GRESOURCE_C}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resource
        COMMAND ${GLIB_COMPILE_RESOURCES}
        ARGS
        --target=${GRESOURCE_C}
        --generate-source
        ${GRESOURCE_XML}
        VERBATIM
        MAIN_DEPENDENCY ${GRESOURCE_XML}
        DEPENDS
        resource/window.ui resource/guitar_e3.wav resource/violin_a4.wav resource/saxophone_c3.wav resource/instruments.conf
)

add_custom_target(
        kbi-resource
        DEPENDS ${GRESOURCE_C}
)

add_executable(${PROJECT_NAME} main.cpp KbiWindow.cpp maximilian/maximilian.cpp Player.cpp Ctrl.cpp NotesDrawingArea.cpp ${GRESOURCE_C})

set_source_files_properties(
        resource/${GRESOURCE_C}
        PROPERTIES GENERATED TRUE
)

add_dependencies(${PROJECT_NAME} kbi-resource)

target_link_libraries(${PROJECT_NAME} ${GTKMM_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${PORTAUDIO2_LIBRARIES})
target_link_libraries(${PROJECT_NAME} pthread)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    target_link_libraries(${PROJECT_NAME} asound)
endif ()

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
