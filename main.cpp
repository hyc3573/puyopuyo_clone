#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <Windows.h>
#include <vector>
#include <stack>
#include <queue>
#include <utility>
#include <list>

#define SWIDTH 800
#define SHEIGHT 800

#define BWIDTH 6
#define BHEIGHT 10

#define MARGIN 1

#define FALLSPD 1

using namespace std;
using namespace sf;

typedef enum Square { BLANK, GREEN, YELLOW, RED, BLUE } Square;
Color colors[5] = { Color::Black, Color::Green, Color::Yellow, Color::Red, Color::Blue };

typedef enum PuyoType { HEAD, TAIL } PuyoType; // if rotated, head becomes rotation's center

class Board
{
private:
    vector<vector<Square>> board;

    stack<pair<int, int>> dfs(int X, int Y, bool visited[BWIDTH][BHEIGHT])
    {
        stack<pair<int, int>> result;
        stack<pair<int, int>> ohno;
        
        visited[X][Y] = true;

        if (X != 0 && (board[X - 1][Y] == board[X][Y] && !visited[X-1][Y]))
        {
            result = dfs(X - 1, Y, visited);
            while (!result.empty())
            {
                ohno.push(result.top());
                result.pop();
            }
        }
        if (X != BWIDTH - 1 && (board[X + 1][Y] == board[X][Y] && !visited[X+1][Y]))
        {
            result = (dfs(X + 1, Y, visited));
            while (!result.empty())
            {
                ohno.push(result.top());
                result.pop();
            }
        }
        if (Y != 0 && (board[X][Y - 1] == board[X][Y] && !visited[X][Y-1]))
        {
            result = dfs(X, Y - 1, visited);
            while (!result.empty())
            {
                ohno.push(result.top());
                result.pop();
            }
        }
        if (Y != BHEIGHT - 1 && (board[X][Y + 1] == board[X][Y] && !visited[X][Y+1]))
        {
            result = dfs(X, Y + 1, visited);
            while (!result.empty())
            {
                ohno.push(result.top());
                result.pop();
            }
        }
        ohno.push(pair<int, int>(X, Y));

        return ohno;
    }
    
public:
    Board()
    {
        board = vector<vector<Square>>(BWIDTH, vector<Square>(BHEIGHT, BLANK));
    }

    bool isIllegal(int X, int Y)
    {
        return X < 0 || Y < 0 || X >= BWIDTH || Y >= BHEIGHT;
    }

    Square getSquare(int X, int Y)
    {
        return board[X][Y];
    }

    void setSquare(int X, int Y, Square setting)
    {
        board[X][Y] = setting;
    }

    bool pop()
    {
        bool popped = false;
        bool visited[BWIDTH][BHEIGHT] = { 0, };
        
        for (int i = 0;i < BWIDTH;i++)
        {
            for (int j = 0;j < BHEIGHT; j++)
            {
                if (board[i][j] != BLANK)
                {
                    auto result = dfs(i, j, visited);
                    if (result.size() >= 4)
                    {
                        popped = true;
                        while (result.size() != 0)
                        {
                            board[result.top().first][result.top().second] = BLANK;
                            result.pop();
                        }
                    }
                }
            }
        }
        return popped;
    }
};

class Puyo
{
private:
    Square color;
    PuyoType type;
    Board& board;
    int X;
    int Y;
    int rotation;

public:
    Puyo(Square color, PuyoType type, int X, int Y, Board& board) : color(color), type(type), board(board), X(X), Y(Y), rotation(0) 
    {
        board.setSquare(X, Y, color);
    };
    Puyo(Board& board) : color(BLANK), type(HEAD), X(0), Y(0), rotation(0), board(board) {};

    Puyo& operator=(const Puyo& rhs)
    {
        color = rhs.color;
        type = rhs.type;
        board = rhs.board;
        X = rhs.X;
        Y = rhs.Y;
        rotation = rhs.rotation;
        return *this;
    }

    bool move(int movement)
    {
        if (board.isIllegal(X+movement, Y) || board.getSquare(X+movement, Y) != BLANK)
        {
            return false;
        }
        board.setSquare(X, Y, BLANK);
        X += movement;
        board.setSquare(X, Y, color);
        return true;
    }

    void fall()
    {
        if (isStopped())
        {
            return;
        }
        board.setSquare(X, Y, BLANK);
        Y += 1;
        board.setSquare(X, Y, color);
    }

