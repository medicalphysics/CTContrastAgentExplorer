#pragma once

#include "node.h"

#include <vector>
#include <string>


class NodeManager {
public:
    NodeManager();
    void testNetwork();
    void clearNodes();
    bool readOrganNetworkFromFile(const std::string& path);


private:
    std::size_t m_iterationSteps = 600;
    double m_simulationTime = 2;
    double m_organPS = 4.0;
    ArterialInput* m_arterialNode = nullptr;
    std::vector<Node*> m_nodes;
};
