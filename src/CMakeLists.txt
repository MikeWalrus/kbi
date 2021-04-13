link_directories(${GTKMM_LIBRARY_DIRS})
include_directories(${GTKMM_INCLUDE_DIRS})

include_directories(include)
include_directories(maximilian)
include_directories(maximilian/libs)

add_compile_definitions(LITTLE_ENDIAN)

add_compile_definitions(__LINUX_ALSA__)

add_subdirectory(resource)

add_executable(${PROJECT_NAME} main.cpp KbiWindow.cpp maximilian/maximilian.cpp 1.TestTone.cpp Player.cpp include/Player.h include/utilities.h maximilian/libs/maxiSynths.cpp)

set_source_files_properties(
        resource/${GRESOURCE_C}
        PROPERTIES GENERATED TRUE
)

add_dependencies(${PROJECT_NAME} kbi-resource)

target_link_libraries(${PROJECT_NAME} ${GTKMM_LIBRARIES})
target_link_libraries(${PROJECT_NAME} portaudio)
target_link_libraries(${PROJECT_NAME} pthread)
target_link_libraries(${PROJECT_NAME} asound)
