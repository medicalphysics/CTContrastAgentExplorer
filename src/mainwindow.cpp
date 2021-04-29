#include "mainwindow.h"
#include "settingswidget.h"

#include <QSplitter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    auto mainWidget = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(mainWidget);

    auto settingsWidget = new SettingsWidget(mainWidget);
    mainWidget->addWidget(settingsWidget);

}

MainWindow::~MainWindow()
{
}

