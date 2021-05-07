

#pragma once

#include "nodemanager.h"
#include "series_type.h"

#include <QMap>
#include <QSet>
#include <QString>
#include <QWidget>

class NodeManagerInterface : public QObject {
    Q_OBJECT
public:
    NodeManagerInterface(QObject* parent = nullptr);

    void setRenalClearenceRate(const double renalRate);
    void setBloodVolume(const double BV);
    void setCardiacOutput(const double cardiacOutput);
    void setInjectionTime(const double time);
    void setInjectionVolume(const double volume);
    void setInjectionConcentration(const double ca);

    const QMap<int, QString> getOrgans() const;

    void requestSeries(QSet<int> IDs);
    void setTimeOptions(double stop = 7, double step = 0.05);

signals:
    void seriesChanged(SeriesPtr series) const;

protected:
    void updateBolus();
    void updateSeries() const;

private:
    NodeManager m_manager;
    double m_injection_time = 1;
    double m_injection_volume = 1;
    double m_injection_concentration = 1;
    double m_time_stop = 10;
    double m_time_step = .05;
    QSet<int> m_requestedSeries;
};
