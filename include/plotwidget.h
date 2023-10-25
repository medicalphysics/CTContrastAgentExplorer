#pragma once

#include "series_type.h"

#include <QListView>
#include <QMap>
#include <QSet>
#include <QStandardItem>
#include <QString>
#include <QVector>
#include <QWidget>
#include <QtCharts>

class PlotWidget : public QWidget {
    Q_OBJECT
public:
    PlotWidget(const QMap<int, QString>& organs, QWidget* parent = nullptr);
    void setSeries(SeriesPtr series);
    void setPlotTime(const double time);
    void setAxisShowHU(bool showHU);
    void setkVp(int kvp);
    void savePlot();
    void copyPlotDataClipboard();

    QListView* setupListView();
signals:
    void requestSeries(QSet<int>);
    void plotTimeChanged(double seconds);
    void modeHUchanged(bool showHU);
    void kVpChanged(int kvp);

protected:
    void listItemChanged(QStandardItem* item);

private:
    double m_totalTime = 0;
    int m_kVp = 100;
    QMap<int, QString> m_organs;
    QSet<int> m_checkedOrgans;
    QChart* m_chart = nullptr;
    QChartView* m_chartView = nullptr;
    QValueAxis* m_xAxis = nullptr;
    QValueAxis* m_yAxis = nullptr;
    QString m_htmlTable;
};
