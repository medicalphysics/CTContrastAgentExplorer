#include "mainwindow.h"
#include "plotwidget.h"
#include "settingswidget.h"

#include <QSplitter>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    auto mainWidget = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(mainWidget);

    auto settingsWidget = new SettingsWidget(mainWidget);
    mainWidget->addWidget(settingsWidget);

    auto plotWidget = new PlotWidget(mainWidget);
    mainWidget->addWidget(plotWidget);

    m_nodeManager.testNetwork();



}

MainWindow::~MainWindow()
{
}
