
// Constructor functions for network
network::network() : inputs(0), outputs(0), rng(std::random_device{}()), world(nullptr) {}

// Destructor function for network, deletes all neurons in the network
network::~network() {
    for (neuron* n : neurons) {
        delete n;
    }
}

void network::addNeuron(neuron* n) {
    neurons.push_back(n);
}

void network::removeNeuron(int index) {
    if (index < 0 || index >= neurons.size()) {
        return; // Invalid index
    }

    neuron* n = neurons[index];

    // Remove this neuron from its publishers' subscribers list
    for (synapse& s : n->publishers) {
        neuron* publisher = s.from;
        publisher->subscribers.erase(std::remove(publisher->subscribers.begin(), publisher->subscribers.end(), n), publisher->subscribers.end());
    }

    // Remove this neuron from its subscribers' publishers list
    for (neuron* subscriber : n->subscribers) {
        subscriber->publishers.erase(std::remove_if(subscriber->publishers.begin(), subscriber->publishers.end(),
            [n](const synapse& s) { return s.from == n; }), subscriber->publishers.end());
    }

    // Delete the neuron and remove it from the network's list
    neurons.erase(neurons.begin() + index);
    delete n;

    // Update indices of remaining neurons
    for (int i = index; i < neurons.size(); ++i) {
        neurons[i]->index = i;
    }
}

void network::queueActionCode(int code) {
    actionQueue.push_back(code);
}

void network::processInputCode(int code, bool on) {
    for (neuron* n : neurons) {
        if (n->special && n->isInput && n->specialCode == code) {
            n->passInput(code, on);
        }
    }
}

void network::updateNetwork(double situationValue, int lastActionCode) {
    for (neuron* n : neurons) {
        if (n->special && !n->isInput && n->specialCode == lastActionCode) {
            n->update(situationValue, n->index);
        }
    }
}

int network::getNextSpecialCode(bool isInput) {

    // Ensures that the next special code is within the range of available inputs or outputs
    if(nextInputCode == inputs+1){
        nextInputCode = 0;
    }
    if(nextActionCode == outputs+1){
        nextActionCode = 0;
    }

    if (isInput) {
        return nextInputCode++;
    } else {
        return nextActionCode++;
    }
}

void network::actionComplete() {
    if (!actionQueue.empty()) {
        int lastActionCode = actionQueue.front();
        actionQueue.pop_front();
        updateNetwork(normalizeSituationValue(World->getSituationValue()), lastActionCode);
    }
}

double network::normalizeSituationValue(double value) {
    if (value > maxSituationValue) {
        maxSituationValue = value;
    }
    if (value < minSituationValue) {
        minSituationValue = value;
    }

    if(value > 0){
        return value / maxSituationValue;
    } else {
        return value / -minSituationValue; //Doesn't matter if value is 0, will return 0
    }
}

void network::addToWorld(world* w) {
    world = w;
}