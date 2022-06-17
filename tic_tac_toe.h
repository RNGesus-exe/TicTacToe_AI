#ifndef TIC_TAC_TOE_H
#define TIC_TAC_TOE_H

#include <iostream>

#define TOTAL_ACTIONS 9
#define UCHAR_SIZE 3
#define ROW 3
#define COLUMN 3

enum player
{
    AI,
    USER
};

enum actions
{
    TOPLEFT,
    TOP,
    TOPRIGHT,
    MIDDLELEFT,
    MIDDLE,
    MIDDLERIGHT,
    BOTTOMLEFT,
    BOTTOM,
    BOTTOMRIGHT
};

using namespace std;

class TicTacToe
{
    class Node
    {
    public:
        unsigned char state[UCHAR_SIZE];
        Node *children[TOTAL_ACTIONS];
        int winRate;

        void setState(unsigned char state[])
        {
            this->state[0] = state[0];
            this->state[1] = state[1];
            this->state[2] = state[2];
        }
        Node()
        {
            this->winRate = 9876; // APPROX INFINITY
            this->state[0] = this->state[1] = this->state[2] = 0;
            this->children[0] = this->children[1] = this->children[2] =
                this->children[3] = this->children[4] = this->children[5] =
                    this->children[6] = this->children[7] = this->children[8] = nullptr;
        }
    };

    unsigned char curr_state[UCHAR_SIZE]; // To keep track of current game state
    char board[ROW][COLUMN];              // To perform mapping easily
    bool turn = false;                    // true - User && false - AI   {User - O && Ai - X}
    bool poss_actions[TOTAL_ACTIONS];     // To show how many possible moves available from a given state
    Node *head;                           // Parent node of game tree
    Node *curr_node;                      // To move with curr_state in game tree
    int total_calls = 0;

public:
    TicTacToe()
    {
        this->head = new Node;
        this->curr_state[0] = this->curr_state[1] = this->curr_state[2] = 0;
    }

    void emptyBoard(char board[ROW][COLUMN])
    {
        for (int i = 0; i < ROW; i++)
        {
            for (int j = 0; j < COLUMN; j++)
            {
                board[i][j] = '-';
            }
        }
    }

    void init()
    {
        int temp = 0;
        printf("\n Which player should go first ");
        printf("\n 1) User (You)");
        printf("\n 2) Computer (AI)");
        printf("\n Enter your choice = ");
        cin >> temp;

        print_board_info(); // Showcase Rules once

        if (temp == 1)
        {
            performUserAction();
        }
        else if (temp != 2)
        {
            cerr << "That is an invalid option, AI is shutting down!!\n";
            exit(1);
        }

        // next turn is AI's
        this->turn = 0;
    }

    bool isStateEqual(unsigned char source[], unsigned char destination[])
    {
        if (source[0] == destination[0] && source[1] == destination[1] && source[2] == destination[2])
        {
            return true;
        }
        return false;
    }

    void printFreeSlots(unsigned char state[], bool poss_action[])
    {
        returnFreeSlots(state, poss_action);
        cout << "Free slots are :";
        for (int i = 0; i < TOTAL_ACTIONS; i++)
        {
            if (poss_action[i])
            {
                cout << " " << i;
            }
        }
        cout << "\n\n";
    }

    void returnFreeSlots(unsigned char state[], bool poss_action[])
    {
        for (int i = 0; i < TOTAL_ACTIONS; i++)
        {
            poss_action[i] = checkSpace(state, i);
        }
    }

    bool isGameNotOver()
    {
        return !(isGameOver(this->curr_state));
    }

    void printChildren()
    {
        cout << "CHILDREN ARE AS FOLLOWS : \n";
        this->returnFreeSlots(this->curr_node->state, this->poss_actions);
        for (int i = 0; i < TOTAL_ACTIONS; i++)
        {
            if (this->poss_actions[i])
            {
                this->print_board(this->curr_node->children[i]->state);
                cout << " WinRate = " << this->curr_node->children[i]->winRate << endl;
            }
        }
    }

    void setCurrState(unsigned char state[])
    {
        this->curr_state[0] = state[0];
        this->curr_state[1] = state[1];
        this->curr_state[2] = state[2];
    }

    void setTurn(bool turn)
    {
        this->turn = turn;
    }

