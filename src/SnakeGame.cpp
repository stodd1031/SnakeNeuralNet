#include "SnakeGame.hpp"

/* #region  SnakeGame */

SnakeGame::SnakeGame(sf::RenderWindow *window)
{
    this->window = window;

    apple.newLocation(snake);
}

void SnakeGame::nextFrameHuman()
{
    sf::Event event;
    int keys = 0;
    if (snake.getNextDirection() > -1)
    {
        snake.setDirection(snake.getNextDirection());
        snake.setNextDirection(-1);
    }
    while (window->pollEvent(event))
    {
        switch (event.type)
        {
        // window closed
        case sf::Event::Closed:
            window->close();
            running = false;
            break;

        // key pressed
        case sf::Event::KeyPressed:
        {
            int newDirection = snake.getDirection();
            switch (event.key.code)
            {
            case sf::Keyboard::Up:
                newDirection = UP;
                keys++;
                break;
            case sf::Keyboard::Right:
                newDirection = RIGHT;
                keys++;
                break;
            case sf::Keyboard::Down:
                newDirection = DOWN;
                keys++;
                break;
            case sf::Keyboard::Left:
                newDirection = LEFT;
                keys++;
                break;
            case sf::Keyboard::W:
                newDirection = UP;
                keys++;
                break;
            case sf::Keyboard::D:
                newDirection = RIGHT;
                keys++;
                break;
            case sf::Keyboard::S:
                newDirection = DOWN;
                keys++;
                break;
            case sf::Keyboard::A:
                newDirection = LEFT;
                keys++;
                break;
            }
            if (keys > 1 && (snake.getDirection() + 2) % 4 != newDirection)
            {
                snake.setNextDirection(newDirection);
            }
            else if (newDirection != snake.getDirection() && (snake.getDirection() + 2) % 4 != newDirection || snake.getDirection() < 0)
            {
                snake.setDirection(newDirection);
            }
            else
            {
                keys--;
            }
            break;
        }
        }
    }
    window->clear();
    snake.move();

    if (snake.collide())
    {
        std::cout << "collide" << std::endl;
        // window->close();
        running = false;
    }

    if (snake.eatingApple(apple))
    {
        snake.newSeg();
        for (int index = 0; index < 30; index++)
        {
            snake.newSeg();
        }
        if (snake.getLength() >= BLOCK_COUNT_X * BLOCK_COUNT_Y)
        {
            // win
            // window->close();
            running = false;
        }
        apple.newLocation(snake);
    }
    apple.draw(window);
    snake.draw(window);
    window->display();
}

void SnakeGame::nextFrameAI(int direction)
{
    if (window)
    {
        sf::Event event;
        while (window->pollEvent(event))
        {
            switch (event.type)
            {
            // window closed
            case sf::Event::Closed:
                window->close();
                running = false;
                break;
            case sf::Event::KeyPressed:
            {
                if (event.key.code == sf::Keyboard::Space)
                {
                    if (slow)
                    {
                        window->setFramerateLimit(0);
                        slow = !slow;
                    }
                    else
                    {
                        window->setFramerateLimit(8);
                        slow = !slow;
                    }
                }
                break;
            }
            }
        }
    }

    snake.setDirection(direction);

    reward = 0;

    switch (snake.getDirection())
    {
    case LEFT:
        if (apple.getCoord().x < snake.getHead().getCoord().x)
        {
            reward = REWARD_TOWARDS_APPLE;
        }
        break;
    case RIGHT:
        if (apple.getCoord().x > snake.getHead().getCoord().x)
        {
            reward = REWARD_TOWARDS_APPLE;
        }
        break;
    case DOWN:
        if (apple.getCoord().y > snake.getHead().getCoord().y)
        {
            reward = REWARD_TOWARDS_APPLE;
        }
        break;
    case UP:
        if (apple.getCoord().y < snake.getHead().getCoord().y)
        {
            reward = REWARD_TOWARDS_APPLE;
        }
        break;
    }

    snake.move();

    if (snake.collide())
    {
        // std::cout << "collide" << std::endl;
        // window->close();
        running = false;
        reward = REWARD_DIE;
    }

    movesSinceApple++;
    if (snake.eatingApple(apple))
    {
        movesSinceApple = 0;
        reward = REWARD_EAT_APPLE;
        snake.newSeg();

        if (snake.getLength() >= (BLOCK_COUNT_X * BLOCK_COUNT_Y))
        {
            if (snake.getHead().getCoord().x == snake.getTail()->getCoord().x)
            {
                if (abs(snake.getHead().getCoord().y - snake.getTail()->getCoord().y) == 1)
                {
                    // win
                    reward = REWARD_WIN;
                }
            }
            else if (snake.getHead().getCoord().y == snake.getTail()->getCoord().y)
            {
                if (abs(snake.getHead().getCoord().x - snake.getTail()->getCoord().x) == 1)
                {
                    // win
                    reward = REWARD_WIN;
                }
            }
            else
            {
                // bad move, next move will not be on tail
                reward = REWARD_EAT_APPLE;
                // reward = REWARD_DIE;
            }

            // window->close();
            running = false;
        }
        else
        {
            apple.newLocation(snake);
        }
    }
    else if (movesSinceApple >= BLOCK_COUNT_X * BLOCK_COUNT_Y)
    {
        movesSinceApple = 0;
        reward = REWARD_LOOP;
        running = false;
    }
    if (window)
    {
        window->clear();
        apple.draw(window);
        snake.draw(window);
        window->display();
    }
}

