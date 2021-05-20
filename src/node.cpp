
#include "node.h"

#include <cmath>

Node::Node(int ID, const std::string& name, const double volume, bool isVascular)
    : m_ID(ID)
    , m_volume(std::abs(volume))
    , m_name(name)
    , m_isVascular(isVascular)
{
    m_timeStep = 1.0 / m_concentration.size();
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

void Node::addInputNode(Node* node, double flow, double delay)
{
    node->setTimeStep(m_timeStep);
    Input in = { flow, delay, node };
    m_inputs.push_back(in);
}

void Node::deleteOrganNode()
{
    if (m_organPointer) {
        for (auto it = m_inputs.begin(); it != m_inputs.end();) {
            if (it->node == m_organPointer) {
                it = m_inputs.erase(it);
                delete m_organPointer;
                m_organPointer = nullptr;
                return;
            } else {
                it++;
            }
        }
    }
}

void Node::addOrgan(double volume, double PS)
{
    deleteOrganNode();
    m_organPointer = new Node(m_ID, m_name + " Extra-Vascular", volume, false);
    m_organPointer->setTimeStep(m_timeStep);
    m_ps = PS;
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
    if (m_current_step != 0) {
        m_concentration[0] = 0;
        m_current_step = 0;
        if (m_organPointer) {
            m_organPointer->resetNode();
        }
        for (const auto& in : m_inputs) {
            in.node->resetNode();
        }
    }
}

void Node::setTimeStep(const double timestep)
{
    resetNode();
    m_timeStep = std::abs(timestep);
    if (m_organPointer) {
        m_organPointer->setTimeStep(timestep);
    }
}

void Node::setOrganPS(const double PS)
{
    m_ps = PS;
    if (m_organPointer) {
        resetNode();
        m_organPointer->clearInputs();
        m_organPointer->addInputNode(this, m_ps);
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

void Node::clearInputs()
{
    m_inputs.clear();
    resetNode();
}

template <std::floating_point T>
inline T interp(const T x, const T x0, const T x1, const T y0, const T y1)
{
    return (x - x0) * (y1 - y0) / (x1 - x0) + y0;
}

double Node::getConcentration(const double time)
{
    const std::size_t rstep = static_cast<std::size_t>(std::max(time, 0.0) / m_timeStep);
    const auto y0 = getConcentration(rstep);
    const auto y1 = getConcentration(rstep + 1);
    return interp(time, m_timeStep * rstep, m_timeStep * (rstep + 1), y0, y1);
}
double Node::getConcentration(const std::size_t step)
{
    const auto valid_step = std::min(step, m_concentration.size() - 1);

    if (valid_step > m_current_step) {
        advanceTo(valid_step);
    }
    
    return m_concentration[valid_step];
}

double Node::evaluate(const std::size_t step, const double C0) const
{
    double total = 0;
    for (const auto& input : m_inputs) {
        const auto Cin = input.node->getConcentration(step);
        total += input.flow * m_flowScaling * (Cin-C0) / (volume());
    }
    if (m_organPointer) {
        const auto Cin = m_organPointer->getConcentration(step);
        total += m_ps * m_flowScaling * (Cin - C0) / volume();
    }

    return total;
}

void Node::advanceTo(const std::size_t step)
{
    const auto minStep = std::min(m_concentration.size() - 1, step);
    while (m_current_step < minStep) {
        const double Co = getConcentration(m_current_step);
        const double k1 = evaluate(m_current_step, Co);
        const double k2 = evaluate(m_current_step, Co + m_timeStep * k1 * 0.5);
        const double k3 = evaluate(m_current_step, Co + m_timeStep * k2 * 0.5);
        const double k4 = evaluate(m_current_step, Co + m_timeStep * k3);

        m_current_step++;

        m_concentration[m_current_step] = Co + m_timeStep * (k1 + 2 * k2 + 2 * k3 + k4) / 6;
    }
}

ArterialInput::ArterialInput(int ID, const std::string& name, double volume)
    : Node(ID, name, volume, true)
{
}

double ArterialInput::evaluate(const std::size_t step, const double C0) const
{
    const double total = Node::evaluate(step, C0);
    const auto time = step * timeStep();

    const auto input = m_injTime > time ? m_caStrenght * m_injVolume / m_injTime : 0;

    return total + input / volume();
}

RenalClearence::RenalClearence(int ID, const std::string& name, double volume)
    : Node(ID, name, volume, true)
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