    void performAIAction()
    {

        int bestChoice = 0;
        int bestWinRate = -9876;
        this->returnFreeSlots(this->curr_state, this->poss_actions);
        for (unsigned int action = 0; action < TOTAL_ACTIONS; ++action)
        {
            if (this->poss_actions[action] && (this->curr_node->children[action]->winRate > bestWinRate))
            {
                bestWinRate = this->curr_node->children[action]->winRate;
                bestChoice = action;
            }
        }

        // Perform the best Action
        this->performAction(this->curr_state, bestChoice, AI, this->curr_state);
        this->curr_node = this->curr_node->children[bestChoice];

        cout << " AI has marked the " << bestChoice << " box \n";
    }

    void printCurrNode()
    {
        if (this->curr_node)
        {
            cout << "Below is the node state :\n\n";
            this->print_board(this->curr_node->state);
            cout << "-----------------------------\n\n";
        }
        else
        {
            cerr << "There was nothing to print !!!\n";
        }
    }

    void miniMaxAlgo(Node *node, bool curr_turn)
    {
        // Uncomment to debug game tree
        // print_board(node->state);

        if (!this->isGameOver(node->state))
        {
            for (unsigned int action = 0; action < TOTAL_ACTIONS; ++action)
            {
                if (this->checkSpace(node->state, action)) // Check all possible future states
                {
                    // Game Tree Formation

                    node->children[action] = new Node;                                            // Create a new future state
                    performAction(node->state, action, curr_turn, node->children[action]->state); // Perform action on future state
                    miniMaxAlgo(node->children[action], !curr_turn);                              // Check the future state for further future states
                }
            }
        }

        // Leaf Nodes Logic && Back Tracking Logic

        if (returnGameWon(node->state))
        {
            if (!curr_turn) // User turn
            {
                node->winRate = -10;
            }
            else // comp turn
            {
                node->winRate = 10;
            }
        }
        else if (returnTie(node->state))
        {
            node->winRate = 0;
        }
        else
        {
            // Back Tracking Logic
            int bestScore = 0;
            if (curr_turn) // MIN
            {
                bestScore = 9876;
            }
            else // MAX
            {
                bestScore = -9876;
            }

            // Find all possible children
            this->returnFreeSlots(node->state, this->poss_actions);
            // Perform minimax
            for (unsigned int child = 0; child < TOTAL_ACTIONS; ++child)
            {
                if (curr_turn && poss_actions[child]) // Check User turn and child node exists
                {
                    // Find minimum among children
                    if (node->children[child]->winRate < bestScore)
                    {
                        bestScore = node->children[child]->winRate;
                    }
                }
                else if (!curr_turn && poss_actions[child])
                {
                    // FInd the maximum among the children
                    if (node->children[child]->winRate > bestScore)
                    {
                        bestScore = node->children[child]->winRate;
                    }
                }
            }

            // Set best Possible winRate
            node->winRate = bestScore;
        }

        // Uncomment to see minimax results
        // this->print_board(node->state);
        // cout << "Node Score = " << node->winRate << endl;
    }

    void generateGameTree()
    {
        this->head = new Node;
        this->head->setState(this->curr_state);
        this->curr_node = this->head;        // First time initialize parent node
        miniMaxAlgo(this->head, this->turn); // Turn will be of AI
    }

    int get_calls()
    {
        return this->total_calls;
    }

    bool isGameOver(unsigned char state[])
    {
        return (returnGameWon(state) || returnTie(state));
    }

    bool returnGameWon(unsigned char state[])
    {
        emptyBoard(board);
        mapState(state, board);

        bool winFlag = false;

        // Top row
        if (board[0][0] != '-' && board[0][0] == board[0][1] && board[0][0] == board[0][2])
        {
            winFlag = true;
        }
        // Middle row
        else if (board[1][0] != '-' && board[1][0] == board[1][1] && board[1][0] == board[1][2])
        {
            winFlag = true;
        }
        // Bottom row
        else if (board[2][0] != '-' && board[2][0] == board[2][1] && board[2][0] == board[2][2])
        {
            winFlag = true;
        }
        // Left column
        else if (board[0][0] != '-' && board[0][0] == board[1][0] && board[0][0] == board[2][0])
        {
            winFlag = true;
        }
        // Middle column
        else if (board[0][1] != '-' && board[0][1] == board[1][1] && board[0][1] == board[2][1])
        {
            winFlag = true;
        }
        // Right column
        else if (board[0][2] != '-' && board[0][2] == board[1][2] && board[0][2] == board[2][2])
        {
            winFlag = true;
        }
        // Diagonal
        else if (board[0][0] != '-' && board[0][0] == board[1][1] && board[0][0] == board[2][2])
        {
            winFlag = true;
        }
        // Adjacent
        else if (board[0][2] != '-' && board[0][2] == board[1][1] && board[0][2] == board[2][0])
        {
            winFlag = true;
        }
        return winFlag;
    }

