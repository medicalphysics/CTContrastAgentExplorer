
#include "node.h"

#include <cmath>

Node::Node(int ID, const std::string& name, const double volume, const double time)
    : m_ID(ID)
    , m_volume(std::abs(volume))
    , m_totalTime(time)
    , m_name(name)
{
    m_concentration.fill(0.0);
}

Node::~Node()
{
    deleteOrganNode();
}

void Node::changeID(int ID)
{
    m_ID = ID;
    if (m_organPointer) {
        m_organPointer->changeID(ID);
    }
}

void Node::addInputNode(Node* node, double flow)
{
    node->setTotalTime(m_totalTime);
    m_inputs.push_back(std::make_pair(flow, node));
}

void Node::deleteOrganNode()
{
    if (m_organPointer) {
        for (auto it = m_inputs.begin(); it != m_inputs.end();) {
            if (it->second == m_organPointer) {
                it = m_inputs.erase(it);
            } else {
                it++;
            }
        }
        delete m_organPointer;
        m_organPointer = nullptr;
    }
}

void Node::addOrgan(double volume, double PS)
{
    deleteOrganNode();
    m_organPointer = new Node(m_ID, m_name + " Extra-Vascular", volume, m_totalTime);
    this->addInputNode(m_organPointer, PS);
    m_organPointer->addInputNode(this, PS);
}

std::string Node::name(bool fullName) const
{
    if (m_organPointer && fullName)
        return m_name + " Vascular";
    return m_name;
}

void Node::resetNode()
{
    m_current_step = 0;
    if (m_organPointer) {
        m_organPointer->resetNode();
    }
}

void Node::setTotalTime(const double time)
{
    resetNode();
    m_totalTime = std::abs(time);
    if (m_organPointer) {
        m_organPointer->setTotalTime(time);
    }
}

void Node::setOrganPS(const double PS)
{
    if (m_organPointer) {
        resetNode();
        const auto organ_volume = m_organPointer->volume();
        deleteOrganNode();
        addOrgan(organ_volume, PS);
    }
}

void Node::setCardiacOutputScaling(const double scaling)
{
    resetNode();
    m_flowScaling = scaling;
    if (m_organPointer) {
        m_organPointer->setCardiacOutputScaling(scaling);
    }
}
void Node::setBloodVolumeScaling(const double scaling)
{
    resetNode();
    m_volumeScaling = scaling;
    if (m_organPointer) {
        m_organPointer->setBloodVolumeScaling(scaling);
    }
}

template <std::floating_point T>
inline T interp(const T x, const T x0, const T x1, const T y0, const T y1)
{
    return (x - x0) * (y1 - y0) / (x1 - x0) + y0;
}

double Node::getConcentration(const double time)
{
    const std::size_t rstep = static_cast<std::size_t>(time * m_concentration.size() / m_totalTime);
    const auto sSize = stepSize();
    const auto y0 = getConcentration(rstep);
    const auto y1 = getConcentration(rstep + 1);
    return interp(time, sSize * rstep, sSize * (rstep + 1), y0, y1);
}
double Node::getConcentration(const std::size_t step)
{
    if (step > m_current_step) {

        advanceTo(step);
    }
    return step < m_concentration.size() ? m_concentration[step] : 0.0;
}

double Node::evaluate(const std::size_t step, const double C0) const
{
    double total = 0;
    for (const auto& [flow, node] : m_inputs) {
        const auto Cin = node->getConcentration(step);
        total += flow * m_flowScaling * (Cin - C0) / (m_volume * m_volumeScaling);
    }

    return total;
}

void Node::advanceTo(const std::size_t step)
{

    const auto sSize = stepSize();
    const auto minStep = std::min(m_concentration.size() - 2, step);
    while (m_current_step <= minStep) {
        const double Co = m_concentration[m_current_step];
        const double k1 = evaluate(m_current_step, Co);
        const double k2 = evaluate(m_current_step, Co + sSize * k1 * 0.5);
        const double k3 = evaluate(m_current_step, Co + sSize * k2 * 0.5);
        const double k4 = evaluate(m_current_step, Co + sSize * k3);

        m_current_step++;

        m_concentration[m_current_step] = Co + sSize * (k1 + 2 * k2 + 2 * k3 + k4) / 6;
    }
}

ArterialInput::ArterialInput(int ID, const std::string& name, double volume, double stepSizeMax)
    : Node(ID, name, volume, stepSizeMax)
{
}

double ArterialInput::evaluate(const std::size_t step, const double C0) const
{
    const double total = Node::evaluate(step, C0);
    const auto time = step * stepSize();

    const auto input = m_injTime > time ? m_caStrenght * m_injVolume / m_injTime : 0;

    return total + input / volume();
}

RenalClearence::RenalClearence(int ID, const std::string& name, double volume, double stepSizeMax)
    : Node(ID, name, volume, stepSizeMax)
{
}

double RenalClearence::evaluate(const std::size_t step, const double C0) const
{
    const double total = Node::evaluate(step, C0);
    return total - m_renalClearenceRate * C0 / volume();
}

void RenalClearence::setRenalClearenceRate(const double rate)
{
    resetNode();
    m_renalClearenceRate = rate;
}
