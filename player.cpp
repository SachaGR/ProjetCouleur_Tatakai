#include "player.h"

Player::Player(String name, int pv, int score, bool shieldEnable, int shieldCD)
{
    name_ = name;
    pv_ = pv;
    score_ = score;
    shieldEnable_ = shieldEnable;
    shieldCD_ = shieldCD;
}


