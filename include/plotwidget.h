#pragma once

#include <QWidget>
#include <QtCharts>

class PlotWidget : public QWidget {
    Q_OBJECT
public:
    PlotWidget(QWidget* parent = nullptr);

private:
    QtCharts::QChart* m_chart = nullptr;
    QtCharts::QChartView* m_chartView = nullptr;
    QtCharts::QSplineSeries* m_series = nullptr;
    QtCharts::QDateTimeAxis* m_xAxis = nullptr;
    QtCharts::QValueAxis* m_yAxis = nullptr;
};
