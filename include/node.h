#pragma once

#include <array>
#include <concepts>
#include <functional>
#include <string>
#include <utility>
#include <vector>

class Node;

struct Input {
    double flow = 0;
    double delay = 0;
    Node* node = nullptr;
};

class Node {
public:
    Node(int ID, const std::string& name, const double volume, bool isVascular = true);
    virtual ~Node();
    int id() const { return m_ID; }
    void changeID(const int ID);

    void addInputNode(Node* node, double flow, double delay = 0.0);
    void addOrgan(double volume, double PS = 240.0);
    Node* organ() const { return m_organPointer; }
    double getConcentration(const double time);
    double getConcentration(const std::size_t step);
    double volume() const { return m_volume * m_volumeScaling; }
    double volumeEES() const { return m_isVascular ? volume() * 0.48 : volume() * 0.04; }
    void setTimeStep(const double time);
    double timeStep() const { return m_timeStep; }
    std::string name(bool fullName = false) const;
    void setOrganPS(const double PS);
    auto organPS() const { return m_ps; }
    void resetNode();
    void setCardiacOutputScaling(const double scaling);
    void setBloodVolumeScaling(const double scaling);
    const auto& inputs() const { return m_inputs; }
    void clearInputs();
    const auto nSteps() const { return m_concentration.size(); }

protected:
    void deleteOrganNode();
    void advanceTo(const std::size_t step);
    virtual double evaluate(const std::size_t step, const double C0) const;

private:
    bool m_isVascular = true;
    int m_ID = 0;
    double m_volume = 20;
    double m_flowScaling = 1;
    double m_volumeScaling = 1;
    std::size_t m_current_step = 0;
    std::array<double, 1024> m_concentration;
    double m_timeStep = 0.004;
    double m_ps = 4 * 60;
    Node* m_organPointer = nullptr;
    std::string m_name;
    std::vector<Input> m_inputs;
};

class ArterialInput : public Node {
public:
    ArterialInput(int ID, const std::string& name, double volume);

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
    RenalClearence(int ID, const std::string& name, double volume);
    void setRenalClearenceRate(const double rate);

protected:
    double evaluate(const std::size_t step, const double C0) const override;

private:
    double m_renalClearenceRate = 240.0;
};
