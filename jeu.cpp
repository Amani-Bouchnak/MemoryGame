#include "jeu.h"
#include "plateau.h"

Jeu::Jeu(int dim)
{
    demarrerNouvellePartie(dim);
}
Plateau& Jeu::getPlateau()
{
    return plateau;
}
int Jeu::getCoups() const
{
    return coups;
}
int Jeu::getPairesTrouvees() const
{
    return pairesTrouvees;
}
bool Jeu::deuxCartesRetournees() const
{
    return indexDeuxiemeCarte != -1;
}
void Jeu::demarrerNouvellePartie(int dim)
{
    plateau=Plateau(dim);
    plateau.initialiserPlateau();
    indexPremiereCarte=-1; //aucune carte n'est retournée
    indexDeuxiemeCarte=-1; //une seule (ou aucune) carte est retournée
    coups=0;
    pairesTrouvees=0;
}
bool Jeu::retournerCarte(int index)
{
    if(index>=0 && plateau.getNombreCartes()>index && indexDeuxiemeCarte == -1)
    {
        Carte& c=plateau.getCarte(index);
        if(!c.getEstRetournee() && !c.getEstTrouvee())
        {
            c.setEstRetournee(true);
            if(indexPremiereCarte==-1)
                indexPremiereCarte=index;
            else
            {
                indexDeuxiemeCarte=index;
                coups++;
            }
            return true;
        }
        return false;
    }
    return false;
}
bool Jeu::verifierPaire()
{
    Carte& c1=plateau.getCarte(indexPremiereCarte);
    Carte& c2=plateau.getCarte(indexDeuxiemeCarte);
    if(c1.getId()==c2.getId())
    {
        pairesTrouvees++;
        c1.setEstTrouvee(true);
        c2.setEstTrouvee(true);
        indexPremiereCarte=-1;
        indexDeuxiemeCarte=-1;
        return true;
    }
    return false;
}
void Jeu::cacherCartesNonValidees()
{
    if (indexPremiereCarte != -1 && indexDeuxiemeCarte != -1)
    {
        Carte& c1=plateau.getCarte(indexPremiereCarte);
        Carte& c2=plateau.getCarte(indexDeuxiemeCarte);
        c1.setEstRetournee(false);
        c2.setEstRetournee(false);
        indexPremiereCarte=-1;
        indexDeuxiemeCarte=-1;
    }
}
bool Jeu::estPartieTerminee() const
{
    return pairesTrouvees == plateau.getNombreCartes() / 2;
}



