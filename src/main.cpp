#include "SnakeGame.hpp"
#include "network.hpp"

#include <sys/time.h>

#define MAX_MOVES 136

class timer
{
    timeval t1;
    timeval t2;

public:
    timer()
    {
        gettimeofday(&t1, NULL);
    }
    ~timer()
    {
        gettimeofday(&t2, NULL);
        std::cout << std::fixed << std::setprecision(6) << ((t2.tv_usec + t2.tv_sec * 1000000) - (t1.tv_usec + t1.tv_sec * 1000000)) / 1000000.0 << std::endl;
    }
};

sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "SFML window");
SnakeGame game(nullptr);

double func(Model &model)
{
    double score = 0;

    MyArr<double> gameBoard(BOARD_SIZE);

    // int moves = 0;

    for (int gameNum = 0; gameNum < 100; gameNum++)
    // while(moves < 5000)
    {
        game.reset();

        while (game.running)
        {
            // moves++;
            game.getBoard(gameBoard);

            model.step(gameBoard);

            game.nextFrameAI(model.getHighestOutputIndex());
        }

        // score += (5* (game.getScore()-1.0 )) + game.getReward();
        score += game.getScore() + game.getReward();
    }
    return score;
}

int main()
{
    srand(time(NULL));

    GeneticModel gm({BOARD_SIZE, 4}, 1, func);
    gm.runGenerations(100, 500);
    // gm.runGenerations2Parents(0, 1000);
    game.setWindow(&window);
    // gm.runGenerations2Parents(0, 1000);
    gm.runGenerations(0, 500);



    // Model nn({BOARD_SIZE, 4});
    // nn.defaultWiring();

    // MyArr<double> vec(nn.firstLayer->neuronCount);
    // nn.randomizeNeuronWeights(-1, 1);

    // for (int index = 0; index < vec.size(); index++)
    // {
    //     vec[index] = ((double)rand() / RAND_MAX) * (2) - 1;
    // }

    // timer t;

    // for (int index = 0; index < 100 * 500; index++)
    // {
    //     timer gfd;
    //     nn.step(vec);
    // }


    /*
        // window.setFramerateLimit(8);

        SnakeGame game(&window);

        int modelSize[] = {BOARD_SIZE, 4};

        Model model(modelSize, 2);

        // model.changeActivation(1, relu);
        // model.changeActivationPrime(1, reluPrime);
        // model.changeActivation(1, sigmoid);
        // model.changeActivationPrime(1, sigmoidPrime);
        // model.changeActivation(1, relu);
        // model.changeActivationPrime(1, reluPrime);
        model.changeActivation(1, sigmoid);
        model.changeActivationPrime(1, sigmoidPrime);

        std::vector<double> gameBoard(BOARD_SIZE);
        // double *gameBoard = new double[BOARD_SIZE];

    #define EPSILON 100000
        unsigned long epsilon = EPSILON;

        int scoreCount = 0;
        double average = 0;

        std::vector<std::vector<double>> boardsSinceApple;
        std::vector<int> actionsSinceApple;

        std::vector<std::vector<double>> boardsSinceStart;
        std::vector<int> actionsSinceStart;

        int games = 0;

        while (1)
        {
            while (game.running)
            {
                game.getBoard(gameBoard);
                boardsSinceApple.push_back(gameBoard);
                boardsSinceStart.push_back(gameBoard);

                model.step(gameBoard);

                int output = 0;
                if (rand() % EPSILON < epsilon)
                {
                    output = rand() % 4;
                }
                else
                {
                    output = model.getOutput();
                }
                actionsSinceApple.push_back(output);
                actionsSinceStart.push_back(output);
                game.nextFrameAI(output);

                if (game.getReward() == 2)
                {
                    model.optimize(boardsSinceStart, actionsSinceStart, 1);
                    model.optimize(boardsSinceStart, actionsSinceStart, 1);
                    boardsSinceStart.clear();
                    actionsSinceStart.clear();
                }
                else if (game.getReward() && games != 1000000)
                {
                    model.optimize(boardsSinceApple, actionsSinceApple, game.getReward());
                    boardsSinceApple.clear();
                    actionsSinceApple.clear();
                }
            }

            boardsSinceStart.clear();
            actionsSinceStart.clear();

            average = ((average * scoreCount) + game.getScore()) / ++scoreCount;

            if (games != 1000000);
            {
                games ++;
            }

            // if (epsilon != EPSILON / 100)
            if (epsilon != 0)
            {
                epsilon--;
                std::cout << "scr: " << game.getScore() << " eps: " << epsilon << " avg: " << average << std::endl;
            }
            else
            {
                std::cout << "scr: " << game.getScore() << " avg: " << average << std::endl;
            }

            // model.print();

            game.reset();
            if (scoreCount > 10000)
            {
                average = 0;
                scoreCount = 0;
            }
        }

    */
    /*
        sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "SFML window");

        int modelSize[] = {BOARD_SIZE, 20, 4};
        Model bestModel(modelSize, 3);
        double bestScore = 0;

        bestModel.changeActivation(2, sigmoid);
        bestModel.changeActivationPrime(2, sigmoidPrime);

        SnakeGame game(&window);

        double *gameBoard = new double[BOARD_SIZE];

        while (1)
        {
            // for (int index = 0; index++)
            Model newModel(modelSize, 3);
            newModel = bestModel;

            int steps = 0;
            while (steps < BLOCK_COUNT_X * BLOCK_COUNT_X * BLOCK_COUNT_Y * BLOCK_COUNT_Y && game.running)
            {

                game.getBoard(gameBoard);

                newModel.step(gameBoard);

                game.nextFrameAI(newModel.getOutput());

                steps++;

            }

            if (game.getScore() / steps > bestScore)
            {

                bestModel = newModel;

                bestScore = game.getScore() / steps;
                std::cout << game.getScore() << std::endl;

            }
            game.reset();
        }
        */
}