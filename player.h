#ifndef PLAYER_H
#define PLAYER_H

#include <QImage>

#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

class Player
{
public:
    Player(int HP, int score);

    //Getters & Setters
    void setHP(int HP) {HP_ = HP;}
    int getHP() {return HP_;}
    void setScore(int score) {score_ = score;}
    int getScore() {return score_;}

private :
    int HP_ = 100;
    int score_ = 0;
};

#endif // PLAYER_H
