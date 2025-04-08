#include <iostream>
#include <climits>
#include <vector>
#include <algorithm>
using namespace std;
//AYBERK SAVAS 
char aiSymbol, playerSymbol;
int nodesVisited = 0;

class board {
public:
    char cells[3][3];

    board() {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                cells[i][j] = ' ';
            }
        }
    }

    board(const board &other) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                cells[i][j] = other.cells[i][j];
            }
        }
    }

    bool updateBoard(int move, char symbol) {
        int row = (move - 1) / 3;
        int col = (move - 1) % 3;
        if (cells[row][col] == ' ') {
            cells[row][col] = symbol;
            return true;
        }
        else {
            return false;
        }
    }

    void display() const {
        for (int i = 0; i < 3; i++) {
            cout << " ";
            for (int j = 0; j < 3; j++) {
                cout << cells[i][j];
                if (j < 2) {
                    cout << " | ";
                }
            }
            cout << endl;
            if (i < 2) {
                cout << "---+---+---" << endl;
            }
        }
    }
};

char checkWinner(const board &b) {
    for (int i = 0; i < 3; i++) {
        //checking rows
        if ((b.cells[i][0] != ' ') && (b.cells[i][0] == b.cells[i][1]) && (b.cells[i][1] == b.cells[i][2])) {
            return b.cells[i][0];
        }
    }
    //checking columns
    for (int j = 0; j < 3; j++) {
        if ((b.cells[0][j] != ' ') && (b.cells[0][j] == b.cells[1][j]) && (b.cells[1][j] == b.cells[2][j])) {
            return b.cells[0][j];
        }
    }
    //checking diagonals
    if ((b.cells[0][0] != ' ') && (b.cells[0][0] == b.cells[1][1]) && (b.cells[1][1] == b.cells[2][2])) {
        return b.cells[0][0];
    }
    if ((b.cells[0][2] != ' ') && (b.cells[0][2] == b.cells[1][1]) && (b.cells[1][1] == b.cells[2][0])) {
        return b.cells[0][2];
    }
    return ' ';
}

bool isTerminal(const board &b) {
    if (checkWinner(b) != ' ') {
        return true;
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (b.cells[i][j] == ' ') {
                return false;
            }
        }
    }
    return true;
}

int utility(const board &b) {
    char winner = checkWinner(b);
    if (winner == aiSymbol) {
        return 10;
    }
    else if (winner == playerSymbol) {
        return -10;
    }
    else {
        return 0;
    }
}

vector<int> availableMoves(const board &b) {
    vector<int> moves;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (b.cells[i][j] == ' ') {
                moves.push_back(i * 3 + j + 1);
            }
        }
    }
    return moves;
}

class treeNode {
public:
    int row;
    int column;
    int eva;
    board boardState;
    vector<treeNode*> children;

    treeNode(const board &state, int move, int eva = 0) : boardState(state), eva(eva) {
        if (move == -1) {
            row = -1;
            column = -1;
        }
        else {
            row = (move - 1) / 3;
            column = (move - 1) % 3;
        }
    }

    ~treeNode() {
        for (size_t i = 0; i < children.size(); i++) {
            delete children[i];
        }
    }
};

int minimax(treeNode* node, int depth, int alpha, int beta, bool maximizingPlayer) {
    nodesVisited++;

    if ((isTerminal(node->boardState)) || (depth == 0)) {
        node->eva = utility(node->boardState);
        return node->eva;
    }

    vector<int> moves = availableMoves(node->boardState);

    if (maximizingPlayer) {
        int maxEval = INT_MIN;
        for (size_t i = 0; i < moves.size(); i++) {
            int move = moves[i];
            board newBoard = node->boardState;
            newBoard.updateBoard(move, aiSymbol);
            treeNode* child = new treeNode(newBoard, move);
            node->children.push_back(child);
            int eval = minimax(child, depth - 1, alpha, beta, false);
            maxEval = max(maxEval, eval);
            alpha = max(alpha, eval);
            if (beta <= alpha) {
                break;
            }
        }
        node->eva = maxEval;
        return maxEval;
    }
    else {
        int minEval = INT_MAX;
        for (size_t i = 0; i < moves.size(); i++) {
            int move = moves[i];
            board newBoard = node->boardState;
            newBoard.updateBoard(move, playerSymbol);
            treeNode* child = new treeNode(newBoard, move);
            node->children.push_back(child);
            int eval = minimax(child, depth - 1, alpha, beta, true);
            minEval = min(minEval, eval);
            beta = min(beta, eval);
            if (beta <= alpha) {
                break;
            }
        }
        node->eva = minEval;
        return minEval;
    }
}

int main() {
    string choice;
    board gameBoard;

    cout << "Do you want to play as X or O? (X goes first): ";
    while (true) {
        cin >> choice;
        if ((choice == "X") || (choice == "x")) {
            playerSymbol = 'X';
            aiSymbol = 'O';
            break;
        }
        else if ((choice == "O") || (choice == "o")) {
            playerSymbol = 'O';
            aiSymbol = 'X';
            break;
        }
        else {
            cout << "Wrong choice, try again: ";
        }
    }

    cout << "Welcome to Tic Tac Toe!" << endl;
    cout << "You are playing as " << playerSymbol << "." << endl;
    cout << "The AI is playing as " << aiSymbol << "." << endl;
    cout << "Enter a number from 1-9 to make your move:" << "\n" << endl;
    cout << " 1 | 2 | 3" << endl;
    cout << "---+---+---" << endl;
    cout << " 4 | 5 | 6" << endl;
    cout << "---+---+---" << endl;
    cout << " 7 | 8 | 9\n" << endl;

    bool playerTurn = (playerSymbol == 'X');
    const int maxDepth = 9;

    while (!isTerminal(gameBoard)) {
        if (playerTurn) {
            int move;
            bool valid = false;
            while (!valid) {
                cout << "Your move (1-9): ";
                cin >> move;
                valid = gameBoard.updateBoard(move, playerSymbol);
                if (!valid) {
                    cout << "That position is already taken. Try again." << endl;
                }
            }
        }
        else {
            cout << "AI is making a move..." << endl;
            nodesVisited = 0;

            treeNode* root = new treeNode(gameBoard, -1);
            int bestMove = -1;
            int bestValue = INT_MIN;
            vector<int> moves = availableMoves(gameBoard);
            for (size_t i = 0; i < moves.size(); i++) {
                int move = moves[i];
                board newBoard = gameBoard;
                newBoard.updateBoard(move, aiSymbol);
                treeNode* child = new treeNode(newBoard, move);
                root->children.push_back(child);
                int eval = minimax(child, maxDepth, INT_MIN, INT_MAX, false);
                if (eval > bestValue) {
                    bestValue = eval;
                    bestMove = move;
                }
            }

            cout << "AI chose position " << bestMove << "." << endl;
            cout << "Nodes explored: " << (nodesVisited + 1) << endl;
            gameBoard.updateBoard(bestMove, aiSymbol);
            delete root;
        }

        cout << endl;
        gameBoard.display();
        cout << endl;
        playerTurn = !playerTurn;
    }

    char winner = checkWinner(gameBoard);
    if (winner == playerSymbol) {
        cout << "Congratulations! You win!" << endl;
    }
    else if (winner == aiSymbol) {
        cout << "The AI wins!" << endl;
    }
    else {
        cout << "It's a draw!" << endl;
    }

    return 0;
}
