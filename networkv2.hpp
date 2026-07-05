#ifndef NETWORKV2_HPP
#define NETWORKV2_HPP

#include <deque>
#include <vector>
#include <iostream>
#include <random>

#include "neuronv2.hpp"

class neuron;
class world;



/****************************************************************************************************
 What networks are able to do:
    -   Store a list of neurons, add and track them properly, and remove them when they die
    -   Notify neurons when input codes are recieved
    -   Process output codes from neurons and queue their actions 
    -   Read value of a situation (could bump to higher level)
    -   Assign special codes to new input and output neurons, and keep track of them
    -   Update the network based on the situation value, and the previous action
    -   Save the network to a file, and load it from a file
    -   Handle case where nothing is queued for actions, thus not creating any inputs

    -   State predictions?
****************************************************************************************************/


class network {
public:

    friend class neuron;
    friend class world;

    world* World; // Pointer to the world object, so that the network can access the situation value

    network();
    ~network();

    void addToWorld(world* w); // Adds a world to the network, so that it can access the situation value

    void addNeuron(neuron* n); // Adds a neuron to the network
    void removeNeuron(int index); // Removes a neuron from the network, and updates all other neurons' publishers and subscribers accordingly

    void queueActionCode(int code); // Adds an action code to the queue
    void processInputCode(int code, bool on); // Processes an input code by notifying the input neurons to activate, which then activates the rest of the network
    void updateNetwork(double situationValue, int lastActionCode); // Updates the network based on the situation value, and the previous action (tracks the activated output neurons and updates them accordingly)
    int getNextSpecialCode(bool isInput); // Returns the next special code for an input or output neuron, and increments the counter
    void actionComplete(); // Called when an action is complete, to update the network based on the situation value and the previous action
    double normalizeSituationValue(double value); // Normalizes the situation value to be between -1 and 1, based on the maximum and minimum values seen so far
    int getNextAction() { return actionQueue.front(); } // Returns the next action code for an output neuron, and increments the counter

    void saveNetwork(std::ostream& outs); // Just saves the network wherever I guess

private:

    std::vector<neuron*> neurons; // List of all neurons in the network, with their index in the vector being their index in the network
    std::deque<int> actionQueue; // Queue of action codes to be processed, with the most recent at the front
    std::mt19937 rng; // Random number generator for the network, used for random connections and special neurons

    // Sidenote to remember that input and output code 0 is valid
    int inputs = 0;
    int outputs = 0;

    int nextInputCode = 0; // Keeps track of the next input code to assign
    int nextActionCode = 0; // Keeps track of the next action code to assign

    double maxSituationValue = 1.0; // Keeps track of the maximum situation value seen so far, for normalization
    double minSituationValue = -1.0; // Keeps track of the minimum situation value seen so far, for normalization
};

#endif // NETWORKV2_HPP