cmake_minimum_required(VERSION 3.25)

project(MyCadStarter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(APP_TARGET MyCadStarter)

include_directories(${PROJECT_SOURCE_DIR})

# ===== Qt =====
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Svg Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Svg Core)

# ===== OpenCascade path =====
set(THIRD_PARTY_PATH "C:/Users/Alex/Downloads/occt_ver7.8.0/occt" CACHE PATH "Path to OpenCascade")

include_directories(${THIRD_PARTY_PATH}/inc)

# Windows / Linux lib paths
if (WIN32)
    link_directories(${THIRD_PARTY_PATH}/win64/gcc/libd)
endif()

if (UNIX)
    link_directories(${THIRD_PARTY_PATH}/lin64/gcc/libd)
endif()

# ===== OpenCascade libs =====
set(OpenCASCADE_LIBS
    TKernel
    TKMath
    TKG2d
    TKG3d
    TKBRep
    TKGeomBase
    TKGeomAlgo
    TKTopAlgo
    TKPrim
    TKBO
    TKBool
    TKFillet
    TKService
    TKV3d
    TKOpenGl
    TKMesh
    TKXSBase
    TKDESTEP
    TKDEIGES
)

# ===== GoogleTest =====
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ===== Sources =====
set(SOURCES
    app/main.cpp
    app/MainWindow.h
    app/MainWindow.cpp

    core/project/SceneNode.h
    core/project/ProjectModel.h
    core/project/ProjectModel.cpp

    core/commands/CommandManager.h
    core/commands/CommandManager.cpp

    core/utils/AppStrings.h
    core/utils/AppStrings.cpp

    geometry/occ/OccPrimitiveBuilder.h
    geometry/occ/OccPrimitiveBuilder.cpp
    geometry/occ/OccImportExport.h
    geometry/occ/OccImportExport.cpp

    ui/viewport/ViewportWidget.h
    ui/viewport/ViewportWidget.cpp

    ui/docks/SceneTreeDock.h
    ui/docks/SceneTreeDock.cpp
    ui/docks/PropertyDock.h
    ui/docks/PropertyDock.cpp

    resources/resources.qrc
)

source_group("App" FILES
    app/main.cpp
    app/MainWindow.h
    app/MainWindow.cpp
)

source_group("Core" FILES
    core/project/SceneNode.h
    core/project/ProjectModel.h
    core/project/ProjectModel.cpp
    core/commands/CommandManager.h
    core/commands/CommandManager.cpp
    core/utils/AppStrings.h
    core/utils/AppStrings.cpp
)

source_group("Geometry" FILES
    geometry/occ/OccPrimitiveBuilder.h
    geometry/occ/OccPrimitiveBuilder.cpp
    geometry/occ/OccImportExport.h
    geometry/occ/OccImportExport.cpp
)

source_group("Viewport" FILES
    ui/viewport/ViewportWidget.h
    ui/viewport/ViewportWidget.cpp
)

source_group("Docks" FILES
    ui/docks/SceneTreeDock.h
    ui/docks/SceneTreeDock.cpp
    ui/docks/PropertyDock.h
    ui/docks/PropertyDock.cpp
)

source_group("Resources" FILES
    resources/resources.qrc
)

add_executable(${APP_TARGET} ${SOURCES})

target_link_libraries(${APP_TARGET}
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Svg
    ${OpenCASCADE_LIBS}
)

set_target_properties(${APP_TARGET} PROPERTIES
    WIN32_EXECUTABLE TRUE
)

# ===== Tests =====
enable_testing()
include(GoogleTest)

add_executable(MyCadStarterTests
    tests/test_appstrings.cpp
    tests/test_commandmanager.cpp
    core/utils/AppStrings.cpp
    core/commands/CommandManager.cpp
)

target_link_libraries(MyCadStarterTests
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    GTest::gtest_main
)

gtest_discover_tests(MyCadStarterTests)

# ===== Documentation =====
find_package(Doxygen QUIET)
if (DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in
                   ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
endif()
