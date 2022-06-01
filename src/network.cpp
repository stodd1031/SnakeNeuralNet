#include "network.hpp"

/* #region  misc */
double sigmoid(double num)
{
    // double ret = num / (1 + abs(num)) + 1;
    // ret *= 0.5;
    // return ret;

    double ret = 1 / (1 + exp(-num));
    return ret;
}

double sigmoidPrime(double num)
{
    // if (num == 0)
    // {
    //     return 1;
    // }
    // double ret = (1 + abs(num)) - (num) * (num / abs(num));
    // ret /= (1 + abs(num)) * (1 + abs(num)) * 2;
    // return ret;

    double s = sigmoid(num);
    return s * (1 - s);
}

double none(double num)
{
    return num;
}

double nonePrime(double num)
{
    return 1;
}

double relu(double num)
{
    if (num < 0)
    {
        return 0;
    }
    return num;
}

double reluPrime(double num)
{
    if (num < 0)
    {
        return 0;
    }
    return 1;
}

double step(double num)
{
    if (num > 0)
    {
        return 1;
    }
    return 0;
}

double stepPrime(double num)
{
    return 0;
}

/* #endregion */

/* #region Neuron */

Neuron::Neuron()
{
}

/* #endregion */

/* #region Layer */

Layer::Layer() {}

Layer::Layer(int layerSize)
{
    neuronCount = layerSize;
    // neurons = std::vector<Neuron>(layerSize);
    neurons.setSize(layerSize);
}

void Layer::step()
{
    for (int neuron = 0; neuron < neuronCount; neuron++)
    {
        neurons[neuron].value = 0;

        for (int weight = 0, size = neurons[neuron].backwardsWeights.size(); weight < size; weight++)
        {
            neurons[neuron].value += neurons[neuron].backwardsWeights[weight]->weight * neurons[neuron].backwardsWeights[weight]->begginingNeuron->activation;
        }

        neurons[neuron].value += neurons[neuron].bias;

        // neurons[index].activation = sigmoid(neurons[index].value);
        // neurons[index].activation = neurons[index].value;

        neurons[neuron].activation = neurons[neuron].activationFunction(neurons[neuron].value);
    }
}

void Layer::print()
{
    for (int index = 0; index < neuronCount; index++)
    {
        std::cout << std::fixed << std::setprecision(3) << neurons[index].activation << " " << std::flush;
    }
    std::cout << std::endl;
}

/* #endregion */

/* #region Model */

Model::Model(std::vector<int> modelShape)
{
    setup(modelShape);
}

Model::Model()
{
}

Model::~Model()
{
    for (int weight = 0, size = allWeights.size(); weight < size; weight++)
    {
        delete (allWeights[weight]);
    }
    for (int neuron = 0; neuron < lastLayer->neuronCount; neuron++)
    {
        delete lastLayer->neurons[neuron].forwardsWeights[0];
    }
}

void Model::setup(std::vector<int> modelShape)
{
    layerCount = modelShape.size();
    // layers.setSize(layerCount);
    for (int index = 0; index < layerCount; index++)
    {
        layers.push_back(Layer(modelShape[index]));
    }
    firstLayer = &(layers[0]);
    lastLayer = &(layers[layerCount - 1]);
}

void Model::defaultWiring()
{
    for (int layer = 0; layer < layerCount - 1; layer++)
    {
        for (int neuron = 0; neuron < layers[layer].neuronCount; neuron++)
        {
            for (int nextNeuron = 0; nextNeuron < layers[layer + 1].neuronCount; nextNeuron++)
            {
                
                // new uninitialized weight
                Weight *newWeight = new Weight();
                // append newWeight to vector of this neruons forward wieghts
                layers[layer].neurons[neuron].forwardsWeights.push_back(newWeight);
                // append newWeight to next layer neuron backwards weights
                layers[layer + 1].neurons[nextNeuron].backwardsWeights.push_back(newWeight);
                // std::cout << "size: " << layers[layer + 1].neurons[nextNeuron].backwardsWeights.size() << std::endl;
                // the created weights pointer to the neurons at the front and end
                newWeight->begginingNeuron = &layers[layer].neurons[neuron];
                newWeight->endNeuron = &layers[layer + 1].neurons[nextNeuron];

                // add weight to all weights vector
                allWeights.push_back(newWeight);
            }
        }
    }

    // last layer weights (treated as goals)
    // not added to all weights, not treated as weights
    for (int neuron = 0; neuron < lastLayer->neuronCount; neuron++)
    {
        lastLayer->neurons[neuron].forwardsWeights.push_back(new Weight());
        lastLayer->neurons[neuron].forwardsWeights[0]->begginingNeuron = &(lastLayer->neurons[neuron]);
    }
}

