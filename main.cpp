#include"board.h"
#include"minimax.h"
#include<iostream>

void show(int currentTurn,player p) {
    std::cout<<"current turn is "<<currentTurn<<std::endl;
    std::cout<<"current player is "<<(p==BLACK? "black":"white")<<std::endl;
}

int main () {
    GoGame g;
    Minimax minimax(WHITE,4,1.1);
    int currentTurn=0;
    while (!g.GameOver()) {
        g.show();
        currentTurn++;
        int x,y;
        player p=CurrentPlayer(currentTurn);
        if (p==minimax.getSelf()){
            auto move=minimax.getBestMove(g);
            x=move.first,y=move.second;
            if (x==-1||y==-1){
                std::cout<<"cant find"<<std::endl;
            }
            g.set(x,y,p);
        }
        else {
            // std::cin>>x>>y;
            // if(!g.outOfRange(x,y)){
            //     g.set(x,y,p);
            // }
            auto move=minimax.getBestMove(g);
            x=move.first,y=move.second;
            if (x==-1||y==-1){
                std::cout<<"cant find"<<std::endl;
            }
            g.set(x,y,p);
        }
        if (g.Win(x,y,p)){
            std::cout<<(p==BLACK? "BLACK":"WHITE")<<" win"<<std::endl;
            g.show();
            break;
        }
    }
    return 0;
}
