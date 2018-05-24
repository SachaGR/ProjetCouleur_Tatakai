#include "attack.h"

Attack::Attack(String name, vector<int> refSkel, int damageMean, int ultCharge, vector<QImage> animationSprites)
{
    name_ = name;
    refSkel_ = refSkel;
    damageMean_ = damageMean;
    ultCharge_ = ultCharge;
    animationSprites_ = animationSprites;
}

/* Inflict damages to the defined player
 *
 * Input:
 *  - Player player: player to attack
 *  - bool ultUsed: determine if the ultimate is used or not
 *
 * Output:
 *
*/
void Attack::damage(Player player, bool ultUsed)
{
    float pv=player.getPv();
    float dmg=damageMean_*(70+rand()%60)/100; // damage between 70% and 130% of damageMean.
    if (ultUsed){
        player.setPv(pv-dmg*2);
    }
    else{
        player.setPv(pv-dmg);
    }
}

