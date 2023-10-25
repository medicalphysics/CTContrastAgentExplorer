cmake_minimum_required(VERSION 3.5)

project(CTCAExplorer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



find_package(Qt6 COMPONENTS Widgets Gui LinguistTools Charts REQUIRED)
qt_standard_project_setup()

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



qt_add_library(Solver STATIC ${SOLVER_HEADERS} ${SOLVER_SOURCES})
target_include_directories(Solver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

qt_add_executable(CTCAExplorer WIN32
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
)

set_target_properties(CTCAExplorer PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

target_link_libraries(CTCAExplorer PRIVATE Solver Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts)
target_include_directories(CTCAExplorer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)


qt_add_resources(CTCAExplorer "data"
    BASE "resources/"
    FILES 
    resources/data/organs.csv 
    resources/icons/logo.png 
    resources/icons/splash.png
)

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
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CT contrast agent model")
set(CPACK_NSIS_MODIFY_PATH "ON")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CMAKE_PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "Erlend Andersen")
if(WIN32)
    set(CPACK_SOURCE_GENERATOR "TGZ")
    set(CPACK_NSIS_MODIFY_PATH "ON")
    set(CPACK_NSIS_CREATE_ICONS_EXTRA
    "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\CTCAExplorer.lnk' '$INSTDIR\\\\bin\\\\CTCAExplorer.exe'"
    )
    set(CPACK_NSIS_DELETE_ICONS_EXTRA
    "Delete '$SMPROGRAMS\\\\$START_MENU\\\\CTCAExplorer.lnk'"
)
endif()
include(CPack)




