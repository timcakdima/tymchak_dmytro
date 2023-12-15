#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <stack>
#include <Windows.h>

using namespace std;

const int ROWS = 6;
const int COLS = 6;

struct Node {
    int row, col, g, h, f;
    Node* parent;

    Node(int r, int c, int g_val, int h_val, Node* p) : row(r), col(c), g(g_val), h(h_val), parent(p) {
        f = g + h;
    }
};

bool isValid(int row, int col) {
    return (row >= 0) && (row < ROWS) && (col >= 0) && (col < COLS);
}

bool isObstacle(const vector<vector<int>>& matrix, int row, int col) {
    return matrix[row][col] == 1;
}

bool isDestination(int row, int col, const pair<int, int>& dest) {
    return row == dest.first && col == dest.second;
}

int calculateHValue(int row, int col, const pair<int, int>& dest) {
    return abs(row - dest.first) + abs(col - dest.second);
}

void tracePath(Node* endNode) {
    stack<pair<int, int>> path;
    while (endNode != nullptr) {
        path.push({ endNode->row, endNode->col });
        endNode = endNode->parent;
    }

    cout << "Шлях: ";
    while (!path.empty()) {
        auto point = path.top();
        path.pop();
        cout << "[" << point.first << ", " << point.second << "] ";
    }
    cout << endl;
}

bool isPartOfPath(Node* currentNode, int row, int col);


void printMatrixWithHighlightedPath(const vector<vector<int>>& matrix, const pair<int, int>& start,
    const pair<int, int>& dest, Node* currentNode) {
    system("cls");  // Очистка консолі (Windows)

    cout << "Матриця:" << endl;
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (make_pair(i, j) == start) {
                cout << "A ";
            }
            else if (make_pair(i, j) == dest) {
                cout << "B ";
            }
            else {
                if (matrix[i][j] == 1) {
                    cout << "* ";
                }
                else if (currentNode != nullptr && isPartOfPath(currentNode, i, j)) {
                    cout << ". ";  // Виділяємо шлях символом "."
                }
                else {
                    cout << "0 ";
                }
            }
        }
        cout << endl;
    }
    Sleep(500);  // Пауза для візуалізації кроку алгоритму (500 мілісекунд)
}


bool isPartOfPath(Node* currentNode, int row, int col) {
    while (currentNode != nullptr) {
        if (currentNode->row == row && currentNode->col == col) {
            return true;
        }
        currentNode = currentNode->parent;
    }
    return false;
}

void aStar(const vector<vector<int>>& matrix, const pair<int, int>& start, const pair<int, int>& dest) {
    auto cmp = [](Node* a, Node* b) {
        return (a->f) > (b->f);
        };

    priority_queue<Node*, vector<Node*>, decltype(cmp)> openSet(cmp);

    vector<vector<bool>> closedSet(ROWS, vector<bool>(COLS, false));

    Node* startNode = new Node(start.first, start.second, 0, calculateHValue(start.first, start.second, dest), nullptr);
    openSet.push(startNode);

    while (!openSet.empty()) {
        Node* currentNode = openSet.top();
        openSet.pop();

        int row = currentNode->row;
        int col = currentNode->col;

        closedSet[row][col] = true;

        if (isDestination(row, col, dest)) {
            printMatrixWithHighlightedPath(matrix, start, dest, currentNode);
            tracePath(currentNode);
            return;
        }

        vector<pair<int, int>> successors = {
            {row - 1, col}, {row + 1, col}, {row, col - 1}, {row, col + 1}
        };

        for (const auto& successor : successors) {
            int nextRow = successor.first;
            int nextCol = successor.second;

            if (isValid(nextRow, nextCol) && !closedSet[nextRow][nextCol] && !isObstacle(matrix, nextRow, nextCol)) {
                int gValue = currentNode->g + 1;
                int hValue = calculateHValue(nextRow, nextCol, dest);
                Node* successorNode = new Node(nextRow, nextCol, gValue, hValue, currentNode);

                openSet.push(successorNode);
            }
        }

        // Виводимо матрицю з виділеним шляхом
        printMatrixWithHighlightedPath(matrix, start, dest, currentNode);
    }

    cout << "Шлях не знайдено." << endl;
}

int main() {
    SetConsoleOutputCP(1251);

    ifstream inputFile("Area.txt");
    if (!inputFile.is_open()) {
        cout << "Помилка відкриття файлу." << endl;
        return 1;
    }

    vector<vector<int>> matrix(ROWS, vector<int>(COLS));
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            inputFile >> matrix[i][j];
        }
    }

    inputFile.close();

    pair<int, int> start, dest;
    cout << "Введіть початкову точку (рядок колонка): ";
    cin >> start.first >> start.second;

    cout << "Введіть кінцеву точку (рядок колонка): ";
    cin >> dest.first >> dest.second;

    if (!isValid(start.first, start.second) || !isValid(dest.first, dest.second) ||
        isObstacle(matrix, start.first, start.second) || isObstacle(matrix, dest.first, dest.second)) {
        cout << "Неправильна початкова або кінцева точка." << endl;
        system("pause");
        return 1;
    }

    if (matrix[dest.first][dest.second] == 1) {
        cout << "Кінцева точка знаходиться на перешкоді. До неї не можна дістатися." << endl;
        system("pause");
        return 1;
    }

    system("pause");

    aStar(matrix, start, dest);

    return 0;
}
