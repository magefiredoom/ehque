#ifndef NEURONV2_HPP
#define NEURONV2_HPP

#include <vector>
#include <random>

struct synapse;
class neuron;
class network;

/*************************************************************************************************************
 What neurons are able to do:
    -   Belong to a network of other neurons (network itself processes outputs and inputs)
    -   Connect to another neuron (any neuron in the network or a neuron that is yet to exist, if it doesn't exist, it will be created, w/ 1 publisher & 1 subscriber, which may or may not exist depending on which side it is being connected from)
        -   Thus, be initialized given a) a network it belongs to, or b) a neuron which publishes / subscribes to, and c), a special +
    -   Store a (flexible) list of publishers(send stimuli to THIS) with their weights, and a list of subscribers(receive stimuli from THIS)
    -   Change all of their own arbitrary constants (weights, thresholds, initializing values, etc.)
    -   Become an input / output neuron, which when a net recieves a given code:
        - Input neurons will act as a neuron recieving a stimulus also with a given weight, or full activation
        - Output neurons, once past their threshold, will send a code to the net's output queue to be processed
        - Special neurons still act as standard neurons otherwise, but can do another action
    -   Store a state which is a boolean value
        -   Set to true when the publishers * their respective weights exceed the threshold, and false when they do not
        -   Know when states change and when to clear states 
    -   Somehow compute similarity between states and neurons
    -   Somehow manage to ensure they are not just linearly connected, and branch
    -   Once initial input neurons are turned off, the also send an update signal through their own subscribers, to determine whether they should swich their states.

***************************************************************************************************************/

class neuron{

public:
    //the network this neuron belongs to
    friend class network;
    network* net; 

    int index; // Index of this neuron in the network's storage for easy connection & saving

    neuron(network* n); // Creates a neuron that belongs to the given network, then it connects to 2 random neurons in network+1
    neuron(neuron* n, bool asInput); // Creates a neuron that is connected to the given neuron and belongs to the given neuron's network
    neuron(bool isInput, int specialCode, network* n); // Creates a special neuron within network n, which then connects to a random neuron in the network+1

    // Simple publish / subscribe functions, takes the index so it can create neurons that don't exist yet
    void publishTo(int neuronIndex); // Weight of this on n is n->threshold *2 / n->publishers.size(), this value diverges if you keep adding more publishers, and makes new connections somewhat meaningful, but not overwhelming
    void subscribeTo(int neuronIndex); // Weight of n on this is this->threshold *2 / this->publishers.size(), following same logic

    // Conditiion functions
    void recieveStimuli(); // Checks if any publishers are active, then determines own state afterwards, then if active, sends stimuli to subscribers
    void update(double situationValue, int from);
    // Updates based on situation; ranges from -x to +x, where the bigger abs(x), the more impact acting on the update
    // During the update, neuron can: 
    // 1) Update thresholds and weights associated with creating this situation
    // If weight weights are increased if the publisher was on during activation and decreased if it was off, or inverse if the situation value is negative
    // 2) Update the # publishers and subscribers, from network+1
    // 3) Start to recieve / send from an action code
    // Updates are called backwards from an output neuron
    // If a neuron cannot be activated, it dies
    // From prevents infinite recursion and just goes until it goes back to the initial output neuron

    // Special functions
    void queueOutput(int code); // If neuron's an output, tells net to execute the action
    void passInput(int code, bool on); // If net recieves an input, tells input neurons to activate

private:

    // The threshold of this neuron, which is the value that must be exceeded for it to activate and send a code to the net's output queue
    double threshold = 1.0;

    // The state of this neuron, which is true when the sum of the weighted inputs exceeds the threshold, and false when it does not
    bool state = false; 

    // Publisher and subscriber lists of this neuron
    std::vector<synapse> publishers; // Neurons that send stimuli to this neuron, with weights
    std::vector<neuron*> subscribers; // Neurons that recieve stimuli from this neuron

    //Variables for special functionality
    bool special = false; // If true, is either a input or output neuron
    bool isInput = false; // If true, is an input neuron, if false, is an output neuron
    int specialCode = -1; // Code the neuron either sends (if output) or recieves (if input) from

    //Arbitrary variables for initialization & learning
    double constantChangeMultiplier = 0.01; // Multiplier for how much to change constants when updating, should be small to avoid overshooting
    double randomConnectionChance = 0.01; // Chance to randomly add a publisher or subscriber when updating
    double randomSpecialChance = 0.01; // Chance to randomly make a neuron special (input or output)
    double oppositeWeight = 0.1;
    double maxRandomness = 1.0;
};

struct synapse{
    neuron* from;
    double weight;
};


#endif // NEURONV2_HPP