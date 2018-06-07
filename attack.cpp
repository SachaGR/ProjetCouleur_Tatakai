#include "attack.h"

Attack::Attack(String name, int damageMean, int ultCharge)
{
    name_ = name;
    damageMean_ = damageMean;
    ultCharge_ = ultCharge;
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
    int HP=player->getHP();
    int dmg=floor(damageMean_*(70+rand()%60)/100); // damage between 70% and 130% of damageMean.
    (ultUsed) ? player->setHP(HP-(dmg*2)): player->setHP(HP-dmg);
}
