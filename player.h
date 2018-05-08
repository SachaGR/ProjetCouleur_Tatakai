#ifndef PLAYER_H
#define PLAYER_H

#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

class Player
{
public:
    Player();

    //Getters & Setters
    void setName(String name) {name_ = name;}
    String getName() {return name_;}
    void setPv(int pv) {pv_ = pv;}
    int getPv() {return pv_;}
    void setScore(int score) {score_ = score;}
    int getScore() {return score_;}
    void setShieldEnable(bool shieldEnable) {shieldEnable_ = shieldEnable;}
    bool isShieldEnable() {return shieldEnable_;}
    void setShieldCD(int shieldCD) {shieldCD_ = shieldCD;}
    int getShieldCD() {return shieldCD_;}
    void setPicture(Mat picture) {picture_ = picture;}
    Mat getPicture() {return picture_;}



private :
    String name_;
    int pv_;
    int score_;
    bool shieldEnable_;
    int shieldCD_;
    Mat picture_;
};

#endif // PLAYER_H