void Model::randomizeNeuronWeights(double min, double max)
{
    for (int weight = 0, size = allWeights.size(); weight < size; weight++)
    {
        allWeights[weight]->weight = min + ((double)rand() / RAND_MAX) * (max - min);
    }
}

void Model::randomizeNeuronWeightsChoice(std::vector<double> choices)
{
    for (int weight = 0, size = allWeights.size(); weight < size; weight++)
    {
        allWeights[weight]->weight = choices[rand() % choices.size()];
    }
}

void Model::mutate(Model &sourceModel, int percent, std::vector<double> choices)
{
    for (int weight = 0, size = allWeights.size(); weight < size; weight++)
    {
        if (rand() % 100 < percent)
        {
            allWeights[weight]->weight = choices[rand() % choices.size()];
        }
        else
        {
            allWeights[weight]->weight = sourceModel.allWeights[weight]->weight;
        }
    }
}

void Model::mutate(Model &sourceModel, int percent, double min, double max)
{
    for (int weight = 0, size = allWeights.size(); weight < size; weight++)
    {
        if (rand() % 100 < percent)
        {
            allWeights[weight]->weight = min + ((double)rand() / RAND_MAX) * (max - min);
        }
        else
        {
            allWeights[weight]->weight = sourceModel.allWeights[weight]->weight;
        }
    }
}

void Model::mutate(int percent, double min, double max)
{
    for (int weight = 0, size = allWeights.size(); weight < size; weight++)
    {
        if (rand() % 100 < percent)
        {
            allWeights[weight]->weight = min + ((double)rand() / RAND_MAX) * (max - min);
        }
    }
}

void Model::mutateChange(Model &sourceModel, int percent, double min, double max)
{
    for (int weight = 0, size = allWeights.size(); weight < size; weight++)
    {
        if (rand() % 100 < percent)
        {
            // allWeights[weight]->weight += min + ((double)rand() / RAND_MAX) * (max - min);
            allWeights[weight]->weight = sourceModel.allWeights[weight]->weight + (min + ((double)rand() / RAND_MAX) * (max - min));
        }
        else
        {
            allWeights[weight]->weight = sourceModel.allWeights[weight]->weight;
        }
    }
}

void Model::mutateChange(int percent, double min, double max)
{
    for (int weight = 0, size = allWeights.size(); weight < size; weight++)
    {
        if (rand() % 100 < percent)
        {
            allWeights[weight]->weight += (min + ((double)rand() / RAND_MAX) * (max - min));
        }

    }
}

void Model::breed(Model &parent1, Model &parent2)
{
    for (int weight = 0, size = allWeights.size(); weight < size; weight++)
    {
        if (rand() % 2 == 0)
        {
            allWeights[weight]->weight = parent1.allWeights[weight]->weight;
        }
        else
        {
            allWeights[weight]->weight = parent2.allWeights[weight]->weight;
        }
    }
}

void Model::step(double *input)
{
    for (int neuron = 0; neuron < layers[0].neuronCount; neuron++)
    {
        layers[0].neurons[neuron].activation = input[neuron];
    }
    for (int index = 1; index < layerCount; index++)
    {
        layers[index].step();
    }
}

void Model::step(int *input)
{
    for (int neuron = 0; neuron < layers[0].neuronCount; neuron++)
    {
        layers[0].neurons[neuron].activation = input[neuron];
    }
    for (int index = 1; index < layerCount; index++)
    {
        layers[index].step();
    }
}

void Model::step(std::vector<double> input)
{
    for (int neuron = 0; neuron < layers[0].neuronCount; neuron++)
    {
        layers[0].neurons[neuron].activation = input[neuron];
    }
    for (int index = 1; index < layerCount; index++)
    {

        layers[index].step();
    }
}

void Model::step(MyArr<double> input)
{
    for (int neuron = 0; neuron < layers[0].neuronCount; neuron++)
    {
        layers[0].neurons[neuron].activation = input[neuron];
    }
    for (int index = 1; index < layerCount; index++)
    {
        layers[index].step();
    }
}

void Model::print()
{
    for (int index = 0; index < layerCount; index++)
    {
        layers[index].print();
    }
}

void Model::SGDRec(Weight *weight, double previousChain, double &derivative)
{
    if (weight->endNeuron->forwardsWeights[0]->endNeuron == nullptr)
    {
        double funcDer = weight->endNeuron->activationFunctionPrime(weight->endNeuron->value);
        double costDer = (weight->endNeuron->activation - weight->endNeuron->GOAL); // derivative of cost function
        derivative = previousChain * costDer * funcDer;
    }
    else
    {
        for (int nextWeight = 0; nextWeight < weight->endNeuron->forwardsWeights.size(); nextWeight++)
        {
            double funcDer = weight->endNeuron->activationFunctionPrime(weight->endNeuron->value);
            double chainMul = weight->endNeuron->forwardsWeights[nextWeight]->weight;
            SGDRec(weight->endNeuron->forwardsWeights[nextWeight], chainMul * previousChain * funcDer, derivative);
        }
    }
}

