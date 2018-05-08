#ifndef GAME_H
#define GAME_H

#include "attack.h"
#include "player.h"

#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

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

    //Getters & Setters
    void setUltimateCharge(int ultimateCharge) {ultimateCharge_ = ultimateCharge;}
    int getUltimateCharge() {return ultimateCharge_;}
    void setActivePlayer(int activePlayer) {activePlayer_ = activePlayer;}
    int getActivePlayer() {return activePlayer_;}
    void setRound(int round) {round_ = round;}
    int getRound() {return round_;}
    void setBackground(Mat background) {background_ = background;}
    Mat getBackground() {return background_;}

private :
    int ultimateCharge_;
    vector<Player> players_;
    vector<Attack> attacks_;
    int activePlayer_;
    int round_;
    vector<Attack> posKept_;
    VideoCapture camera_;
    Mat background_;
};

#endif // GAME_H
