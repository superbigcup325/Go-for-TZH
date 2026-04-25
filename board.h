#ifndef OOP_BOARD_H
#define OOP_BOARD_H

#include <iostream>
#include <vector>

enum player {BLACK=1,WHITE};

class GoGame {
private:
    std::vector<std::vector<int>> graph;
    // 0 none, 1 black, 2 white
    int maxCount;
    int currentCount;
    int size;
    // std::string blackC="";
    // std::string whiteC="";
    std::vector<std::pair<int,int>> directions={
        {0,1},
        {1,0},
        {1,1},
        {1,-1}
    };

public:
    GoGame(const int size_value=38);

    // 落子
    void set(const int x,const int y,const player currentPlayer);

    bool outOfRange(const int x,const int y);

    // 坐标是否合法
    bool validPosition(const int x,const int y);

    // 判断是否有胜利
    bool Win(const int x,const int y,const int currentPlayer);

    // 判断是否结束 平局？
    bool GameOver();

    void show();

    std::vector<std::vector<int>> getGraph();

    // 获取当前角色
    friend player CurrentPlayer(int currentTurn);
};

inline player CurrentPlayer(const int currentTurn) {
    return (currentTurn&1? BLACK:WHITE);
}

GoGame::GoGame(const int size_value):
    size(size_value),
    currentCount(0),
    maxCount(size_value*size_value) {
    // 1-based
    graph=std::vector(size+1,std::vector(size+1,0));
}

void GoGame::set(const int x,const int y,const player currentPlayer) {
    if (!validPosition(x,y)) {
        std::cout<<"error: invalid position"<<std::endl;
        return;
    }
    graph[x][y]=currentPlayer;
    currentCount++;
}

bool GoGame::outOfRange(const int x,const int y) {
    return (x<1||x>size||y<1||y>size);
}

bool GoGame::validPosition(const int x,const int y) {
    if (outOfRange(x,y)) return false;
    // 是否下过
    if (graph[x][y]==0) return true;
    else return false;
}

bool GoGame::Win(const int x,const int y,const int currentPlayer) {
    for (auto [dx,dy]:directions) {
        int count=1;
        int nx=x+dx,ny=y+dy;
        while (!outOfRange(nx,ny)&&graph[nx][ny]==currentPlayer) {
            count++;
            nx+=dx,ny+=dy;
        }
        nx=x-dx,ny=y-dy;
        while (!outOfRange(nx,ny)&&graph[nx][ny]==currentPlayer) {
            count++;
            nx-=dx,ny-=dy;
        }
        if (count>=5) return true;
    }
    return false;
}

bool GoGame::GameOver() {
    return currentCount>=maxCount;
}

void GoGame::show() {
    for (int i=1;i<=size;i++) {
        for (int j=1;j<=size;j++) {
            std::cout<<(graph[i][j]==BLACK? "●":graph[i][j]==WHITE? "○":" ")<<(j==size? "\n":" ");
        }
    }
}

std::vector<std::vector<int>> GoGame::getGraph() {
    return graph;
}

#endif //OOP_BOARD_H
