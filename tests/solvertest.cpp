
#include "node.h"
#include "nodemanager.h"

#include <iostream>
#include <vector>

void testNodes()
{
    std::vector<Node*> nodes;

    const std::size_t steps = 360;
    const double time = 1;

    ArterialInput* input = new ArterialInput(0, 80.0, steps, time);
    auto f = [=](double time, double step) -> double {
        const auto volume = 120.0;
        const auto rate = 4.0 * 60;
        const auto end = volume / rate;
        return time <= end ? volume / end * step : 0.0;
    };
    input->setInputFunction(f);
    nodes.push_back(input);

    auto vessel1 = new Node(1, 80, steps, time);
    nodes.push_back(vessel1);

    auto organ1 = new Node(2, 37.0, steps, time);
    organ1->addOrgan(484.0);
    nodes.push_back(organ1);

    auto vessel2 = new Node(3, 80, steps, time);
    nodes.push_back(vessel2);

    auto kidney = new RenalClearence(4, 54, steps, time);
    kidney->addOrgan(89.0);
    nodes.push_back(kidney);

    //connections
    const double flow = 975;
    vessel1->addInputNode(input, flow);

    organ1->addInputNode(vessel1, flow);

    vessel2->addInputNode(organ1, flow);

    kidney->addInputNode(vessel2, flow / 2);
    input->addInputNode(vessel2, flow / 2);
    input->addInputNode(kidney, flow / 2);

    auto organ1_ec = organ1->organ();
    auto kidney_ec = kidney->organ();
    std::cout << "time, in, vessel1, organ, organ_ec, vessel2, kidney, kidney_ec\n";
    for (std::size_t i = 0; i < steps; ++i) {
        std::cout << i * time / steps << ", ";
        std::cout << input->getConcentration(i) << ", ";
        std::cout << vessel1->getConcentration(i) << ", ";
        std::cout << organ1->getConcentration(i) << ", ";
        std::cout << organ1_ec->getConcentration(i) << ", ";
        std::cout << vessel2->getConcentration(i) << ", ";
        std::cout << kidney->getConcentration(i) << ", ";
        std::cout << kidney_ec->getConcentration(i) << ", ";
        std::cout << std::endl;
    }
}
void testManager()
{
    auto manage = NodeManager();
    manage.testNetwork();
    

}

int main()
{
    testManager();
    //testNodes();
}
