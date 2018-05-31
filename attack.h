#ifndef ATTACK_H
#define ATTACK_H

#include "player.h"

#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <cmath>
#include <QPixmap>
#include <QTimer>

using namespace std;
using namespace cv;

class Attack
{
public:
    Attack(String name, int damageMean, int ultCharge, QPixmap animationSprites);
    void damage(Player *player, bool ultUsed);
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
    int damageMean_;
    int ultCharge_;
    QPixmap animationSprites_;
    int currentAnimationFrame_;
};

#endif // ATTACK_H
