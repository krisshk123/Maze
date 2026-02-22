#include "raylib.h"
#include <vector>
#include <stack>
#include <cstdlib>
#include <ctime>

const int CELL_SIZE = 40;
const int MAZE_WIDTH = 15;
const int MAZE_HEIGHT = 10;

enum GameState { MENU, CHESS_KINGDOM, ESCAPE_CHRONOLAB, MYTHICAL_MAZE, EXIT };
GameState currentGameState = MENU;

struct Cell {
    bool visited = false;
    bool walls[4] = {true, true, true, true};
};

class Maze {
public:
    Cell cells[MAZE_WIDTH][MAZE_HEIGHT];
    std::stack<Vector2> stack;
    std::vector<Vector2> path;
    Vector2 playerPos = {0, 0};
    Vector2 exitPos = {MAZE_WIDTH - 1, MAZE_HEIGHT - 1};

    Maze() {
        GenerateMaze();
    }

    void GenerateMaze() {
        srand(time(nullptr));
        Vector2 start = {0, 0};
        stack.push(start);
        cells[0][0].visited = true;

        while (!stack.empty()) {
            Vector2 current = stack.top();
            std::vector<Vector2> neighbors = GetUnvisitedNeighbors(current);

            if (!neighbors.empty()) {
                Vector2 next = neighbors[rand() % neighbors.size()];
                RemoveWallBetween(current, next);
                cells[(int)next.x][(int)next.y].visited = true;
                stack.push(next);
            } else {
                stack.pop();
            }
        }
    }

    std::vector<Vector2> GetUnvisitedNeighbors(Vector2 cell) {
        std::vector<Vector2> neighbors;
        if (cell.y > 0 && !cells[(int)cell.x][(int)cell.y - 1].visited)
            neighbors.push_back({cell.x, cell.y - 1});
        if (cell.x < MAZE_WIDTH - 1 && !cells[(int)cell.x + 1][(int)cell.y].visited)
            neighbors.push_back({cell.x + 1, cell.y});
        if (cell.y < MAZE_HEIGHT - 1 && !cells[(int)cell.x][(int)cell.y + 1].visited)
            neighbors.push_back({cell.x, cell.y + 1});
        if (cell.x > 0 && !cells[(int)cell.x - 1][(int)cell.y].visited)
            neighbors.push_back({cell.x - 1, cell.y});
        return neighbors;
    }

    void RemoveWallBetween(Vector2 current, Vector2 next) {
        if (current.x == next.x) {
            if (current.y > next.y) {
                cells[(int)current.x][(int)current.y].walls[0] = false;
                cells[(int)next.x][(int)next.y].walls[2] = false;
            } else {
                cells[(int)current.x][(int)current.y].walls[2] = false;
                cells[(int)next.x][(int)next.y].walls[0] = false;
            }
        } else if (current.y == next.y) {
            if (current.x > next.x) {
                cells[(int)current.x][(int)current.y].walls[3] = false;
                cells[(int)next.x][(int)next.y].walls[1] = false;
            } else {
                cells[(int)current.x][(int)current.y].walls[1] = false;
                cells[(int)next.x][(int)next.y].walls[3] = false;
            }
        }
    }

    void MovePlayer(int dx, int dy) {
        Vector2 newPos = {playerPos.x + dx, playerPos.y + dy};
        if (newPos.x >= 0 && newPos.x < MAZE_WIDTH && newPos.y >= 0 && newPos.y < MAZE_HEIGHT) {
            if ((dx == 1 && !cells[(int)playerPos.x][(int)playerPos.y].walls[1]) ||
                (dx == -1 && !cells[(int)playerPos.x][(int)playerPos.y].walls[3]) ||
                (dy == 1 && !cells[(int)playerPos.x][(int)playerPos.y].walls[2]) ||
                (dy == -1 && !cells[(int)playerPos.x][(int)playerPos.y].walls[0])) {
                playerPos = newPos;
                path.push_back(playerPos);
            }
        }
        if (playerPos.x == exitPos.x && playerPos.y == exitPos.y) {
            currentGameState = MENU;
        }
    }

    void DrawTrail() {
        for (const auto& pos : path) {
            DrawRectangle(pos.x * CELL_SIZE, pos.y * CELL_SIZE, CELL_SIZE, CELL_SIZE, RED);
        }
    }

    void Draw() {
        DrawTrail();
        for (int x = 0; x < MAZE_WIDTH; x++) {
            for (int y = 0; y < MAZE_HEIGHT; y++) {
                int screenX = x * CELL_SIZE;
                int screenY = y * CELL_SIZE;
                
                if (cells[x][y].walls[0]) DrawLine(screenX, screenY, screenX + CELL_SIZE, screenY, WHITE);
                if (cells[x][y].walls[1]) DrawLine(screenX + CELL_SIZE, screenY, screenX + CELL_SIZE, screenY + CELL_SIZE, WHITE);
                if (cells[x][y].walls[2]) DrawLine(screenX, screenY + CELL_SIZE, screenX + CELL_SIZE, screenY + CELL_SIZE, WHITE);
                if (cells[x][y].walls[3]) DrawLine(screenX, screenY, screenX, screenY + CELL_SIZE, WHITE);
            }
        }
        DrawCircle(playerPos.x * CELL_SIZE + CELL_SIZE / 2, playerPos.y * CELL_SIZE + CELL_SIZE / 2, 10, YELLOW);
        DrawRectangle(exitPos.x * CELL_SIZE, exitPos.y * CELL_SIZE, CELL_SIZE, CELL_SIZE, GREEN);
    }
};

Maze maze;

void HandleInput() {
    if (IsKeyPressed(KEY_ONE)) currentGameState = CHESS_KINGDOM;
    if (IsKeyPressed(KEY_TWO)) currentGameState = ESCAPE_CHRONOLAB;
    if (IsKeyPressed(KEY_THREE)) currentGameState = MYTHICAL_MAZE;
    if (currentGameState == MYTHICAL_MAZE) {
        if (IsKeyPressed(KEY_RIGHT)) maze.MovePlayer(1, 0);
        if (IsKeyPressed(KEY_LEFT)) maze.MovePlayer(-1, 0);
        if (IsKeyPressed(KEY_DOWN)) maze.MovePlayer(0, 1);
        if (IsKeyPressed(KEY_UP)) maze.MovePlayer(0, -1);
    }
}

int main() {
    InitWindow(800, 600, "Chronoscape");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        HandleInput();
        BeginDrawing();
        ClearBackground(BLACK);
        if (currentGameState == MYTHICAL_MAZE) maze.Draw();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}


