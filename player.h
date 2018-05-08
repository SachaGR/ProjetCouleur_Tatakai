#ifndef PLAYER_H
#define PLAYER_H

#include "mainwindow.h"

using namespace std;
using namespace cv;

class Player
{
public:
    Player();

    //Getters & Setters


private :
    String name_;
    int pv_;
    int score_;
    bool shieldEnable_;
    int shieldCD_;
    Mat picture_;
};

#endif // PLAYER_H