int SnakeGame::getReward()
{
    return reward;
}

void SnakeGame::getBoard(double *gameBoard)
{
    /*
    for (int index = 0; index < BOARD_SIZE; index++)
    {
        gameBoard[index] = 0;
    }
    Coord snakeHeadCoord = snake.getHead().getCoord();
    gameBoard[UP * OPTION_COUNT + WALL] = snakeHeadCoord.y;
    gameBoard[DOWN * OPTION_COUNT + WALL] = BLOCK_COUNT_Y - 1 - snakeHeadCoord.y;
    gameBoard[LEFT * OPTION_COUNT + WALL] = snakeHeadCoord.x;
    gameBoard[RIGHT * OPTION_COUNT + WALL] = BLOCK_COUNT_X - 1 - snakeHeadCoord.x;

    if (apple.getCoord().x == snakeHeadCoord.x)
    {
        if (apple.getCoord().y > snakeHeadCoord.y)
        {
            gameBoard[UP * OPTION_COUNT + FOOD] = 1;
        }
        else
        {
            gameBoard[DOWN * OPTION_COUNT + FOOD] = 1;
        }
    }
    else if (apple.getCoord().y == snakeHeadCoord.y)
    {
        if (apple.getCoord().x > snakeHeadCoord.x)
        {
            gameBoard[RIGHT * OPTION_COUNT + FOOD] = 1;
        }
        else
        {
            gameBoard[LEFT * OPTION_COUNT + FOOD] = 1;
        }
    }

    SnakeSeg *seg = snake.getTail();

    while (seg->frontSeg)
    {
        if (seg->getCoord().x == snakeHeadCoord.x)
        {
            if (seg->getCoord().y > snakeHeadCoord.y)
            {
                gameBoard[UP * OPTION_COUNT + SNAKE_BODY] = 1;
            }
            else
            {
                gameBoard[DOWN * OPTION_COUNT + SNAKE_BODY] = 1;
            }
        }
        else if (seg->getCoord().y == snakeHeadCoord.y)
        {
            if (seg->getCoord().x > snakeHeadCoord.x)
            {
                gameBoard[RIGHT * OPTION_COUNT + SNAKE_BODY] = 1;
            }
            else
            {
                gameBoard[LEFT * OPTION_COUNT + SNAKE_BODY] = 1;
            }
        }
        seg= seg->frontSeg;
    }
    */

    for (int index = 0; index < BOARD_SIZE; index++)
    {
        gameBoard[index] = 0;
    }

    Coord snakeHeadCoord = snake.getHead().getCoord();

    // set outside board as death block
    for (int x = 0; x < BOARD_X; x++)
    {
        for (int y = 0; y < BOARD_Y; y++)
        {
            if (snakeHeadCoord.x - VISION_LENGTH_X + x < 0 || snakeHeadCoord.y - VISION_LENGTH_Y + y < 0 ||
                snakeHeadCoord.x - VISION_LENGTH_X + x > BLOCK_COUNT_X - 1 || snakeHeadCoord.y - VISION_LENGTH_Y + y > BLOCK_COUNT_Y - 1)
            {
                gameBoard[x * BOARD_Y * OPTION_COUNT + y * OPTION_COUNT + DEATH] = 1;
            }
        }
    }

    // set apple as apple
    gameBoard[(VISION_LENGTH_X - snakeHeadCoord.x + apple.getCoord().x) * BOARD_Y * OPTION_COUNT +
              (VISION_LENGTH_Y - snakeHeadCoord.y + apple.getCoord().y) * OPTION_COUNT +
              FOOD] = 1;

    // set all but head snake blocks as death

    SnakeSeg *seg = snake.getTail();

    while (seg->frontSeg)
    {
        gameBoard[(VISION_LENGTH_X - snakeHeadCoord.x + seg->getCoord().x) * BOARD_Y * OPTION_COUNT +
                  (VISION_LENGTH_Y - snakeHeadCoord.y + seg->getCoord().y) * OPTION_COUNT +
                  DEATH] = 1;
        seg = seg->frontSeg;
    }

    // set head as head

    // gameBoard[VISION_LENGTH_X * BOARD_Y * OPTION_COUNT + VISION_LENGTH_Y * OPTION_COUNT + HEAD] = 1;
    gameBoard[VISION_LENGTH_X * BOARD_Y * OPTION_COUNT + VISION_LENGTH_Y * OPTION_COUNT + DEATH] = 0;

    // gameBoard[BOARD_Y * OPTION_COUNT * BOARD_X + snake.getDirection()] = 1;
}

