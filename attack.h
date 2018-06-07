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
    Attack(String name, int damageMean, int ultCharge);
    void damage(Player *player, bool ultUsed);

    // Getters & Setters
    int getUltCharge() {return ultCharge_;}

private :
    String name_;
    int damageMean_;
    int ultCharge_;
};

#endif // ATTACK_H
