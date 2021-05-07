

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

void NodeManagerInterface::setTimeOptions(double stop, double step)
{
    m_time_stop = stop;
    m_time_step = step;
    updateSeries();
}

void getSeries(SeriesPtr& series, Node* v_node, double time_stop, double time_step)
{

    QVector<Node*> nodes;
    nodes.append(v_node);
    if (v_node->organ()) {
        nodes.append(v_node->organ());
    }

    for (auto node : nodes) {
        const std::string& name = node->name();
        auto qname = QString::fromStdString(name);
        series->names.append(qname);

        QList<QPointF> s;
        double time = 0;
        while (time < time_stop) {
            const auto c = node->getConcentration(time);
            series->xMax = std::max(series->xMax, time);
            series->yMax = std::max(series->yMax, c);
            s.append(QPointF(time, c));
            time += time_step;
        }
        series->data.append(s);
    }

    if (nodes.size() > 1) {
        const std::string& name = v_node->name() + " Total";
        auto qname = QString::fromStdString(name);
        series->names.append(qname);

        QList<QPointF> s;
        double time = 0;
        while (time < time_stop) {
            double c = 0;
            double volume = 0;
            for (auto node : nodes) {
                c += node->getConcentration(time)*node->volume();
                volume += node->volume();
            }
            c /= volume;
            series->xMax = std::max(series->xMax, time);
            series->yMax = std::max(series->yMax, c);
            s.append(QPointF(time, c));
            time += time_step;
        }
        series->data.append(s);
    }
}

void NodeManagerInterface::updateSeries() const
{
    auto series = SeriesPtr(new Series);
    for (const auto id : m_requestedSeries) {
        for (auto node : m_manager.nodes()) {
            if (id == node->id()) {
                getSeries(series, node, m_time_stop, m_time_step);
            }
        }
    }

    emit seriesChanged(series);
}
