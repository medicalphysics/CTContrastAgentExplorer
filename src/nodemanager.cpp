#include "..\include\nodemanager.h"
#include "nodemanager.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <map>

NodeManager::NodeManager()
{
    m_nodes.clear();
}
NodeManager::NodeManager(const std::vector<std::string>& organConfigLines)
{
    readOrganNetworkFromText(organConfigLines);
}
NodeManager::~NodeManager()
{
    m_nodes.clear();
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

void NodeManager::readOrganNetworkFromText(const std::vector<std::string>& lines)
{
    clearNodes();

    struct Connection {
        int ID = 0;
        int inputID = 0;
        double flow = 0.0;
    };

    std::vector<Connection> connections;

    for (const auto& line : lines) {
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
                        m_arterialNode = new ArterialInput(ID, name, IVvolume, m_stepSize);
                        m_nodes.push_back(m_arterialNode);
                    } else if (ID == 255) {
                        auto node = new RenalClearence(ID, name, IVvolume, m_stepSize);
                        if (ECvolume > 0.0) {
                            node->addOrgan(ECvolume, m_organPS);
                        }
                        m_nodes.push_back(node);
                    } else {
                        auto node = new Node(ID, name, IVvolume, m_stepSize);
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
    for (int i = 0; i < static_cast<int>(m_nodes.size()); ++i) {
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

    for (auto n : m_nodes) {
        const auto volumeScaling = m_bloodVolume / referenceBloodVolume();
        const auto flowScaling = m_cardiacOutput / referenceCardiacOutput();
        n->setBloodVolumeScaling(volumeScaling);
        n->setCardiacOutputScaling(flowScaling);
    }
    if (m_renalNode) {
        m_renalNode->setRenalClearenceRate(m_renalClearenceRate);
    }
}

double NodeManager::getConcentration(const int ID, const double time)
{
    if (ID == 255) {
        auto last = m_nodes.back();
        if (last->id() == ID)
            return last->getConcentration(time);
    }
    if (ID < static_cast<int>(m_nodes.size()) && ID >= 0) {
        auto node = m_nodes[ID];
        if (node->id() == ID)
            return node->getConcentration(time);
    }
    return -1;
}

void NodeManager::resetNodes()
{
    for (auto n : m_nodes) {
        n->resetNode();
    }
}

void NodeManager::setStepSize(const double step)
{
    m_stepSize = step;
    for (auto n : m_nodes) {
        n->setStepSize(step);
    }
}

void NodeManager::setOrganPS(const double PS)
{
    m_organPS = PS;
    for (auto n : m_nodes) {
        n->setOrganPS(PS);
    }
}

void NodeManager::setBolusInjection(const double injection_time, const double volume, const double strenght)
{
    if (m_arterialNode) {
        m_arterialNode->setInput(volume, injection_time, strenght);
        resetNodes();
    }
}

void NodeManager::setBloodVolume(const double BV)
{
    m_bloodVolume = BV;
    const auto scaling = m_bloodVolume / referenceBloodVolume();
    for (auto n : m_nodes) {
        n->setBloodVolumeScaling(scaling);
    }
}

void NodeManager::setCardiacOutput(const double CO)
{

    m_cardiacOutput = CO;
    const auto scaling = CO / referenceCardiacOutput();
    for (auto n : m_nodes) {
        n->setCardiacOutputScaling(scaling);
    }
}

void NodeManager::setRenalClearenceRate(const double rate)
{
    m_renalClearenceRate = rate;
    if (m_renalNode) {
        m_renalNode->setRenalClearenceRate(rate);
    }
    resetNodes();
}

double NodeManager::bloodVolume() const
{
    const double a = m_isMale ? 33.164 : 34.85;
    const double b = m_isMale ? 1.229 : 1.954;

    return a * std::pow(m_patientHeight / 2.54, 0.725) * std::pow(m_patientWeight / 2.2, 0.425) - b;
}
double NodeManager::cardiacOutput() const
{
    const double ref = 36.36 * std::pow(m_patientHeight / 2.54, 0.725) * std::pow(m_patientWeight / 2.2, 0.425);

    return ref * (1.0 - (m_patientAge - 30.0) * 1.01 / 100.0);
}

double NodeManager::renalClearence() const
{
    const double Q = m_isMale ? 1 : 0.85;
    return Q * ((140.0 - m_patientAge) * m_patientWeight) / (72 * m_patientCreatinine);
}