void SnakeGame::getBoard(MyArr<double> &gameBoard)
{

    for (int index = 0; index < BOARD_SIZE; index++)
    {
        gameBoard[index] = 0;
    }

    Coord snakeHeadCoord = snake.getHead().getCoord();

    // set outside board as death block
    for (int x = 0; x < BOARD_X; x++)
    {
        for (int y = 0; y < BOARD_Y; y++)
        {
            if (snakeHeadCoord.x - VISION_LENGTH_X + x < 0 || snakeHeadCoord.y - VISION_LENGTH_Y + y < 0 ||
                snakeHeadCoord.x - VISION_LENGTH_X + x > BLOCK_COUNT_X - 1 || snakeHeadCoord.y - VISION_LENGTH_Y + y > BLOCK_COUNT_Y - 1)
            {
                gameBoard[x * BOARD_Y * OPTION_COUNT + y * OPTION_COUNT + DEATH] = 1;
            }
        }
    }

    // set apple as apple
    gameBoard[(VISION_LENGTH_X - snakeHeadCoord.x + apple.getCoord().x) * BOARD_Y * OPTION_COUNT +
              (VISION_LENGTH_Y - snakeHeadCoord.y + apple.getCoord().y) * OPTION_COUNT +
              FOOD] = 1;

    SnakeSeg *seg = snake.getTail();

    // set snake tail
    // gameBoard[(VISION_LENGTH_X - snakeHeadCoord.x + seg->getCoord().x) * BOARD_Y * OPTION_COUNT +
    //           (VISION_LENGTH_Y - snakeHeadCoord.y + seg->getCoord().y) * OPTION_COUNT +
    //           TAIL] = 1;

    seg = seg->frontSeg;

    // set all but head snake blocks as death
    if (seg)
    {
        while (seg->frontSeg)
        {
            gameBoard[(VISION_LENGTH_X - snakeHeadCoord.x + seg->getCoord().x) * BOARD_Y * OPTION_COUNT +
                      (VISION_LENGTH_Y - snakeHeadCoord.y + seg->getCoord().y) * OPTION_COUNT +
                      DEATH] = 1;
            seg = seg->frontSeg;
        }
    }

    // set head as head

    // empty center space(head) of and tail
    gameBoard[VISION_LENGTH_X * BOARD_Y * OPTION_COUNT + VISION_LENGTH_Y * OPTION_COUNT + DEATH] = 0;
    // gameBoard[VISION_LENGTH_X * BOARD_Y * OPTION_COUNT + VISION_LENGTH_Y * OPTION_COUNT + TAIL] = 0;

    // gameBoard[BOARD_Y * OPTION_COUNT * BOARD_X + snake.getDirection()] = 1;
}

void SnakeGame::setWindow(sf::RenderWindow *window)
{
    this->window = window;
}

// TODO MAKE MORE OOP
void SnakeGame::reset()
{
    running = true;
    snake.reset();
    movesSinceApple = 0;

    apple.newLocation(snake);
}

