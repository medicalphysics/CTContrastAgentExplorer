
#include "plotwidget.h"

#include <QVBoxLayout>
PlotWidget::PlotWidget(QWidget* parent)
    : QWidget(parent)
{
    auto layout = new QVBoxLayout;
    setLayout(layout);

    m_chart = new QtCharts::QChart();

    m_series = new QtCharts::QSplineSeries();
    m_series->append(16000, 1);
    m_series->append(19000, 2);
    m_series->setName("test");

    m_chart->addSeries(m_series);

    m_xAxis = new QtCharts::QDateTimeAxis();
    m_xAxis->setFormat("mm:ss.z");
    m_xAxis->setTitleText(tr("Time [m:s]"));
    m_yAxis = new QtCharts::QValueAxis();
    m_yAxis->setTitleText(tr("Concentration"));
    m_chart->addAxis(m_yAxis, Qt::AlignLeft);
    m_chart->addAxis(m_xAxis, Qt::AlignBottom);
    m_series->attachAxis(m_xAxis);
    m_series->attachAxis(m_yAxis);

    m_chartView = new QtCharts::QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    layout->addWidget(m_chartView);
}
