#ifndef GO_MINIMAX_H
#define GO_MINIMAX_H

#include "board.h"
#include <vector>
#include <algorithm>
#include <climits>

class Minimax {
private:
    player self;
    player opponent;
    const double defendWeight=1.1;
    int depth;
    int nodeCount;

    struct Pattern {
        int count=1;
        int openEnds=0;

        int score() const {
            if (count >= 5) return 1000000;
            if (count == 4) {
                if (openEnds == 2) return 100000;
                if (openEnds == 1) return 15000;
            }
            if (count == 3) {
                if (openEnds == 2) return 5000;
                if (openEnds == 1) return 800;
            }
            if (count == 2) {
                if (openEnds == 2) return 500;
                if (openEnds == 1) return 50;
            }
            if (count == 1) {
                if (openEnds == 2) return 30;
                if (openEnds == 1) return 5;
            }
            return 0;
        }
    };

    Pattern analyzeDirection(const GoGame& g,int x,int y,int dx,int dy,player p) const;
    int evaluatePiece(const GoGame& g,int x,int y,player p) const;
    int evaluateEmpty(const GoGame& g,int x,int y) const;
    double evaluate(const GoGame& g) const;
    bool checkWin(const GoGame& g,player p) const;
    std::vector<std::pair<int,int>> getCandidateMoves(const GoGame& g) const;
    int alphaBeta(GoGame& g,int depth,int alpha,int beta,bool isMaximizing);

public:
    Minimax(player self_value,int depth_value,double defendWeight_value=1.0);
    std::pair<int,int> getBestMove(GoGame& g);
    void setDepth(int depth_value) { depth = depth_value; }
    int getNodesSearched() const { return nodeCount; }
    player getSelf() const { return self; }
};

// 单点棋型分析
Minimax::Pattern Minimax::analyzeDirection(const GoGame& g,int x,int y,int dx,int dy,player p) const {
    Pattern pattern;
    int count=1,openEnds=0;

    // 正向边
    int nx=x+dx,ny=y+dy;
    while (!g.outOfRange(nx,ny)&&g.getPlayer(nx,ny)==p) {
        nx+=dx;
        ny+=dy;
        pattern.count++;
    }
    if (!g.outOfRange(nx,ny)&&g.getPlayer(nx,ny)==NONE) {
        pattern.openEnds++;
    }

    // 反向边
    nx=x-dx,ny=y-dy;
    while (!g.outOfRange(nx,ny)&&g.getPlayer(nx,ny)==p) {
        nx-=dx;
        ny-=dy;
        pattern.count++;
    }
    if (!g.outOfRange(nx,ny)&&g.getPlayer(nx,ny)==NONE) {
        pattern.openEnds++;
    }

    return pattern;
}

int Minimax::evaluatePiece(const GoGame& g,int x,int y,player p) const {
    if (g.getPlayer(x,y)!=p) return 0;
    int score=0;
    std::vector<std::pair<int,int>> directions={
        {0,1},{1,0},{1,1},{1,-1}
    };
    for (auto& [dx,dy]:directions) {
        int bx=x-dx,by=y-dy;
        if (!g.outOfRange(bx,by)&&g.getPlayer(bx,by)==p) continue;

        Pattern pattern;
        pattern.count=1;
        pattern.openEnds=0;

        int nx=x+dx,ny=y+dy;
        while (!g.outOfRange(nx,ny)&&g.getPlayer(nx,ny)==p) {
            nx+=dx;
            ny+=dy;
            pattern.count++;
        }
        if (!g.outOfRange(nx,ny)&&g.getPlayer(nx,ny)==NONE) {
            pattern.openEnds++;
        }
        if (!g.outOfRange(bx,by)&&g.getPlayer(bx,by)==NONE) {
            pattern.openEnds++;
        }

        score+=pattern.score();
    }
    return score;
}

int Minimax::evaluateEmpty(const GoGame& g,int x,int y) const {
    if (g.getPlayer(x,y)!=NONE) return 0;
    int score=0;
    std::vector<std::pair<int,int>> directions={
        {0,1},{1,0},{1,1},{1,-1}
    };
    for (auto& [dx,dy]:directions) {
        score+=analyzeDirection(g,x,y,dx,dy,BLACK).score();
        score+=analyzeDirection(g,x,y,dx,dy,WHITE).score();
    }
    return score;
}

