#pragma once

#include <array>
#include <concepts>
#include <functional>
#include <utility>
#include <vector>

class Node {
public:
    Node(int ID, const std::string& name, const double volume, const double stepSize = 1.0 / 60.0);
    virtual ~Node();
    int id() const { return m_ID; }
    void changeID(const int ID);

    void addInputNode(Node* node, double flow);
    void addOrgan(double volume, double PS = 240.0);
    Node* organ() const { return m_organPointer; }
    double getConcentration(const double time);
    double getConcentration(const std::size_t step);
    double volume() const { return m_volume * m_volumeScaling; }
    void setStepSize(const double step);
    double stepSize() const { return m_stepSize; }
    const std::string& name() const { return m_name; }
    void setOrganPS(const double PS);
    void resetNode();
    void setCardiacOutputScaling(const double scaling);
    void setBloodVolumeScaling(const double scaling);

protected:
    void deleteOrganNode();
    void advanceTo(const std::size_t step);
    virtual double evaluate(const std::size_t step, const double C0) const;

private:
    int m_ID = 0;
    double m_volume = 20;
    double m_flowScaling = 1;
    double m_volumeScaling = 1;
    std::size_t m_current_step = 0;
    std::vector<double> m_concentration;
    double m_stepSize = 1.0 / 60.0;
    Node* m_organPointer = nullptr;
    std::string m_name;
    std::vector<std::pair<double, Node*>> m_inputs;
};

class ArterialInput : public Node {
public:
    ArterialInput(int ID, const std::string& name, double volume, double stepSize_max = 1.0 / 60.0);

    
    void setInput(double volume, double time, double strenght)
    {
        m_injVolume = volume;
        m_injTime = std::max(time, 0.001);
        m_caStrenght = strenght;
        resetNode();
    }

protected:
    double evaluate(const std::size_t step, const double C0) const override;

private:
    double m_injVolume = 1;
    double m_injTime = 1;
    double m_caStrenght = 1;

};

class RenalClearence : public Node {
public:
    RenalClearence(int ID, const std::string& name, double volume, double stepSize_max = 1.0 / 60.0);
    void setRenalClearenceRate(const double rate);

protected:
    double evaluate(const std::size_t step, const double C0) const override;

private:
    double m_renalClearenceRate = 240.0;
};
