#include "networkv2.hpp" //network can be a deque
#include <cmath>

    //TODO: HANDLE CASE WHERE NETWORK IS EMPTY, AND THEREFORE NO NEURONS TO CONNECT TO (all constructors), also, add neuron to network
//Standard neuron constructor, puts a neuron into a network, and connects both ends to random neurons in the network+1, recieves some data from network
neuron::neuron(network* n) : net(n) , index(n->neurons.size()) {

    n->addNeuron(this);

    //Generates random neuron
    std::uniform_int_distribution<int> pickNeuron(0, net->neurons.size());

    // Links to the two random neurons
    publishTo(pickNeuron(net->rng));
    subscribeTo(pickNeuron(net->rng));

}

// Constructor for neuron connected to a given neuron, recieves data from neuron
neuron::neuron(neuron* n, bool asInput) : net(n->net) , index(n->net->neurons.size()) {

    n->net->addNeuron(this);

    //Generates random neuron
    std::uniform_int_distribution<int> pickNeuron(0, net->neurons.size());

    // Links both ends of neuron
    if(asInput){
        subscribeTo(n->index);
        publishTo(pickNeuron(net->rng));
    } else {
        publishTo(n->index);
        subscribeTo(pickNeuron(net->rng));
    }

}

// Constructor for special neurons, assigned initially as an in or out
neuron::neuron(bool isInput, int specialCode, network* n) : special(true), isInput(isInput), specialCode(specialCode), net(n), index(n->neurons.size()) {

    n->addNeuron(this);

    //Generates random neuron
    std::uniform_int_distribution<int> pickNeuron(0, net->neurons.size());

    // Links to a random neuron in the network+1
    if(isInput){
        publishTo(pickNeuron(net->rng));
    } else {
        subscribeTo(pickNeuron(net->rng));
    }

}


// Publishing and subscrbing functions, if it publishes twice to a neuron, just has it counted twice with different weights and it's what it's
// Publishes to neuron n, by adding self to n's publishers and n to self's subscribers, with a weight based on n's threshold and number of publishers
void neuron::publishTo(int neuronIndex){

    neuron* n;
        if(neuronIndex < 0 || neuronIndex >= net->neurons.size()){
            n = new neuron(this, true);
        } else{
            n = net->neurons[neuronIndex];
        }

    // Adds this neuron to the given neuron's publishers, and adds the given neuron to this neuron's subscribers
    synapse s;
    s.from = this;
    s.weight = n->threshold * 2 / (n->publishers.size() + 1);
    n->publishers.push_back(s);
    subscribers.push_back(n);
}

// Subscribes to neuron n, by adding self to n's subscribers and n to self's publishers, with a weight based on self's threshold and number of publishers
void neuron::subscribeTo(int neuronIndex){

    neuron* n;
        if(neuronIndex < 0 || neuronIndex >= net->neurons.size()){
            n = new neuron(this, false);
        } else{
            n = net->neurons[neuronIndex];
        }

    // Adds the given neuron to this neuron's publishers, and adds this neuron to the given neuron's subscribers
    synapse s;
    s.from = n;
    s.weight = threshold * 2 / (publishers.size() + 1);
    publishers.push_back(s);
    n->subscribers.push_back(this);
}

//Condition functions for learning and stuff

// Determine whether it needs to be activated, and if so, send stimuli to subscribers
void neuron::recieveStimuli(){

    // Checks if any publishers are active, and if so, determines own state afterwards
    double sum = 0;
    for(synapse s : publishers){
            sum += s.weight * s.from->state;
    }

    // If the sum of the weighted inputs exceeds the threshold, activate and send stimuli to subscribers
    if(sum > threshold){
        state = true;
        queOutput(); // If it's an output neuron, tells the net to execute the action
        for(neuron* n : subscribers){
            n->recieveStimuli();
        }
    } else {
        state = false;
        for(neuron* n : subscribers){ // Still needs subscribers to know that it is off, so they can update their own states accordingly
            n->recieveStimuli();
        }
    }

}

// Updates based on situation value, should generally be around -1 to 1, where the bigger abs(x), the more impact acting on the update
void neuron::update(double situationValue, int from){

    std::uniform_real_distribution<double> randomChance(-oppositeWeight*maxRandomness, maxRandomness);;
    std::uniform_int_distribution<int> pickNeuron(0, net->neurons.size());

    // Random chance multiplier to see if we should do the opposite of what we normally would, to avoid getting stuck in a local minimum
    double ehqueMultiplier = randomChance(net->rng);

    // Updates weights based on weights, the situation, and whether the publisher was on during activation, with some randomness
    for(synapse& s : publishers){
        // If the publisher was on during activation, increase the weight, otherwise decrease it, or inverse if the situation value is negative
        if(s.from->state){
            s.weight += constantChangeMultiplier * situationValue * ehqueMultiplier * s.weight;
        } else {
            s.weight -= constantChangeMultiplier * situationValue * ehqueMultiplier * s.weight;
        }
    }

    // Makes some more publishers or subscribers, with some randomness
    if(randomChance(net->rng) < randomConnectionChance){
        if(randomChance(net->rng) < (maxRandomness + oppositeWeight * maxRandomness) / 2){
            // Makes a new publisher
            subscribeTo(pickNeuron(net->rng));
        } else {
            // Makes a new subscriber
            publishTo(pickNeuron(net->rng));
        }
    }

    // Randomly makes a neuron special (input or output)
    if(randomChance(net->rng) < randomSpecialChance){
        special = true;
        isInput = randomChance(net->rng) < (maxRandomness + oppositeWeight * maxRandomness) / 2;
        specialCode = net->getNextSpecialCode(isInput); // MAKE SURE FUNCTION IS IMPLEMENTED IN NETWORK
    }

    // Checks to see neuron still COULD activate
    double sum = 0.0;
    for (synapse s : publishers){
        sum += fabs(s.weight);
    }

    if(from != index){
    for(neuron* n : subscribers){
        n->update(situationValue, from); // Updates subscribers as well, so they can update their own values accordingly
        }
    }
    // What would the steady state of a network look like?
    constantChangeMultiplier *= 1.0 - ehqueMultiplier * constantChangeMultiplier; // Decreases the constant change multiplier over time, so that the neuron learns more slowly as it gets older
    randomConnectionChance *= 1.0 - ehqueMultiplier * randomConnectionChance * constantChangeMultiplier; // Decreases the random connection chance over time, so that the neuron learns more slowly as it gets older
    randomSpecialChance *= 1.0 - ehqueMultiplier * randomSpecialChance * constantChangeMultiplier; // Decreases the random special chance over time, so that the neuron learns more slowly as it gets older
    oppositeWeight *= 1.0 - ehqueMultiplier * oppositeWeight * constantChangeMultiplier; // Decreases the opposite weight over time, so that the neuron learns more slowly as it gets older
    maxRandomness *= 1.0 - ehqueMultiplier * maxRandomness * constantChangeMultiplier; // Decreases the max randomness over time, so that the neuron learns more slowly as it gets older


    if(sum < threshold){
        // If it can't activate, it dies
        // Notifies subscribers and publishers that it is dead, and removes itself from the network
        net->removeNeuron(index);
    }

}

// Special functions for input and output neurons
void neuron::queueOutput(int code){
    if(special && !isInput){
        // TO BE IMPLEMENTED IN NETWORK
        net->queueActionCode(code);
    }
}

void neuron::passInput(int code, bool on){
    if(special && isInput && specialCode == code){
        state = on;
        for(neuron* n : subscribers){
            n->recieveStimuli();
        }
    }
}

