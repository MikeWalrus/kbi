link_directories(${GTKMM_LIBRARY_DIRS})
link_directories(${PORTAUDIO2_LIBRARY_DIRS})
include_directories(${GTKMM_INCLUDE_DIRS})
include_directories(${PORTAUDIO2_INCLUDE_DIRS})

include_directories(include)
include_directories(maximilian)
include_directories(maximilian/libs)

add_subdirectory(resource)

add_executable(${PROJECT_NAME} main.cpp KbiWindow.cpp maximilian/maximilian.cpp Player.cpp maximilian/libs/maxiSynths.cpp Ctrl.cpp NotesDrawingArea.cpp resource/kbi.gresourse.c)

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
