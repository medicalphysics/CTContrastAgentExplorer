#pragma once

#include "node.h"

#include <vector>

class NodeManager {
public:
    NodeManager();
    void testNetwork();
    void clearNodes();

private:
    std::size_t m_iterationSteps = 600;
    double m_simulationTime = 2;
    ArterialInput* m_arterialNode = nullptr;
    std::vector<Node*> m_nodes;
};