    bool returnTie(unsigned char state[])
    {
        bool tieFlag = true;

        returnFreeSlots(state, this->poss_actions);
        for (int i = 0; i < TOTAL_ACTIONS; i++)
        {
            if (this->poss_actions[i])
            {
                tieFlag = false;
                break;
            }
        }
        return tieFlag;
    }

    void victoryMessage()
    {
        this->print_board(this->curr_state);
        if (returnTie(this->curr_state))
        {
            cout << "The game ended in a draw !!!\n";
        }
        else if (returnGameWon(this->curr_state))
        {
            if (turn)
            {
                cout << "Congratulations you have won the game!!!\n";
            }
            else
            {
                cout << "Unfortunately, The Chad AI has won the game!!!\n";
            }
        }
    }

    void performUserAction()
    {
        // Display board infnormation
        print_board(this->curr_state);
        printFreeSlots(this->curr_state, this->poss_actions);

        // Take user input
        int position;
        cout << "Enter the slot you want to mark = ";
        cin >> position;

        // Validate user input
        bool crash = true;
        for (int i = 0; i < TOTAL_ACTIONS; i++)
        {
            if (this->poss_actions[i] && i == position)
            {
                crash = false;
                break;
            }
        }

        if (crash)
        {
            cerr << "There no such choice available, AI is shutting down!!\n";
            exit(1);
        }

        // Perform User Input

        performAction(this->curr_state, position, USER, this->curr_state);
        if (this->curr_node)
        {
            this->curr_node = this->curr_node->children[position];
        }
    }

    void performAction(unsigned char state[], int action, bool turn, unsigned char fur_state[])
    {
        fur_state[0] = state[0];
        fur_state[1] = state[1];
        fur_state[2] = state[2];

        switch (action)
        {
        case 0:
            (turn) ? (fur_state[0] |= 64) : (fur_state[0] |= 128);
            break;

        case 1:
            (turn) ? (fur_state[0] |= 16) : (fur_state[0] |= 32);
            break;

        case 2:
            (turn) ? (fur_state[0] |= 4) : (fur_state[0] |= 8);
            break;

        case 3:
            (turn) ? (fur_state[0] |= 1) : (fur_state[0] |= 2);
            break;

        case 4:
            (turn) ? (fur_state[1] |= 64) : (fur_state[1] |= 128);
            break;

        case 5:
            (turn) ? (fur_state[1] |= 16) : (fur_state[1] |= 32);
            break;

        case 6:
            (turn) ? (fur_state[1] |= 4) : (fur_state[1] |= 8);
            break;

        case 7:
            (turn) ? (fur_state[1] |= 1) : (fur_state[1] |= 2);
            break;

        case 8:
            (turn) ? (fur_state[2] |= 64) : (fur_state[2] |= 128);
            break;
        }
    }

    bool checkSpace(unsigned char state[], int action)
    {
        switch (action)
        {
        case 0:
            return (!(state[0] & 192)); // 0 0 X X X X X X
        case 1:
            return (!(state[0] & 48)); // X X 0 0 X X X X
        case 2:
            return (!(state[0] & 12)); // X X X X 0 0 X X
        case 3:
            return (!(state[0] & 3)); // X X X X X X 0 0
        case 4:
            return (!(state[1] & 192)); // 0 0 X X X X X X
        case 5:
            return (!(state[1] & 48)); // X X 0 0 X X X X
        case 6:
            return (!(state[1] & 12)); // X X X X 0 0 X X
        case 7:
            return (!(state[1] & 3)); // X X X X X X 0 0
        case 8:
            return (!(state[2] & 192)); // 0 0 X X X X X X
        }
        return false;
    }

