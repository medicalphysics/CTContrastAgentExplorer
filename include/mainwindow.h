#pragma once

#include "nodemanagerinterface.h"

#include <QMainWindow>
#include <QThread>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    NodeManagerInterface* m_nodeInterface = nullptr;
    QThread m_workerThread;
};
