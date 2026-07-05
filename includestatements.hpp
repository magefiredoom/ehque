#ifndef INCLUDESTATEMENTS_HPP
#define INCLUDESTATEMENTS_HPP

#include "neuronv2.hpp"
#include "networkv2.hpp"

#endif // INCLUDESTATEMENTS_HPP


/*
Updated 7/05/26

Fun fact! 4th or 5th attempt of me making AIs from scratch
Test implementation of net in main.cpp
Made ownership of world stronger
World's best O(never) solution to any P problem
Fun fact! 3d roam test as drunk birds never find their way home

Updated 7/04/26

How to use neural net!

Assign input codes which will trigger neurons to activate
Assing output codes which when output neurons activate, actions are quebed by the network to be executed

Create a "world" class which is able to evaluate the state of the world
(eventually will try to have situation value as a certain chunk of neurons being active, like a real brain)
"world" just needs an ability to evaluate the situation value with no arguements in a function:
"world::getSituationValue()"
This function should return a double value, spanning -n to +n, where larger abs(n) impact the neuron's activation more.

Once world and i/o codes are set up, declare a network object
After that, initialize a few neurons spanning the i/o codes (at least 1 input, 1 output)
Additionally, create some non-special neurons to fill network

Remember to save your network to a file for easy access

Then you're set!

*Needs to work on prediction How'd've shit've played out
*/