void Model::SGDWeight(Weight *weight)
{
    double input = weight->begginingNeuron->activation;
    double derivative = 0;

    SGDRec(weight, input, derivative);

    if (derivative > 1)
    {
        derivative = 1;
    }
    else if (derivative < -1)
    {
        derivative = -1;
    }
    else
    {
        derivative = std::ceil(derivative * 1000) / 1000; // round to nearest thousandth
    }

    weight->weightChange -= derivative * LR;
}

void Model::SGDBias(Neuron &neuron)
{
    double input = 1;
    double derivative = 0;

    SGDRec(neuron.backwardsWeights[0], input, derivative);

    if (derivative > 1)
    {
        derivative = 1;
    }
    else if (derivative < -1)
    {
        derivative = -1;
    }
    else
    {
        derivative = std::ceil(derivative * 1000) / 1000; // round to nearest thousandth
    }
    neuron.bias -= derivative * LR;
}

void Model::SGDWeightsAndApply(std::vector<double> goal)
{
    for (int neuron = 0; neuron < lastLayer->neuronCount; neuron++)
    {
        lastLayer->neurons[neuron].GOAL = goal[neuron];
    }

    for (int layer = 1; layer < layerCount; layer++)
    {
        for (int neuron = 0; neuron < layers[layer].neuronCount; neuron++)
        {
            for (int weight = 0; weight < layers[layer].neurons[neuron].backwardsWeights.size(); weight++)
            {
                SGDWeight(layers[layer].neurons[neuron].backwardsWeights[weight]);
                layers[layer].neurons[neuron].backwardsWeights[weight]->weight += layers[layer].neurons[neuron].backwardsWeights[weight]->weightChange;
                layers[layer].neurons[neuron].backwardsWeights[weight]->weightChange = 0;
            }
        }
    }
}

// TODO APPLY OPTIMIZE

// TODO BETTER NEGATIVE REWARD CHANGE
// TODO REWARD SCALES
void Model::SGDRewardWeights(int output, int reward)
{
    std::vector<double> goal(layers[layerCount - 1].neuronCount);

    if (reward == 1)
    {
        for (int neuron = 0; neuron < layers[layerCount - 1].neuronCount; neuron++)
        {
            goal[neuron] = 0;
        }
        goal[output] = 1;
        SGDWeightsAndApply(goal);
    }
    else if (reward == -1)
    {
        for (int neuron = 0; neuron < layers[layerCount - 1].neuronCount; neuron++)
        {
            goal[neuron] = layers[layerCount - 1].neurons[neuron].activation;
        }
        goal[output] = 0;
        SGDWeightsAndApply(goal);
    }
}

// TODO BETTER NEGATIVE REWARD CHANGE
// TODO REWARD SCALES
void Model::SGDRewardWeightsBatch(std::vector<std::vector<double>> input, std::vector<int> outputs, int reward)
{
    std::vector<double> goal(lastLayer->neuronCount);
    for (int index = 0; index < input.size(); index++)
    {
        step(input[index]);

        if (reward == 1)
        {
            for (int neuron = 0; neuron < layers[layerCount - 1].neuronCount; neuron++)
            {
                goal[neuron] = 0;
            }
            goal[outputs[index]] = 1;
            SGDWeightsAndApply(goal);
        }
        else if (reward == -1)
        {
            for (int neuron = 0; neuron < layers[layerCount - 1].neuronCount; neuron++)
            {
                // goal[neuron] = layers[layerCount - 1].neurons[neuron].activation;
                goal[neuron] = 1;
            }
            goal[outputs[index]] = 0;
            SGDWeightsAndApply(goal);
        }
    }
}

int Model::getHighestOutputIndex()
{
    int maxNeuron = 0;
    for (int neuron = 1; neuron < layers[layerCount - 1].neuronCount; neuron++)
    {
        if (layers[layerCount - 1].neurons[neuron].activation > layers[layerCount - 1].neurons[maxNeuron].activation)
        {
            maxNeuron = neuron;
        }
    }
    return maxNeuron;
}

double Model::getHighestOutput()
{
    return lastLayer->neurons[getHighestOutputIndex()].activation;
}

std::vector<double> Model::getOutputVector()
{
    std::vector<double> ret;
    for (int neuron = 0; neuron < lastLayer->neuronCount; neuron++)
    {
        ret.push_back(lastLayer->neurons[neuron].activation);
    }
    return ret;
}

void Model::changeActivation(int layer, double (*func)(double num))
{
    for (int index = 0; index < layers[layer].neuronCount; index++)
    {
        layers[layer].neurons[index].activationFunction = func;
    }
}

