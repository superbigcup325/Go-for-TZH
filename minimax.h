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
    const double defendWeight=1.0;
    int depth;
    int nodeCount;

    struct Pattern {
        int count=1;
        int openEnds=0;

        int score() const {
            if (count >= 5) return 1000000;  // 五连
            if (count == 4) {
                if (openEnds == 2) return 100000;  // 活四
                if (openEnds == 1) return 10000;   // 死四
            }
            if (count == 3) {
                if (openEnds == 2) return 10000;   // 活三
                if (openEnds == 1) return 1000;    // 死三
            }
            if (count == 2) {
                if (openEnds == 2) return 1000;    // 活二
                if (openEnds == 1) return 100;     // 死二
            }
            if (count == 1) {
                if (openEnds == 2) return 100;     // 活一
                if (openEnds == 1) return 10;      // 死一
            }
            return 0;
        }
    };

    Pattern analyzeDirection(const GoGame& g,int x,int y,int dx,int dy,player p) const;
    int evaluatePosition(const GoGame& g,int x,int y,player p) const ;
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

// 单个空位威胁评估
int Minimax::evaluatePosition(const GoGame& g,int x,int y,player p) const {
    if (g.getPlayer(x,y)!=NONE) return 0;
    int score=0;
    std::vector<std::pair<int,int>> directions={
        {0,1},{1,0},{1,1},{1,-1}
    };
    for (auto& [dx,dy]:directions) {
        score+=analyzeDirection(g,x,y,dx,dy,p).score();
    }
    return score;
}

// 全局评估
double Minimax::evaluate(const GoGame& g) const {
    int selfScore=0;
    int opponentScore=0;
    int size=g.getSize();

    // 遍历，计算每个空位的威胁
    for (int x=1;x<=size;x++) {
        for (int y=1;y<=size;y++) {
            if (g.getPlayer(x,y)==NONE){
                selfScore+=evaluatePosition(g,x,y,self);
                opponentScore+=evaluatePosition(g,x,y,opponent);
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

    // 排序加快剪枝
    std::sort(candidates.begin(),candidates.end(),
        [&](const std::pair<int,int>& a,const std::pair<int,int>& b) {
            int scoreA=evaluatePosition(g,a.first,a.second,BLACK)+evaluatePosition(g,a.first,a.second,WHITE);
            int scoreB=evaluatePosition(g,b.first,b.second,BLACK)+evaluatePosition(g,b.first,b.second,WHITE);
            return scoreA>scoreB;
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