    void mapState(unsigned char state[], char board[ROW][COLUMN])
    {
        // for char[0]

        if ((!!(state[0] & 128)) && (!(state[0] & 64))) // 1 0 0 0 0 0 0 0
        {
            board[0][0] = 'X';
        }
        else if ((!(state[0] & 128)) && (!!(state[0] & 64))) // 0 1 0 0 0 0 0 0
        {
            board[0][0] = 'O';
        }

        if ((!!(state[0] & 32)) && (!(state[0] & 16))) // 0 0 1 0 0 0 0 0
        {
            board[0][1] = 'X';
        }
        else if ((!(state[0] & 32)) && (!!(state[0] & 16))) // 0 0 0 1 0 0 0 0
        {
            board[0][1] = 'O';
        }

        if ((!!(state[0] & 8)) && (!(state[0] & 4))) // 0 0 0 0 1 0 0 0
        {
            board[0][2] = 'X';
        }
        else if ((!(state[0] & 8)) && (!!(state[0] & 4))) // 0 0 0 0 0 1 0 0
        {
            board[0][2] = 'O';
        }

        if ((!!(state[0] & 2)) && (!(state[0] & 1))) // 0 0 0 0 0 0 1 0
        {
            board[1][0] = 'X';
        }
        else if ((!(state[0] & 2)) && (!!(state[0] & 1))) // 0 0 0 0 0 0 0 1
        {
            board[1][0] = 'O';
        }

        // for char[1]

        if ((!!(state[1] & 128)) && (!(state[1] & 64))) // 1 0 0 0 0 0 0 0
        {
            board[1][1] = 'X';
        }
        else if ((!(state[1] & 128)) && (!!(state[1] & 64))) // 0 1 0 0 0 0 0 0
        {
            board[1][1] = 'O';
        }

        if ((!!(state[1] & 32)) && (!(state[1] & 16))) // 0 0 1 0 0 0 0 0
        {
            board[1][2] = 'X';
        }
        else if ((!(state[1] & 32)) && (!!(state[1] & 16))) // 0 0 0 1 0 0 0 0
        {
            board[1][2] = 'O';
        }

        if ((!!(state[1] & 8)) && (!(state[1] & 4))) // 0 0 0 0 1 0 0 0
        {
            board[2][0] = 'X';
        }
        else if ((!(state[1] & 8)) && (!!(state[1] & 4))) // 0 0 0 0 0 1 0 0
        {
            board[2][0] = 'O';
        }

        if ((!!(state[1] & 2)) && (!(state[1] & 1))) // 0 0 0 0 0 0 1 0
        {
            board[2][1] = 'X';
        }
        else if ((!(state[1] & 2)) && (!!(state[1] & 1))) // 0 0 0 0 0 0 0 1
        {
            board[2][1] = 'O';
        }

        // for char[2]

        if ((!!(state[2] & 128)) && (!(state[2] & 64))) // 1 0 0 0 0 0 0 0
        {
            board[2][2] = 'X';
        }
        else if ((!(state[2] & 128)) && (!!(state[2] & 64))) // 0 1 0 0 0 0 0 0
        {
            board[2][2] = 'O';
        }
    }

    void print_board(unsigned char state[])
    {
        this->emptyBoard(this->board);
        this->mapState(state, this->board);

        cout << "\nBOARD STATUS\n";
        printf("\t\t\t %c | %c | %c \n", board[0][0], board[0][1], board[0][2]);
        printf("\n\t\t\t-----------\n");
        printf("\t\t\t %c | %c | %c \n", board[1][0], board[1][1], board[1][2]);
        printf("\n\t\t\t-----------\n");
        printf("\t\t\t %c | %c | %c \n\n", board[2][0], board[2][1], board[2][2]);
        cout << '\n';
    }

    void print_board_info()
    {
        printf("\nChoose a cell numbered from 0 to 8 as below and play\n\n");

        printf("\t\t\t 0 | 1 | 2 \n");
        printf("\t\t\t-----------\n");
        printf("\t\t\t 3 | 4 | 5 \n");
        printf("\t\t\t-----------\n");
        printf("\t\t\t 6 | 7 | 8 \n\n");
    }

    // Destructor is to be updated further
    ~TicTacToe()
    {
        delete this->head;
    }
};

#endif