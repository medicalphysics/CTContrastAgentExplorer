#include "mainwindow.h"
#include "plotwidget.h"
#include "settingswidget.h"

#include <QSplitter>
#include <QTimer>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    auto mainWidget = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(mainWidget);

    auto settingsWidget = new SettingsWidget(mainWidget);
    mainWidget->addWidget(settingsWidget);

    m_nodeInterface = new NodeManagerInterface();

    auto plotWidget = new PlotWidget(m_nodeInterface->getOrgans(), mainWidget);
    mainWidget->addWidget(plotWidget);

    settingsWidget->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
    plotWidget->setSizePolicy(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::MinimumExpanding);
    mainWidget->setStretchFactor(0, 1);
    mainWidget->setStretchFactor(1, 1000);

    m_nodeInterface->moveToThread(&m_workerThread);
    connect(&m_workerThread, &QThread::finished, m_nodeInterface, &NodeManagerInterface::deleteLater);

    //connections here
    connect(settingsWidget, &SettingsWidget::bloodVolumeChanged, m_nodeInterface, &NodeManagerInterface::setBloodVolume);
    connect(settingsWidget, &SettingsWidget::cardiacOutputChanged, m_nodeInterface, &NodeManagerInterface::setCardiacOutput);
    connect(settingsWidget, &SettingsWidget::renalClearenceChanged, m_nodeInterface, &NodeManagerInterface::setRenalClearenceRate);
    connect(settingsWidget, &SettingsWidget::contrastInjectionConcentrationChanged, m_nodeInterface, &NodeManagerInterface::setInjectionConcentration);
    connect(settingsWidget, &SettingsWidget::contrastInjectionTimeChanged, m_nodeInterface, &NodeManagerInterface::setInjectionTime);
    connect(settingsWidget, &SettingsWidget::contrastInjectionVolumeChanged, m_nodeInterface, &NodeManagerInterface::setInjectionVolume);

    qRegisterMetaType<SeriesPtr>();
    connect(m_nodeInterface, &NodeManagerInterface::seriesChanged, plotWidget, &PlotWidget::setSeries);
    connect(plotWidget, &PlotWidget::requestSeries, m_nodeInterface, &NodeManagerInterface::requestSeries);
    connect(plotWidget, &PlotWidget::plotTimeChanged, m_nodeInterface, &NodeManagerInterface::setTotalTime);
    connect(plotWidget, &PlotWidget::modeHUchanged, m_nodeInterface, &NodeManagerInterface::setUseHUEnhancement);
    connect(plotWidget, &PlotWidget::kVpChanged, m_nodeInterface, &NodeManagerInterface::setKVp);

    m_workerThread.start();

    auto sBar = statusBar();
    sBar->showMessage(tr("Created by Erlend Andersen for SSHF"));

    //initialize values
    QTimer::singleShot(0, [=]() {

    });
}

MainWindow::~MainWindow()
{
    m_workerThread.exit();
    m_workerThread.wait();
}
