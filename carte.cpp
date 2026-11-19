#include "carte.h"

Carte::Carte()
{
    id=00;
    estRetournee=false;
    estTrouvee=false;
}
Carte::Carte(int id)
{
        this->id=id;
        estRetournee=false;
        estTrouvee=false;
}
int Carte::getId() const { return id; }
bool Carte::getEstRetournee() const { return estRetournee; }
void Carte::setEstRetournee(bool b){estRetournee=b;}
bool Carte::getEstTrouvee() const { return estTrouvee; }
void Carte::setEstTrouvee(bool b){estTrouvee=b;}