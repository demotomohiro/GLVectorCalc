set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets)

qt5_wrap_ui(ui_outputs mainwindow.ui)
add_executable(${PROJECT_NAME} main.cpp mainwindow.cpp calcglwidget.cpp ${ui_outputs})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
