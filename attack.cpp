#include "attack.h"

Attack::Attack(String name, int damageMean, int ultCharge, QPixmap animationSprites)
{
    name_ = name;
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
void Attack::damage(Player *player, bool ultUsed)
{
    int pv=player->getPv();
    int dmg=floor(damageMean_*(70+rand()%60)/100); // damage between 70% and 130% of damageMean.
    if (ultUsed){
        player->setPv(pv-(dmg*2));
    }
    else{
        player->setPv(pv-dmg);
    }
}
