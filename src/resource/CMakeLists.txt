# Step 1:
find_program(GLIB_COMPILE_RESOURCES NAMES glib-compile-resources REQUIRED)

set(GRESOURCE_C kbi.gresourse.c)
set(GRESOURCE_XML kbi.gresourse.xml)

# Step 2:
add_custom_command(
        OUTPUT ${GRESOURCE_C}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${GLIB_COMPILE_RESOURCES}
        ARGS
        --target=${GRESOURCE_C}
        --generate-source
        ${GRESOURCE_XML}
        VERBATIM
        MAIN_DEPENDENCY ${GRESOURCE_XML}
        DEPENDS
        window.ui guitar_e3.wav
)

# Step 3:
add_custom_target(
        kbi-resource
        DEPENDS ${GRESOURCE_C}
)