    void rotate(int rotation)
    {
        if (type == TAIL)
        {
            int bakX = X;
            int bakY = Y;

            if (rotation == 1)
            {
                switch (this->rotation)
                {
                case 0:
                    X -= 1;
                    Y -= 1;
                    break;
                case 1:
                    X += 1;
                    Y -= 1;
                    break;
                case 2:
                    X += 1;
                    Y += 1;
                    break;
                case 3:
                    X -= 1;
                    Y += 1;
                }
            }
            else if (rotation == -1)
            {
                switch (this->rotation)
                {
                case 0:
                    cout << "a" << endl;
                    X += 1;
                    Y -= 1;
                    break;
                case 1:
                    cout << "b" << endl;
                    X += 1;
                    Y += 1;
                    break;
                case 2:
                    cout << "c" << endl;
                    X -= 1;
                    Y += 1;
                    break;
                case 3:
                    cout << "d" << endl;
                    X -= 1;
                    Y -= 1;
                }
            }

            if (board.isIllegal(X, Y) || board.getSquare(X, Y) != BLANK)
            {
                X = bakX;
                Y = bakY;
                return;
            }

            board.setSquare(bakX, bakY, BLANK);
            board.setSquare(X, Y, color);
            this->rotation += rotation;
            this->rotation = (4 + this->rotation) % 4;
        }
    }

    pair<int, int> getPosition()
    {
        return pair<int, int>(X, Y);
    }

    bool isStopped()
    {
        return board.isIllegal(X, Y + 1) || board.getSquare(X, Y+1) != BLANK;
    }
};

