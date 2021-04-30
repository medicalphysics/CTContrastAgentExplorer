#include "nodemanager.h"

#include <iostream>

NodeManager::NodeManager()
{
    m_nodes.clear();
}

void NodeManager::testNetwork()
{

    m_arterialNode = new ArterialInput(0, 700.0, m_iterationSteps, m_simulationTime);
    auto aif = [](const double time) -> double {
        if (time < 0.1)
            return 5;
        return 0;
    };
    m_arterialNode->setInputFunction(aif);

    auto blood1 = new Node(1, 800.0);
    m_arterialNode->addOutputNode(blood1, 1500.0);
    auto renal = new RenalClearence(3, 54.0);
    renal->addOrgan(89.0, 4*60.0);
    auto renal_organ = renal->organ();
    blood1->addOutputNode(renal, 1500.0);

    for (std::size_t i = 0; i < m_iterationSteps; ++i) {
        const auto a = m_arterialNode->getConcentration(i);
        const auto c = blood1->getConcentration(i);
        const auto r = renal_organ->getConcentration(i);
        const auto t = blood1->stepSize() * i;
        std::cout << t << ", " << a << ", " << c << "," << r << std::endl;
    }
}

void NodeManager::clearNodes()
{
    for (auto node : m_nodes) {
        delete node;
    }
    m_nodes.clear();
    if (m_arterialNode) {
        delete m_arterialNode;
        m_arterialNode = nullptr;
    }
}
