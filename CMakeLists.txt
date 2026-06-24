# cmake_minimum_required(VERSION 3.16)

# # Project name and language
# project(ImageEditor VERSION 1.0 LANGUAGES C CXX)

# # Set C++ standard
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # Specify source files
# set(SOURCES
#     driver.cpp
#     Image.cpp
#     Compression.cpp
# )

# # Add executable
# add_executable(${PROJECT_NAME} ${SOURCES})

# # Include raylib headers
# include_directories("C:/raylib/include")

# # Link raylib library (specifically the DLL version)
# # Make sure the .dll is in the same folder as your executable or in PATH
# target_link_libraries(${PROJECT_NAME} "C:/raylib/lib/libraylibdll.a")

# # Optional: define RAYGUI support if using raygui.h
# # add_definitions(-DRAYGUI_SUPPORT)



cmake_minimum_required(VERSION 3.16)

project(ImageEditor VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    main.cpp
    Image.cpp
    Compression.cpp
)

# Header files (for IDE visibility)
set(HEADERS
    Image.h
    Compression.h
    raygui.h
)

# Add executable and include headers
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS}
    app.h
    qoi.cpp
    qoi.h
    networktransfer.cpp
    networktransfer.h)

# Include raylib headers
include_directories("C:/raylib/include")

# Link raylib DLL version
target_link_libraries(${PROJECT_NAME} "C:/raylib/lib/libraylibdll.a")

target_link_libraries(ImageEditor ws2_32)

# Optional: define RAYGUI support if using raygui.h
# add_definitions(-DRAYGUI_SUPPORT)


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "C:/raylib/lib/raylib.dll"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

# Copy all image files to build folder
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/output.ppm"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_checkerboard.ppm"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_gradient.ppm"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_random.ppm"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_solid.ppm"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
