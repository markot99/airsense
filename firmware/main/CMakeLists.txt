file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE SOURCES_C "*.c")

idf_component_register(SRCS ${SOURCES} ${SOURCES_C}
                    INCLUDE_DIRS "./../" EMBED_FILES assets/registration.html)

target_link_libraries(${COMPONENT_TARGET} "-L${CMAKE_CURRENT_LIST_DIR}/lib")
target_link_libraries(${COMPONENT_TARGET} algobsec)