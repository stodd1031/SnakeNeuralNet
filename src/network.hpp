#pragma once

#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <memory.h>
#include "MyArr.h"

#define LR .005

double sigmoid(double num);

double sigmoidPrime(double num);

double none(double num);
double nonePrime(double num);

double relu(double num);
double reluPrime(double num);

double step(double num);
double stepPrime(double num);

#define GOAL forwardsWeights[0]->goal

class Neuron;

struct Weight
{
public:
    Neuron *begginingNeuron = nullptr;
    union
    {
        double weight = 0;
        double goal;
    };
    double weightChange = 0;
    Neuron *endNeuron = nullptr;
};

class Neuron
{
public:
    double value = 0;
    double activation = 0;
    double bias = 0;

    double (*activationFunction)(double) = none;
    double (*activationFunctionPrime)(double) = nonePrime;

    // std::vector<Weight *> backwardsWeights;
    // std::vector<Weight *> forwardsWeights;

    MyArr<Weight *> backwardsWeights;
    MyArr<Weight *> forwardsWeights;

    Neuron();
    friend class Layer;
};

class Layer
{
public:
    // std::vector<Neuron> neurons;
    MyArr<Neuron> neurons;
    int neuronCount = 0;

    Layer();
    Layer(int layerSize);

    void step();

    void print();

    friend class Model;
};

class Model
{
public:
    // std::vector<Layer> layers;
    MyArr<Layer> layers;
    Layer *firstLayer;
    Layer *lastLayer;
    int layerCount = 0;

    // std::vector<Neuron *> allNeurons;
    std::vector<Weight *> allWeights;

    Model(std::vector<int> modelShape);
    Model();
    void setup(std::vector<int> modelShape);
    ~Model();

    void step(double *input);
    void step(int *input);
    void step(std::vector<double> input);
    void step(MyArr<double> input);

    void print();

    void SGDBias(Neuron &neuron);
    void SGDWeight(Weight *weight);
    void SGDRec(Weight *weight, double previousChain, double &derivative);
    void SGDWeightsAndApply(std::vector<double> goal);
    void SGDRewardWeights(int outputNeuron, int reward);
    void SGDRewardWeightsBatch(std::vector<std::vector<double>> input, std::vector<int> outputs, int reward);

    int getHighestOutputIndex();
    double getHighestOutput();
    std::vector<double> getOutputVector();

    void changeActivation(int layer, double (*func)(double num));
    void changeActivationPrime(int layer, double (*func)(double num));

    // manage weights and connection

    void randomizeNeuronWeights(double min, double max);
    void randomizeNeuronWeightsChoice(std::vector<double> choices);
    void mutate(Model &sourceModel, int percent, std::vector<double> choices);
    void mutate(Model &sourceModel, int percent, double min, double max);
    void mutate(int percent, double min, double max);
    void mutateChange(Model &sourceModel, int percent, double min, double max);
    void mutateChange(int percent, double min, double max);
    void defaultWiring();
    void breed(Model &parent1, Model &parent2);

    // operators

    void operator=(const Model &model);

    friend class GeneticModel;
};

class GeneticModel
{
public:
    // int generations
    int generation = 1;
    std::vector<int> modelShape;

    double (*scoreFunction)(Model &model);

    Model baseModel;

    GeneticModel();
    GeneticModel(std::vector<int> modelShape, int threads, double (*scoreFunction)(Model &model));
    ~GeneticModel();

    void runGenerations(int generationCount, int generationSize);
    void runGenerations2Parents(int generationCount, int generationSize);
    void genCuda(int generationCount, int generationSize);
};