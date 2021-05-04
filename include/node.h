#pragma once

#include <array>
#include <concepts>
#include <utility>
#include <vector>

class Node {
public:
    Node(int ID, double volume, std::size_t steps = 3600, double total_time = 1);
    ~Node();
    int id() { return m_ID; }
    void changeID(const int ID);

    void addInputNode(Node* node, double flow);
    void addOrgan(double volume, double PS = 240.0);
    Node* organ() { return m_organPointer; }
    double getConcentration(std::size_t step);

    void setTotalSteps(std::size_t steps) { m_totalSteps = steps; }
    void setStepSize(double stepSize) { m_stepSize = stepSize; }
    std::size_t totalSteps() const { return m_totalSteps; }
    double stepSize() const { return m_stepSize; }

protected:
    void deleteOrganNode();
    void advanceTo(std::size_t step);
    virtual double evaluate(std::size_t, double C0) const;

    std::vector<double> m_concentration;
    int m_ID = 0;
    double m_volume = 20;
    std::size_t m_totalSteps = 3600;
    std::size_t m_current_step = 0;
    double m_stepSize = 0.1;
    Node* m_organPointer = nullptr;
    std::vector<std::pair<double, Node*>> m_inputs;
};

class ArterialInput : public Node {
public:
    ArterialInput(int ID, double volume, std::size_t steps = 3600, double total_time = 1);

    template <std::regular_invocable<double,double> F>
    requires std::is_same<std::invoke_result_t<F, double,double>, double>::value void setInputFunction(const F function)
    {
        const auto s = stepSize();
        for (std::size_t i = 0; i < totalSteps(); ++i) {
            m_inflow[i] = function(i * s, s);
        }
    }

protected:
    double evaluate(std::size_t step, double C0) const override;

private:
    std::vector<double> m_inflow;
};

class RenalClearence : public Node {
public:
    RenalClearence(int ID, double volume, std::size_t steps = 3600, double total_time = 1);
    void setRenalClearenceRate(double rate) { m_renalClearenceRate = rate; }

protected:
    double evaluate(std::size_t step, double C0) const override;

private:
    double m_renalClearenceRate = 240.0;
};
