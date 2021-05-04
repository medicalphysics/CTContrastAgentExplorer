
#include "node.h"

#include <cmath>

Node::Node(int ID, double volume, std::size_t steps, double total_time)
    : m_ID(ID)
    , m_volume(std::abs(volume))
    , m_totalSteps(steps)
    , m_stepSize(std::abs(total_time) / steps)
{
    m_totalSteps = std::max(m_totalSteps, std::size_t { 1 });
    m_concentration.resize(m_totalSteps);
    m_concentration[0] = 0;
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
    node->setStepSize(m_stepSize);
    node->setTotalSteps(m_totalSteps);
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
    m_organPointer = new Node(m_ID, volume, m_totalSteps, m_stepSize * m_totalSteps);
    this->addInputNode(m_organPointer, PS);
    m_organPointer->addInputNode(this, PS);
}

double Node::getConcentration(std::size_t step)
{
    if (step > m_current_step) {
        advanceTo(step);
    }
    return m_concentration[step];
}

double Node::evaluate(std::size_t step, double C0) const
{

    double total = 0;

    for (const auto& [flow, node] : m_inputs) {
        const auto Cin = node->getConcentration(step);
        total += flow * (Cin - C0) / m_volume;
    }
    /*for (const auto& [flow, node] : m_outputs) {
        const auto Cin = node->getConcentration(step);
        total += flow * (C0 - Cin) / m_volume;
    }*/

    return total;
}

void Node::advanceTo(std::size_t step)
{
    while (m_current_step < step) {
        const double Co = m_concentration[m_current_step];
        const double k1 = evaluate(m_current_step, Co);
        const double k2 = evaluate(m_current_step, Co + m_stepSize * k1 * 0.5);
        const double k3 = evaluate(m_current_step, Co + m_stepSize * k2 * 0.5);
        const double k4 = evaluate(m_current_step, Co + m_stepSize * k3);
        m_current_step++;
        m_concentration[m_current_step] = Co + m_stepSize * (k1 + 2 * k2 + 2 * k3 + k4) / 6;
    }
}

ArterialInput::ArterialInput(int ID, double volume, std::size_t steps, double total_time)
    : Node(ID, volume, steps, total_time)
{
    m_inflow.resize(m_concentration.size());
}

double ArterialInput::evaluate(std::size_t step, double C0) const
{
    const double total = Node::evaluate(step, C0);
    return total + m_inflow[step] / m_volume;
}

RenalClearence::RenalClearence(int ID, double volume, std::size_t steps, double total_time)
    : Node(ID, volume, steps, total_time)
{
}

double RenalClearence::evaluate(std::size_t step, double C0) const
{
    const double total = Node::evaluate(step, C0);
    return total - m_renalClearenceRate * C0 / m_volume;
}