double Minimax::evaluate(const GoGame& g) const {
    int selfScore=0;
    int opponentScore=0;
    int size=g.getSize();

    for (int x=1;x<=size;x++) {
        for (int y=1;y<=size;y++) {
            player p=g.getPlayer(x,y);
            if (p==self) {
                selfScore+=evaluatePiece(g,x,y,self);
            }
            else if (p==opponent) {
                opponentScore+=evaluatePiece(g,x,y,opponent);
            }
        }
    }

    return selfScore-opponentScore*defendWeight;
}

bool Minimax::checkWin(const GoGame& g,player p) const {
    int size=g.getSize();
    for (int x=1;x<=size;x++) {
        for (int y=1;y<=size;y++) {
            if (g.getPlayer(x,y)==p&&g.Win(x,y,p)) {
                return true;
            }
        }
    }
    return false;
}

std::vector<std::pair<int,int>> Minimax::getCandidateMoves(const GoGame& g) const {
    int size=g.getSize();
    std::vector<std::vector<bool>> visited(size+1,std::vector(size+1,false));
    std::vector<std::pair<int,int>> candidates;

    // 收集所有已有棋子周围2格内的空位
    for (int x=1;x<=size;x++)
    for (int y=1;y<=size;y++)
    if (g.getPlayer(x,y)!=NONE)
    for (int dx=-2;dx<=2;dx++)
    for (int dy=-2;dy<=2;dy++) {
        int nx=x+dx,ny=y+dy;
        if (!g.outOfRange(nx,ny)&&g.getPlayer(nx,ny)==NONE&&!visited[nx][ny]) {
            candidates.push_back({nx,ny});
            visited[nx][ny]=true;
        }
    }

    // 如果棋盘为空，返回中心
    if (candidates.empty()) {
        candidates.push_back({size/2, size/2});
        return candidates;
    }

    std::sort(candidates.begin(),candidates.end(),
        [&](const std::pair<int,int>& a,const std::pair<int,int>& b) {
            return evaluateEmpty(g,a.first,a.second)>evaluateEmpty(g,b.first,b.second);
        });
    
    // 限制数量
    if (candidates.size()>15){
        candidates.resize(15);   
    }

    return candidates;
}

int Minimax::alphaBeta(GoGame& g,int depth,int alpha,int beta,bool isMaximizing) {
    nodeCount++;

    if (checkWin(g,self)) return 1000000+depth;
    if (checkWin(g,opponent)) return -(1000000+depth);

    if (depth==0||g.GameOver()) return evaluate(g);

    auto moves=getCandidateMoves(g);
    if (moves.empty()) return 0;

    if (isMaximizing){
        int maxValue=INT_MIN;
        for (auto& [x,y]:moves) {
            g.set(x,y,self);
            if (g.Win(x,y,self)) {
                g.undo(x,y);
                return 1000000+depth;
            }
            int value=alphaBeta(g,depth-1,alpha,beta,!isMaximizing);
            g.undo(x,y);
            maxValue=std::max(maxValue,value);
            alpha=std::max(alpha,value);
            if (beta<=alpha) break;
        }
        return maxValue;
    }
    else {
        int minValue=INT_MAX;
        for (auto& [x,y]:moves) {
            g.set(x,y,opponent);
            if (g.Win(x,y,opponent)) {
                g.undo(x,y);
                return -(1000000+depth);
            }
            int value=alphaBeta(g,depth-1,alpha,beta,!isMaximizing);
            g.undo(x,y);
            minValue=std::min(minValue,value);
            beta=std::min(beta,value);
            if (beta<=alpha) break;
        }
        return minValue;
    }
}

Minimax::Minimax(player self_value,int depth_value,double defendWeight_value):
    self(self_value),
    opponent(self_value==BLACK? WHITE:BLACK),
    depth(depth_value),
    nodeCount(0),
    defendWeight(defendWeight_value) {}

std::pair<int,int> Minimax::getBestMove(GoGame& g) {
    nodeCount=0;
    int bestScore=INT_MIN;
    std::pair<int,int> bestMove={-1,-1};
    auto moves=getCandidateMoves(g);

    for (auto& [x,y]:moves) {
        g.set(x,y,self);
        if (g.Win(x,y,self)) {
            g.undo(x,y);
            return {x,y};
        }
        int value=alphaBeta(g,depth-1,INT_MIN,INT_MAX,false);
        g.undo(x,y);
        if (value>bestScore) {
            bestScore=value;
            bestMove={x,y};
        }
    }
    return bestMove;
}



#endif //GO_MINIMAX_H