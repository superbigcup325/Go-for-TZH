#ifndef OOP_GO_H
#define OOP_GO_H

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
    GoGame(const int size_value=38):
    size(size_value),
    currentCount(0),
    maxCount(size_value*size_value) {
        // 1-based
        graph=std::vector(size+1,std::vector(size+1,0));
    }

    // 落子
    void set(const int x,const int y,const player currentPlayer) {
        if (!validPosition(x,y)) {
            std::cout<<"error: invalid position"<<std::endl;
            return;
        }
        graph[x][y]=currentPlayer;
        currentCount++;
    }

    bool outOfRange(const int x,const int y) {
        return (x<1||x>size||y<1||y>size);
    }

    // 坐标是否合法
    bool validPosition(const int x,const int y) {
        if (outOfRange(x,y)) return false;
        // 是否下过
        if (graph[x][y]==0) return true;
        else return false;
    }

    // 判断是否有胜利
    bool Win(const int x,const int y,const int currentPlayer) {
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

    // 判断是否结束 平局？
    bool GameOver() {
        return currentCount>=maxCount;
    }

    void show() {
        for (int i=1;i<=size;i++) {
            for (int j=1;j<=size;j++) {
                std::cout<<(graph[i][j]==BLACK? "●":graph[i][j]==WHITE? "○":" ")<<(j==size? "\n":" ");
            }
        }
    }

    std::vector<std::vector<int>> getGraph() {
        return graph;
    }

    // 获取当前角色
    friend player CurrentPlayer(int currentTurn);
};

inline player CurrentPlayer(const int currentTurn) {
    return (currentTurn&1? BLACK:WHITE);
}

#endif //OOP_GO_H