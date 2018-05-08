#ifndef ATTACK_H
#define ATTACK_H

#include "mainwindow.h"
#include "player.h"

#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

class Attack
{
public:
    Attack();
    void damage(Player player);
    void animate();

    // Getters & Setters
    void setName(String name) {name_ = name;}
    String getName() {return name_;}
    void setDamageMean(int damageMean) {damageMean_ = damageMean;}
    int getDamageMean() {return damageMean_;}
    void setUltCharge_(int ultCharge) {ultCharge_ = ultCharge;}
    int getUltCharge() {return ultCharge_;}

private :
    String name_;
    vector<int> refSkel_;
    int damageMean_;
    int ultCharge_;
    vector<QImage> animationSprites_;

};

#endif // ATTACK_H
