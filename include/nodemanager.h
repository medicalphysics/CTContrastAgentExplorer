#pragma once

#include "node.h"

#include <string>
#include <vector>

class NodeManager {
public:
    NodeManager();
    NodeManager(const std::vector<std::string>& organConfigLines);
    ~NodeManager();
    void clearNodes();

    const std::vector<Node*>& nodes() const { return m_nodes; }
    double getConcentration(const int nodeID, const double time);
    void resetNodes();

    void setStepSize(const double step);
    void setOrganPS(const double PS);
    void setBolusInjection(const double injection_time, const double volume, const double strenght);
    void setBloodVolume(const double BV);
    void setCardiacOutput(const double CO);
    void setRenalClearenceRate(const double rate);

protected:
    void readOrganNetworkFromText(const std::vector<std::string>& lines);
    constexpr double referenceCardiacOutput() const { return 6500; }
    constexpr double referenceBloodVolume() const { return 4760; }
    double bloodVolume() const;
    double cardiacOutput() const;
    double renalClearence() const;

private:
    double m_stepSize = 1 / 60.0;
    double m_organPS = 4.0 * 60;
    double m_bloodVolume = referenceBloodVolume();
    double m_cardiacOutput = referenceCardiacOutput();
    double m_renalClearenceRate = 240;
    bool m_isMale = true;
    double m_patientWeight = 70;
    double m_patientHeight = 175;
    double m_patientAge = 40;
    double m_patientCreatinine = 100; // uMol/liter
    ArterialInput* m_arterialNode = nullptr;
    RenalClearence* m_renalNode = nullptr;
    std::vector<Node*> m_nodes;
};
