#ifndef GAME_H
#define GAME_H

#include "mainwindow.h"

using namespace std;
using namespace cv;

class Game
{
public:
    Game();
    void getCamPic();
    Mat findSkel(Mat frame);
    void updateBG();
    void updatePP(Player player);
    Attack compareSkel(Mat Skel);

private :
    int ultimate_;
    vector<Player> players_;
    vector<Attack> attacks_;
    int activePlayer_;
    int round_;
    vector<Attack> posKept_;
    VideoCapture camera_;
    Mat background_;
};

#endif // GAME_H
