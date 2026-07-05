//Hii this is the implementation of the neural net
#include "includeStatements.hpp"
#include "neuronv2.hpp"
#include "networkv2.hpp"

#include <iostream>
#include <random>
#include <cmath>

class world;

void processActionCode(int code, network& net); // Looks at the front of the action queue, and executes the action associated with the code, then pops it from the queue
// Recieves an action code from the queue, does action, pops from queue
// Contains what each action is bound to

void informNewState();
//Returns a positive or negative double centred at 0 to say whether the action was good or bad

// The test case the net will be run through is: (need to find a way to differentiate similarity)
// Given the target position and its position, including the distance gain from each step, how many times can the agent reach a randomized point given N movements

class world;
class position;


class position{
    public:
    position();
    position(int x, int y, int z);
    friend double distance(position P1, position P2);
    friend bool operator!=(const position& lhs, const position& rhs){
        return !(lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z);
    }

    int x = 0;
    int y = 0;
    int z = 0;

};

class world{
    public:
    world();
    double getSituationValue();
    void updateWorld(int actionCode);
    int getInputCode();

    void setTargetPosition(int x, int y, int z);
    void setInitialDistance();

    position target;
    position agentPosition;

    private:
    double previousDistance = 0.0;
};



int main() {
    network net;
    world World;

    net.addToWorld(&World);

    // IO neurons
    neuron* inputNeuron1 = new neuron(true, 0, &net); // Input neuron for positive situation value
    neuron* inputNeuron2 = new neuron(true, 1, &net); // Input for negative situation value
    neuron* inputNeuron3 = new neuron(true, 2, &net); // Input neuron for zero situation value

    neuron* outputNeuron1 = new neuron(false, 0, &net); // Output neuron for moving forward
    neuron* outputNeuron2 = new neuron(false, 1, &net); // Output neuron for moving backward
    neuron* outputNeuron3 = new neuron(false, 2, &net); // Output neuron for moving left
    neuron* outputNeuron4 = new neuron(false, 3, &net); // Output neuron for moving right
    neuron* outputNeuron5 = new neuron(false, 4, &net); // Output neuron for moving up
    neuron* outputNeuron6 = new neuron(false, 5, &net); // Output neuron for moving down

    // Non-special neurons
    for(int i = 0; i < 10; i++){
        neuron* n = new neuron(&net);
    }


    // Create a random target position
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> dist(-100, 100);

    int ii = 0; // Step counter


    // Start of big while loop
    for (int i = 0; i < 10; i++) {
        // Genereates a new target to move to, and sets the initial distance for the world
    World.setTargetPosition(dist(rng), dist(rng), dist(rng));
    World.setInitialDistance();
    ii = 0; // Reset the step counter for each new target
    // Start of small while loop
        // While the agent has not reached the target, process the next action code in the queue, get the situation value from the world, and update the network based on the situation value and the last action code
        while(World.agentPosition != World.target){
            ii++; // Increment the step counter
            // Process the next action code in the queue
                processActionCode(net.getNextAction(), net);
                std::cout << "Agent position: (" << net.World->agentPosition.x << ", " << net.World->agentPosition.y << ", " << net.World->agentPosition.z << ")" << std::endl;
                std::cout << "Target position: (" << net.World->target.x << ", " << net.World->target.y << ", " << net.World->target.z << ")" << std::endl;
                std::cout << "Distance to target: " << distance(net.World->agentPosition, net.World->target) << std::endl;
                std::cout << "step: " << ii << std::endl;
                std::cout << "___________________________________________________________" << std::endl;

            // Get the situation value from the world
            double situationValue = World.getSituationValue();

            net.processInputCode(0, situationValue > 0);
            net.processInputCode(1, situationValue < 0);
            net.processInputCode(2, situationValue == 0);
        }

    }

    return 0;
}



double distance(position P1, position P2){
    return sqrt(pow(P1.x - P2.x, 2) + pow(P1.y - P2.y, 2) + pow(P1.z - P2.z, 2));
}

world::world(){
    target = position(0,0,0);
    agentPosition = position(0, 0, 0);
}

position::position() : position(0, 0, 0) {}


void world::setTargetPosition(int x, int y, int z) {
    target = position(x, y, z);
}

double world::getSituationValue() {
    // Calculate the distance between the agent and the target
    double dist = distance(agentPosition, target);
    // Returns a positive value if the agent is closer to the target than before, and a negative value if it is further away
    double gain = previousDistance-dist;
    previousDistance = dist;
    return gain; 
}

void world::setInitialDistance() {
    previousDistance = distance(agentPosition, target);
}

position::position(int x, int y, int z){
    this->x = x;
    this->y = y;
    this->z = z;
}

// Looks at the front of the action queue, and executes the action associated with the code, then pops it from the queue
void processActionCode(int code, network& net){

    // Action code 1: Move forward
    // Action code 2: Move backward
    // Action code 3: Move left
    // Action code 4: Move right
    // Action code 5: Move up
    // Action code 6: Move down

    switch(code){
        case 0:
            net.World->agentPosition.z++;
            break;
        case 1:
            net.World->agentPosition.z--;
            break;
        case 2:
            net.World->agentPosition.x--;
            break;
        case 3:
            net.World->agentPosition.x++;
            break;
        case 4:
            net.World->agentPosition.y++;
            break;
        case 5:
            net.World->agentPosition.y--;
            break;
        default:
            std::cout << "Invalid action code: " << code << std::endl;
            break;
    }

    net.actionComplete(); // Tells the network that the action is complete, so it can update based on the situation value and the last action code

}































//when the net revamp what time spent mehwen a