int SnakeGame::getScore()
{
    return snake.getLength();
}

/* #endregion */

/* #region  Snake */

Snake::Snake()
{
    tail = &head;
    direction = STATIC;
}

void Snake::draw(sf::RenderWindow *window)
{
    SnakeSeg *seg = tail;
    while (seg)
    {
        seg->draw(window);
        seg = seg->frontSeg;
    }
}

int Snake::getLength()
{
    return snakeLen;
}

void Snake::setDirection(int direction)
{
    this->direction = direction;
}

int Snake::getDirection()
{
    return direction;
}

void Snake::move()
{
    SnakeSeg *seg = tail;
    if (seg->frontSeg)
    {
        while (seg->frontSeg->frontSeg)
        {
            seg->changeSize(seg->frontSeg->getSize());
            seg->moveSeg(seg->frontSeg->getPosition());
            seg->setCoord(seg->frontSeg->getCoord());
            seg = seg->frontSeg;
        }
        sf::Vector2f headPos = seg->frontSeg->getPosition();
        switch (direction)
        {
        case UP:
            seg->changeSize(sf::Vector2f(BLOCKSIZE - (2 * SNAKE_BORDER), BLOCKSIZE));
            headPos.y -= SNAKE_BORDER * 2;
            break;
        case RIGHT:
            seg->changeSize(sf::Vector2f(BLOCKSIZE, BLOCKSIZE - (2 * SNAKE_BORDER)));
            break;
        case DOWN:
            seg->changeSize(sf::Vector2f(BLOCKSIZE - (2 * SNAKE_BORDER), BLOCKSIZE));
            break;
        case LEFT:
            seg->changeSize(sf::Vector2f(BLOCKSIZE, BLOCKSIZE - (2 * SNAKE_BORDER)));
            headPos.x -= SNAKE_BORDER * 2;
            break;
        }
        seg->moveSeg(headPos);
        seg->setCoord(seg->frontSeg->getCoord());
        seg = seg->frontSeg;
    }
    sf::Vector2f pos = seg->getPosition();
    Coord coord = head.getCoord();
    switch (direction)
    {
    case UP:
        pos.y -= BLOCKSIZE;
        coord.y--;
        break;
    case RIGHT:
        pos.x += BLOCKSIZE;
        coord.x++;
        break;
    case DOWN:
        pos.y += BLOCKSIZE;
        coord.y++;
        break;
    case LEFT:
        pos.x -= BLOCKSIZE;
        coord.x--;
        break;
    }
    seg->moveSeg(pos);
    seg->setCoord(coord);
}

bool Snake::collide()
{
    sf::Vector2f headPos = head.getPosition();
    SnakeSeg *seg = tail;
    sf::Vector2f segPos;
    while (seg->frontSeg)
    {
        segPos = seg->getPosition();

        if (seg->getCoord() == head.getCoord())
        {
            return 1;
        }
        seg = seg->frontSeg;
    }
    if (headPos.x < 0)
    {
        return 1;
    }
    if (headPos.x > SCREEN_WIDTH)
    {
        return 1;
    }
    if (headPos.y < 0)
    {
        return 1;
    }
    if (headPos.y > SCREEN_HEIGHT)
    {
        return 1;
    }
    return 0;
}

void Snake::newSeg()
{
    tail = new SnakeSeg(tail);
    snakeLen++;
}

void Snake::setNextDirection(int direction)
{
    nextDirection = direction;
}

int Snake::getNextDirection()
{
    return nextDirection;
}

bool Snake::eatingApple(Apple apple)
{
    sf::Vector2f applePos = apple.getPosition();
    sf::Vector2f headPos = head.getPosition();
    if ((long)applePos.x / BLOCKSIZE == (long)headPos.x / BLOCKSIZE &&
        (long)applePos.y / BLOCKSIZE == (long)headPos.y / BLOCKSIZE)
    {
        return 1;
    }
    return 0;
}

void Snake::reset()
{

    SnakeSeg *seg = tail;
    SnakeSeg *next = tail->frontSeg;
    while (next)
    {
        delete seg;
        seg = next;
        next = next->frontSeg;
    }
    tail = &head;
    snakeLen = 1;
    direction = STATIC;
    nextDirection = -1;

    head.reset();
}

/* #endregion */

/* #region  Apple */

