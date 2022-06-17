#include "tic_tac_toe.h"

int main()
{

    TicTacToe game;
    game.init();
    game.generateGameTree();

    while (game.isGameNotOver())
    {
        // Perform AI Action
        // game.printChildren();
        game.setTurn(AI);
        game.performAIAction();

        if (!game.isGameNotOver())
        {
            break;
        }

        // Perform User Action
        game.setTurn(USER);
        game.performUserAction();
    }

    game.victoryMessage();

    return 0;
}
