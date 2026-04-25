#include"board.h"
#include<iostream>

void show(int currentTurn,player p) {
    std::cout<<"current turn is "<<currentTurn<<std::endl;
    std::cout<<"current player is "<<(p==BLACK? "black":"white")<<std::endl;
}

int main () {
    GoGame g;
    int currentTurn=0;
    while (!g.GameOver()) {
        g.show();
        currentTurn++;
        player p=CurrentPlayer(currentTurn);
        show(currentTurn,p);
        int x,y;
        std::cin>>x>>y;
        if (!g.validPosition(x,y)) {
            std::cout<<"invalid position"<<std::endl;
            currentTurn--;
            continue;
        }
        g.set(x,y,p);
        if (g.Win(x,y,p)) {
            std::cout<<(p==BLACK? "black":"white")<<" win"<<std::endl;
            break;
        }
        if (g.GameOver()) {
            std::cout<<"game over"<<std::endl;
        }
    }
    return 0;
}