void Apple::newLocation(Snake &snake)
{

    bool spaces[(BLOCK_COUNT_X)][(BLOCK_COUNT_Y)];

    for (int index = 0; index < (BLOCK_COUNT_X); index++)
    {
        memset(spaces[index], 0, (BLOCK_COUNT_Y));
    }

    int freeSpace = (BLOCK_COUNT_X * BLOCK_COUNT_Y) - snake.getLength();

    SnakeSeg *seg = snake.getTail();

    while (seg)
    {
        spaces[seg->getCoord().x][seg->getCoord().y] = 1;
        seg = seg->frontSeg;
    }

    int choice = rand() % freeSpace + 1;
    for (int x = 0; x < BLOCK_COUNT_X; x++)
    {
        for (int y = 0; y < BLOCK_COUNT_Y; y++)
        {
            if (!spaces[x][y]) // if block is empty
            {
                choice--;
            }
            if (choice == 0)
            {

                moveApple(x, y);

                return;
            }
        }
    }
}

void Apple::moveApple(int blockx, int blocky)
{
    apple.setPosition(sf::Vector2f(blockx * BLOCKSIZE + 1, blocky * BLOCKSIZE + 1));
    coord.x = blockx;
    coord.y = blocky;
}

Apple::Apple()
{
    apple = sf::RectangleShape(sf::Vector2f(BLOCKSIZE - 2, BLOCKSIZE - 2));
    apple.setFillColor(sf::Color::Red);
    apple.setPosition(sf::Vector2f(1, 1));
}

void Apple::draw(sf::RenderWindow *window)
{
    window->draw(apple);
}

sf::Vector2f Apple::getPosition()
{
    return apple.getPosition();
}

Coord Apple::getCoord()
{
    return coord;
}

void Apple::setCoord(Coord coord)
{
    this->coord = coord;
}

/* #endregion */

/* #region  SnakeSeg */

SnakeSeg *Snake::getTail()
{
    return tail;
}

SnakeSeg Snake::getHead()
{
    return head;
}

SnakeSeg::SnakeSeg()
{
    seg = sf::RectangleShape(sf::Vector2f(BLOCKSIZE - SNAKE_BORDER * 2, BLOCKSIZE - SNAKE_BORDER * 2));
    seg.setFillColor(sf::Color::Green);
    moveSeg(sf::Vector2f(SCREEN_WIDTH / 2 + SNAKE_BORDER, SCREEN_HEIGHT / 2 + SNAKE_BORDER));
    coord.x = (long)seg.getPosition().x / BLOCKSIZE;
    coord.y = (long)seg.getPosition().y / BLOCKSIZE;
}

SnakeSeg::SnakeSeg(SnakeSeg *tail)
{
    seg = sf::RectangleShape(sf::Vector2f(BLOCKSIZE - SNAKE_BORDER * 2, BLOCKSIZE - SNAKE_BORDER * 2));
    seg.setFillColor(sf::Color::Green);
    moveSeg(tail->getPosition());
    coord = tail->getCoord();
    frontSeg = tail;
}

void SnakeSeg::moveSeg(sf::Vector2f loc)
{
    seg.setPosition(loc);
}

sf::Vector2f SnakeSeg::getPosition()
{
    return seg.getPosition();
}

void SnakeSeg::draw(sf::RenderWindow *window)
{
    window->draw(seg);
}

sf::Vector2f SnakeSeg::getSize()
{
    return seg.getSize();
}

void SnakeSeg::changeSize(sf::Vector2f size)
{
    seg.setSize(size);
}

Coord SnakeSeg::getCoord()
{
    return coord;
}

void SnakeSeg::setCoord(Coord coord)
{
    this->coord = coord;
}

void SnakeSeg::reset()
{
    // seg = sf::RectangleShape(sf::Vector2f(BLOCKSIZE - SNAKE_BORDER * 2, BLOCKSIZE - SNAKE_BORDER * 2));
    // seg.setFillColor(sf::Color::Green);
    moveSeg(sf::Vector2f(SCREEN_WIDTH / 2 + SNAKE_BORDER, SCREEN_HEIGHT / 2 + SNAKE_BORDER));
    coord.x = (long)seg.getPosition().x / BLOCKSIZE;
    coord.y = (long)seg.getPosition().y / BLOCKSIZE;
}

/* #endregion */
