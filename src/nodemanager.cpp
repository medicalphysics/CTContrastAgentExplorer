#include "nodemanager.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <map>

NodeManager::NodeManager()
{
    m_nodes.clear();
}

void NodeManager::testNetwork()
{

    auto success = readOrganNetworkFromFile("./resources/data/organs.csv");
    if (m_arterialNode) {
        const auto injection_time = 20.0 / 60.0;
        const auto injection_volume = 125.0;
        auto f = [=](double time, double step) -> double {
            const auto volume = 300.0;
            const auto rate = 4.0 * 60;
            const auto end = volume / rate;
            return time <= end ? volume / end * step : 0.0;

        };
        m_arterialNode->setInputFunction(f);

        for (std::size_t i = 0; i < m_arterialNode->totalSteps(); ++i) {
            std::cout << i << ", " << m_arterialNode->getConcentration(i) << std::endl;
        }
    }
}

void NodeManager::clearNodes()
{
    for (auto node : m_nodes) {
        delete node;
    }
    m_nodes.clear();
}

// trim from start (in place)
static inline void ltrim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(),
        s.end());
}

// trim from both ends (in place)
static inline void trim(std::string& s)
{
    ltrim(s);
    rtrim(s);
}

std::vector<std::string> splitString(const std::string& line, const std::string& deliminator = ",")
{
    std::vector<std::string> res;

    std::size_t first = 0;
    std::size_t last = 0;
    do {
        last = line.find(deliminator, first);

        auto substring = line.substr(first, last - first);
        trim(substring);
        res.push_back(substring);

        first = last + deliminator.length();
    } while (last != line.npos);
    return res;
}

bool NodeManager::readOrganNetworkFromFile(const std::string& path)
{
    std::ifstream file(path);
    auto success = file.good();
    if (!success) {
        return success;
    }

    struct Connection {
        int ID = 0;
        int inputID = 0;
        double flow = 0.0;
    };

    std::vector<Connection> connections;

    std::string line;
    while (std::getline(file, line)) {
        const auto strings = splitString(line);
        if (strings.size() > 4) {
            if (strings.at(0).at(0) != '#') {
                const auto& name = strings.at(0);
                int ID = -1;
                bool parsed_organ = true;
                try {
                    ID = std::stoi(strings.at(1));
                    const auto IVvolume = std::stod(strings.at(2));
                    const auto ECvolume = std::stod(strings.at(3));
                    if (ID == 0) {
                        m_arterialNode = new ArterialInput(ID, IVvolume, m_iterationSteps, m_simulationTime);
                        m_nodes.push_back(m_arterialNode);
                    } else if (ID == 255) {
                        auto node = new RenalClearence(ID, IVvolume, m_iterationSteps, m_simulationTime);
                        if (ECvolume > 0.0) {
                            node->addOrgan(ECvolume, m_organPS);
                        }
                        m_nodes.push_back(node);
                    } else {
                        auto node = new Node(ID, IVvolume, m_iterationSteps, m_simulationTime);
                        if (ECvolume > 0.0) {
                            node->addOrgan(ECvolume, m_organPS);
                        }
                        m_nodes.push_back(node);
                    }
                } catch (std::invalid_argument&) {
                    parsed_organ = false;
                }
                if (parsed_organ) {
                    for (std::size_t i = 4; i < strings.size();) {
                        try {
                            const auto inputID = std::stoi(strings.at(i));
                            ++i;
                            const auto inputFlow = std::stod(strings.at(i));
                            ++i;
                            const Connection conn = { ID, inputID, inputFlow };
                            connections.push_back(conn);
                        } catch (std::invalid_argument&) {
                            i += 2;
                        }
                    }
                }
            }
        }
    }

    std::map<int, int> id_map;
    for (int i = 0; i < m_nodes.size(); ++i) {
        const auto id = m_nodes[i]->id();
        if (id_map.count(id) > 0) {
            throw std::invalid_argument("ID of Node already exists");
        } else {
            id_map[id] = i;
        }
    }
    for (const auto& c : connections) {
        int idx = id_map.at(c.ID);
        int inputIdx = id_map.at(c.inputID);
        auto node = m_nodes[idx];
        auto inputNode = m_nodes[inputIdx];
        node->addInputNode(inputNode, c.flow);
    }

    return success;
}