void Model::changeActivationPrime(int layer, double (*func)(double num))
{
    for (int index = 0; index < layers[layer].neuronCount; index++)
    {
        layers[layer].neurons[index].activationFunctionPrime = func;
    }
}

// TODO CHANGE WIRING
// TODO CHANGE LAYER AND NEURON LAYOUT
// TODO CHANGE ACTIVATION FUNCTION
// TODO CHANGE UPDATE
void Model::operator=(const Model &model)
{
    for (int weight = 0, size = allWeights.size(); weight < size; weight++)
    {
        allWeights[weight]->weight = model.allWeights[weight]->weight;
    }
}

/* #endregion */

/* #region GeneticModel */

GeneticModel::GeneticModel()
{
}

GeneticModel::GeneticModel(std::vector<int> modelShape, int threads, double (*scoreFunction)(Model &model))
{
    this->modelShape = modelShape;
    this->scoreFunction = scoreFunction;

    baseModel.setup(modelShape);
    for (int index = 1, size = modelShape.size(); index < size; index++)
    {
        baseModel.changeActivation(index, step);
    }
    baseModel.defaultWiring();
    // TODO MAKE DYNAMIC
    baseModel.randomizeNeuronWeights(-1, 1);
}

GeneticModel::~GeneticModel()
{
}

void GeneticModel::runGenerations(int generationCount, int generationSize)
{
    if (generationCount == 0)
    {
        generationCount = INFINITY;
    }

    double bestScore = 0;
    int bestIndex = 0;

    Model batch[generationSize];
    for (int model = 0; model < generationSize; model++)
    {
        batch[model].setup(modelShape);
        for (int index = 1, size = modelShape.size(); index < size; index++)
        {
            batch[model].changeActivation(index, step);
        }
        batch[model].defaultWiring();
    }

    for (int gen = 1; gen <= generationCount; gen++)
    {
        {
            batch[0] = baseModel;

            const double score = scoreFunction(batch[0]);

            if (score > bestScore)
            {
                bestScore = score;
                bestIndex = 0;
            }
        }
        for (int model = 1; model < generationSize; model++)
        {
            // batch[model].mutate(baseModel, 8, {-1, 0, 1});
            // batch[model].mutate(baseModel, 10, -1, 1);
            batch[model].mutateChange(baseModel, 10, -1, 1);

            // double score = 0;

            const double score = scoreFunction(batch[model]);

            if (score > bestScore)
            {
                bestScore = score;
                bestIndex = model;
            }
        }

        generation++;
        std::cout << "bestScore: " << bestScore << " generation: " << gen << std::endl;
        baseModel = batch[bestIndex];
        // if (bestScore == runCount)
        // {
        //     break;
        // }
        bestIndex = 0;
        bestScore = 0;
    }
}

void GeneticModel::runGenerations2Parents(int generationCount, int generationSize)
{
    if (generationCount == 0)
    {
        generationCount = INFINITY;
    }

    Model parent1(modelShape);
    Model parent2(modelShape);

    parent1.setup(modelShape);
    parent2.setup(modelShape);
    for (int index = 1, size = modelShape.size(); index < size; index++)
    {
        baseModel.changeActivation(index, step);
        parent2.changeActivation(index, step);
    }
    parent1.defaultWiring();
    parent1.randomizeNeuronWeights(-1, 1);
    parent2.defaultWiring();

    double bestScore = 0;

    Model batch[generationSize];
    for (int model = 0; model < generationSize; model++)
    {
        batch[model].setup(modelShape);
        for (int index = 1, size = modelShape.size(); index < size; index++)
        {
            batch[model].changeActivation(index, step);
        }
        batch[model].defaultWiring();
        batch[model].randomizeNeuronWeights(-1, 1);
    }

    // get first 2 parents;
    for (int model = 0; model < generationSize; model++)
    {
        const double score = scoreFunction(batch[model]);

        if (score >= bestScore)
        {
            bestScore = score;
            parent2 = parent1;
            parent1 = batch[model];
        }
    }
    generation++;

    for (int gen = 2; gen <= generationCount; gen++)
    {
        for (int model = 0; model < generationSize; model++)
        {
            batch[model].breed(parent1, parent2);
            // batch[model].mutate(5, -1, 1);
            batch[model].mutateChange(10, -1, 1);
            const double score = scoreFunction(batch[model]);

            if (score >= bestScore)
            {
                bestScore = score;
                parent2 = parent1;
                parent1 = batch[model];
            }
        }

        generation++;
        std::cout << "bestScore: " << bestScore << " generation: " << gen << std::endl;
        bestScore = 0;
    }
}

void GeneticModel::genCuda(int generationCount, int generationSize)
{
    
}

/* #endregion */