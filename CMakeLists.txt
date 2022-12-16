cmake_minimum_required(VERSION 3.5)

project(CTCAExplorer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools Charts REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools Charts REQUIRED)

set(TS_FILES translations/CTCAExplorer_en_US.ts)
set(RS_FILES resources/resources.qrc)

set(SOLVER_SOURCES
    src/node.cpp
    src/nodemanager.cpp
    )
set(SOLVER_HEADERS
    include/node.h
    include/nodemanager.h
    )

set(PROJECT_SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/settingswidget.cpp
        src/plotwidget.cpp
        src/nodemanagerinterface.cpp
        ${SOLVER_SOURCES}
        ${TS_FILES}
        ${RS_FILES}
)
set(PROJECT_HEADERS
    include/mainwindow.h
    include/settingswidget.h
    include/plotwidget.h
    include/nodemanagerinterface.h
    include/series_type.h
    ${SOLVER_HEADERS}
    )



add_library(Solver ${SOLVER_HEADERS} ${SOLVER_SOURCES})
target_include_directories(Solver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CTCAExplorer WIN32
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
    )
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    add_executable(CTCAExplorer WIN32
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
    )
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(CTCAExplorer PRIVATE Solver Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts)
target_include_directories(CTCAExplorer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(tests)


if (WIN32)
    get_target_property(QT_QMAKE_EXECUTABLE Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
    get_filename_component(QT_WINDEPLOYQT_EXECUTABLE ${QT_QMAKE_EXECUTABLE} PATH)
    set(QT_WINDEPLOYQT_EXECUTABLE "${QT_WINDEPLOYQT_EXECUTABLE}/windeployqt.exe")
    add_custom_command(
        TARGET CTCAExplorer POST_BUILD
        COMMAND ${QT_WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE_DIR:CTCAExplorer>
    )

    SET(WIN_DEPLOY_QT "${CMAKE_CURRENT_BINARY_DIR}/deploy/")
    add_custom_command(
        TARGET CTCAExplorer POST_BUILD
        COMMAND ${QT_WINDEPLOYQT_EXECUTABLE} --dir "${WIN_DEPLOY_QT}" "$<TARGET_FILE_DIR:CTCAExplorer>/$<TARGET_FILE_NAME:CTCAExplorer>"
    )


endif(WIN32)


# Install targets
include(InstallRequiredSystemLibraries)
install(TARGETS CTCAExplorer
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include    
    PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION bin
    )
if(WIN32)
    install(DIRECTORY "${WIN_DEPLOY_QT}" DESTINATION bin)
endif()
# Packaging
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CTCAExplorer")
set(CPACK_NSIS_MODIFY_PATH "ON")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CMAKE_PROJECT_NAME}-${PROJECT_VERSION_MAJOR}-${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VENDOR "Erlend Andersen")
include(CPack)
