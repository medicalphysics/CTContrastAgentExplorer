

#include "nodemanagerinterface.h"

#include <QFile>
#include <QTextStream>

#include <string>
#include <vector>

NodeManagerInterface::NodeManagerInterface(QObject* parent)
    : QObject(parent)
{

    QFile file(":/data/organs.csv");

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        std::vector<std::string> lines;
        while (!in.atEnd()) {
            auto line = in.readLine();
            auto std_line = line.toStdString();
            lines.push_back(std_line);
        }
        m_manager = NodeManager(lines);
    }
    m_manager.setTotalTime(m_totalTime);
}

void NodeManagerInterface::setRenalClearenceRate(const double renalRate)
{
    m_manager.setRenalClearenceRate(renalRate);
    updateSeries();
}
void NodeManagerInterface::setBloodVolume(const double BV)
{
    m_manager.setBloodVolume(BV);
    updateSeries();
}
void NodeManagerInterface::setCardiacOutput(const double cardiacOutput)
{
    m_manager.setCardiacOutput(cardiacOutput);
    updateSeries();
}

void NodeManagerInterface::setInjectionTime(const double time)
{
    m_injection_time = time;
    updateBolus();
}
void NodeManagerInterface::setInjectionVolume(const double volume)
{
    m_injection_volume = volume;
    updateBolus();
}
void NodeManagerInterface::setInjectionConcentration(const double ca)
{
    m_injection_concentration = ca;
    updateBolus();
}

double NodeManagerInterface::cToHu(int kvP)
{
    std::map<int, double> map;
    map[70] = 57.60;
    map[80] = 53.17;
    map[90] = 49.37;
    map[100] = 46.32;
    map[110] = 43.84;
    map[120] = 41.81;
    map[130] = 40.64;
    map[140] = 38.64;
    map[150] = 37.38;

    if (map.count(kvP) > 0) {
        return map[kvP];
    }
    return 1.0;
}

void NodeManagerInterface::updateBolus()
{
    m_manager.setBolusInjection(m_injection_time, m_injection_volume, m_injection_concentration);
    updateSeries();
}

const QMap<int, QString> NodeManagerInterface::getOrgans() const
{
    QMap<int, QString> map;
    for (auto n : m_manager.nodes()) {
        const auto id = n->id();
        const auto name = QString::fromStdString(n->name());
        map[id] = name;
    }
    return map;
}

void NodeManagerInterface::requestSeries(QSet<int> ids)
{
    m_requestedSeries = ids;
    updateSeries();
}

void NodeManagerInterface::setTotalTime(double time)
{
    m_totalTime = time;
    m_time_step = m_totalTime / 512;
    m_manager.setTotalTime(m_totalTime);
    updateSeries();
}

void NodeManagerInterface::setUseHUEnhancement(bool useHU)
{
    m_calculateHU = useHU;
    updateSeries();
}

void NodeManagerInterface::setKVp(int kvp)
{
    m_kVp = kvp;
    updateSeries();
}

void getSeries(SeriesPtr& series, Node* v_node, double time_stop, double time_step, double scaling = 1.0)
{

    QVector<Node*> nodes;
    nodes.append(v_node);
    if (v_node->organ()) {
        nodes.append(v_node->organ());
    }

    for (auto node : nodes) {
        const std::string& name = node->name(true);
        auto qname = QString::fromStdString(name);
        series->names.append(qname);

        QList<QPointF> s;
        QPointF peak(0.0, 0.0);

        double time = 0;
        while (time < time_stop) {
            const auto time_s = time * 60;
            const auto c = node->getConcentration(time) * scaling;
            series->xMax = std::max(series->xMax, time_s);
            series->yMax = std::max(series->yMax, c);
            if (c > peak.y()) {
                peak.setY(c);
                peak.setX(qRound(time_s));
            }
            s.append(QPointF(time_s, c));
            time += time_step;
        }
        series->data.append(s);
        series->peaks.append(peak);
    }

    if (nodes.size() > 1) {
        const std::string& name = v_node->name() + " Total";
        auto qname = QString::fromStdString(name);
        series->names.append(qname);

        QList<QPointF> s;
        QPointF peak(0.0, 0.0);

        double time = 0;
        while (time < time_stop) {
            const auto time_s = time * 60;
            double c = 0;
            double volume = 0;
            for (auto node : nodes) {
                c += node->getConcentration(time) * node->volume() * scaling;
                volume += node->volume();
            }
            c /= volume;
            series->xMax = std::max(series->xMax, time_s);
            series->yMax = std::max(series->yMax, c);
            s.append(QPointF(time_s, c));

            if (c > peak.y()) {
                peak.setX(qRound(time_s));
                peak.setY(c);
            }

            time += time_step;
        }
        series->peaks.append(peak);
        series->data.append(s);
    }
}

void NodeManagerInterface::updateSeries() const
{

    const auto converter = m_calculateHU ? cToHu(m_kVp) : 1.0;

    auto series = SeriesPtr(new Series);
    for (const auto id : m_requestedSeries) {
        for (auto node : m_manager.nodes()) {
            if (id == node->id()) {
                getSeries(series, node, m_totalTime, m_time_step, converter);
            }
        }
    }

    emit seriesChanged(series);
}