int main()
{
    srand(GetTickCount64());

    sf::RenderWindow window(sf::VideoMode(SWIDTH, SHEIGHT), L"��������");
    
    Board board;
    queue<pair<Square, Square>> puyoQue;
    pair<Puyo, Puyo> puyo = make_pair(Puyo(board), Puyo(board));

    vector<vector<RectangleShape>> rboard(BWIDTH, vector<RectangleShape>(BHEIGHT, RectangleShape(Vector2f(SWIDTH/BWIDTH, SHEIGHT/BHEIGHT))));
    for (int i = 0; i < BWIDTH; i++)
    {
        for (int j = 0; j < BHEIGHT; j++)
        {
            rboard[i][j].setPosition(SWIDTH / BWIDTH * i, SHEIGHT / BHEIGHT * j);
            rboard[i][j].setOutlineColor(Color::White);
            rboard[i][j].setOutlineThickness(MARGIN);
        }
    }

    Clock fallClock;

    bool blockinput = false;
    bool settled = false;

    int movement = 0;
    int rotation = 0;

    auto newPuyo = [&]()
    {
        auto temp = puyoQue.front();
        puyoQue.pop();
        puyoQue.push(pair<Square, Square>(static_cast<Square>(rand() % 4 + 1), static_cast<Square>(rand() % 4 + 1)));
        puyo = make_pair(Puyo(temp.first, HEAD, BWIDTH/2 - 1, 0, board), Puyo(temp.second, TAIL, BWIDTH/2 - 1, 1, board));
    };

    auto reset = [&]()
    {
        puyoQue = queue<pair<Square, Square>>();
        board = Board();

        puyoQue.push(pair<Square, Square>(static_cast<Square>(rand() % 4 + 1), static_cast<Square>(rand() % 4 + 1)));
        puyoQue.push(pair<Square, Square>(static_cast<Square>(rand() % 4 + 1), static_cast<Square>(rand() % 4 + 1)));
        puyoQue.push(pair<Square, Square>(static_cast<Square>(rand() % 4 + 1), static_cast<Square>(rand() % 4 + 1)));

        fallClock.restart();

        settled = false;
        blockinput = false;
        
        newPuyo();
    };

    auto settle = [&]()
    {
        newPuyo();
        puyoQue.push(pair<Square, Square>(static_cast<Square>(rand() % 4 + 1), static_cast<Square>(rand() % 4 + 1)));
    };

    auto fall = [&]()
    {
        fallClock.restart();

        blockinput = false;
        settled = true;
        if (puyo.first.getPosition().second > puyo.second.getPosition().second)
        {
            blockinput = puyo.first.isStopped() || blockinput;
            settled = puyo.first.isStopped() && settled;
            puyo.first.fall();
            blockinput = puyo.second.isStopped() || blockinput;
            settled = puyo.second.isStopped() && settled;
            puyo.second.fall();
        }
        else
        {
            blockinput = puyo.second.isStopped() || blockinput;
            settled = puyo.first.isStopped() && settled;
            puyo.second.fall();
            blockinput = puyo.first.isStopped() || blockinput;
            settled = puyo.second.isStopped() && settled;
            puyo.first.fall();
        }
    };

    auto draw = [&]()
    {
        window.clear();

        for (int i = 0; i < BWIDTH; i++)
        {
            for (int j = 0; j < BHEIGHT; j++)
            {
                rboard[i][j].setFillColor(colors[board.getSquare(i, j)]);
                window.draw(rboard[i][j]);
            }
        }

        window.display();
    };

    reset();
    newPuyo();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case Event::Closed:
                window.close();
                break;

            case Event::KeyPressed:
                switch (event.key.code)
                {
                case Keyboard::Left:
                    movement = -1;
                    break;
                case Keyboard::Right:
                    movement = 1;
                    break;
                case Keyboard::Up:
                    rotation = 1;
                    break;
                case Keyboard::Z:
                    rotation = -1;
                    break;
                case Keyboard::Down:
                    fall();
                    break;
                }
                break;
            }
        }

        int distance = abs(puyo.first.getPosition().first - puyo.second.getPosition().first) + abs(puyo.first.getPosition().second - puyo.second.getPosition().second);

        movement *= !(blockinput || (distance != 1));
        bool a, b;

        if (puyo.first.getPosition().first > puyo.second.getPosition().first)
        {
            if (movement == 1)
            {
                a = puyo.first.move(movement);
                b = puyo.second.move(movement);
                if (!a && b)
                {
                    puyo.second.move(-movement);
                }
                else if (!b && a)
                {
                    puyo.first.move(-movement);
                }
            }
            else 
            {
                a = puyo.second.move(movement);
                b = puyo.first.move(movement);
                if (!a && b)
                {
                    puyo.first.move(-movement);
                }
                else if (!b && a)
                {
                    puyo.second.move(-movement);
                }
            }
        }
        else 
        {
            if (movement == -1)
            {
                a = puyo.first.move(movement);
                b = puyo.second.move(movement);
                if (!a && b)
                {
                    puyo.second.move(-movement);
                }
                else if (!b && a)
                {
                    puyo.first.move(-movement);
                }
            }
            else
            {
                a = puyo.second.move(movement);
                b = puyo.first.move(movement);
                if (!a && b)
                {
                    puyo.first.move(-movement);
                }
                else if (!b && a)
                {
                    puyo.second.move(-movement);
                }
            }
        }
        movement = 0;

        rotation *= !blockinput;
        puyo.first.rotate(rotation);
        puyo.second.rotate(rotation);
        rotation = 0;

        if (fallClock.getElapsedTime().asSeconds() > FALLSPD)
        {
            fall();
        }

        if (settled)
        {
            if (puyo.first.getPosition() == pair<int, int>(BWIDTH / 2 - 1, 0) || puyo.second.getPosition() == pair<int, int>(BWIDTH / 2 - 1, 0))
            {
                reset();
            }
            else
            {
                bool allsettled = true;
                Clock dt;

                while (board.pop())
                {
                    do
                    {
                        dt.restart();

                        bool markings[BWIDTH][BHEIGHT + 1] = { 0, };
                        allsettled = true;
                        for (int i = 0; i < BWIDTH; i++)
                        {
                            for (int j = 0; j < BHEIGHT; j++)
                            {
                                if ((!board.isIllegal(i, j + 1) && (board.getSquare(i, j + 1) == BLANK && board.getSquare(i, j) != BLANK)) || markings[i][j + 1])
                                {
                                    allsettled = false;
                                    markings[i][j] = true;
                                }
                            }
                        }

                        for (int i = 0;i < BWIDTH;i++)
                        {
                            for (int j = BHEIGHT - 1;j >= 0;j--) // to prevent clipping
                            {
                                if (markings[i][j] == true)
                                {
                                    board.setSquare(i, j + 1, board.getSquare(i, j));
                                    board.setSquare(i, j, BLANK);
                                }
                            }
                        }

                        draw();
                        //sleep(seconds(1/60-dt.getElapsedTime().asSeconds()));
                        sleep(seconds(0.5));
                    } while (!allsettled);
                }
                
                newPuyo();
                settled = false;
            }
        }

        draw();
    }
    return 0;
}
