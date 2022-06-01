#pragma once

#include <SFML/Graphics.hpp>
#include <memory.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include "MyArr.h"

#define REWARD_TOWARDS_APPLE 0
#define REWARD_DIE -1
#define REWARD_EAT_APPLE 0
#define REWARD_WIN 0
#define REWARD_LOOP 0

#define BLOCKSIZE 40

#define BLOCK_COUNT_X 4
#define BLOCK_COUNT_Y 4

#define SNAKE_BORDER 4

#define SCREEN_WIDTH BLOCK_COUNT_X *BLOCKSIZE
#define SCREEN_HEIGHT BLOCK_COUNT_Y *BLOCKSIZE

#define STATIC -2
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

// RELATING TO GAME BOARD

#define DEATH 0
// #define WALL 0
#define FOOD 1
#define TAIL 2
// #define HEAD 2
// #define SNAKE_BODY 2

#define OPTION_COUNT 2

#define VISION_LENGTH_X (BLOCK_COUNT_X -1)
#define VISION_LENGTH_Y (BLOCK_COUNT_Y -1)

#define BOARD_X (BLOCK_COUNT_X * 2 - 1)
#define BOARD_Y (BLOCK_COUNT_Y * 2 - 1)

#define BOARD_SIZE (BOARD_X * BOARD_Y * OPTION_COUNT)
// #define BOARD_SIZE (BOARD_X * BOARD_Y * OPTION_COUNT+4)
// #define VISION_DIRECTIONS 4
// #define BOARD_SIZE (OPTION_COUNT * VISION_DIRECTIONS)


class Snake;
class Apple;

struct Coord
{
    int x;
    int y;
    bool operator==(Coord coord)
    {
        if (coord.x == x && coord.y == y)
        {
            return 1;
        }
        return 0;
    }
};

class SnakeSeg
{
    sf::RectangleShape seg;
    Coord coord;

public:
    SnakeSeg();
    SnakeSeg(SnakeSeg *tail);
    void moveSeg(sf::Vector2f loc);
    sf::Vector2f getPosition();
    Coord getCoord();
    void setCoord(Coord coord);
    void draw(sf::RenderWindow *window);
    SnakeSeg *frontSeg = nullptr;
    void changeSize(sf::Vector2f size);
    sf::Vector2f getSize();
    void reset();
};

class Snake
{
    int snakeLen = 1;
    SnakeSeg head;
    SnakeSeg *tail;
    int direction= 0 ;
    int nextDirection = -1;

public:
    Snake();
    void increaseLeangth();
    void draw(sf::RenderWindow *window);
    int getLength();
    SnakeSeg *getTail();
    void setDirection(int direction);
    int getDirection();
    void move();
    bool collide();
    SnakeSeg getHead();
    void newSeg();
    void setNextDirection(int direction);
    int getNextDirection();
    bool eatingApple(Apple apple);
    void reset();
};

class Apple
{
    Coord coord;
    sf::RectangleShape apple;

public:
    Apple();
    void moveApple(int blockx, int blocky);
    void draw(sf::RenderWindow *window);
    sf::Vector2f getPosition();
    void newLocation(Snake &snake);
    Coord getCoord();
    void setCoord(Coord coord);
};

class SnakeGame
{
    int reward = 0;
    sf::RenderWindow *window;
    Snake snake;
    Apple apple;
    bool slow = 0;
    int movesSinceApple = 0;

public:
    SnakeGame(sf::RenderWindow *window);
    void nextFrameHuman();
    void nextFrameAI(int direction);
    bool running = 1;
    int getReward();
    void getBoard(double *gameBoard);
    void getBoard(MyArr<double> &gameBoard);
    int getScore();
    void reset();
    void setWindow(sf::RenderWindow *window);
};