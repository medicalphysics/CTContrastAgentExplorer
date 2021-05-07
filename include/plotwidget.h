#pragma once

#include "series_type.h"

#include <QMap>
#include <QSet>
#include <QStandardItem>
#include <QString>
#include <QVector>
#include <QWidget>
#include <QtCharts>

Q_DECLARE_METATYPE(SeriesPtr)

class PlotWidget : public QWidget {
    Q_OBJECT
public:
    PlotWidget(const QMap<int, QString>& organs, QWidget* parent = nullptr);
    void setSeries(SeriesPtr series);
signals:
    void requestSeries(QSet<int>);

protected:
    void setAvailableOrgans(const QMap<int, QString>&);
    void setupListView();
    void listItemChanged(QStandardItem* item);

private:
    QMap<int, QString> m_organs;
    QSet<int> m_checkedOrgans;
    QtCharts::QChart* m_chart = nullptr;
    QtCharts::QChartView* m_chartView = nullptr;
    QtCharts::QValueAxis* m_xAxis = nullptr;
    QtCharts::QValueAxis* m_yAxis = nullptr;
};
