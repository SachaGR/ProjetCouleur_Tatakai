#ifndef PLAYER_H
#define PLAYER_H

#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <QImage>

using namespace std;
using namespace cv;

class Player
{
public:
    Player(String name, int pv, int score, bool shieldEnable, int shieldCD, QImage photo);

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

private :
    String name_;
    int pv_ = 10;
    int score_ = 0;
    bool shieldEnable_ = true;
    int shieldCD_ = 2;
};

#endif // PLAYER_H
