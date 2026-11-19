#include "plateau.h"
#include "carte.h"
#include <random>

Plateau::Plateau(int dim)
{
    if(dim==4)
    {
        lignes=2;
        colonnes=2;
    }else
    if(dim==6)
    {
        lignes=2;
        colonnes=3;
    }else
    if(dim==8)
    {
        lignes=2;
        colonnes=4;
    }else
    if(dim==12)
    {
        lignes=3;
        colonnes=4;
    }else
    if(dim==16)
    {
        lignes=4;
        colonnes=4;
    }
}
void Plateau::initialiserPlateau()
{
    // 1. Obtenir une vraie graine aléatoire depuis le système
    std::random_device rd;

    // 2. Initialiser le moteur de génération (Mersenne Twister)
    std::mt19937 gen(rd());

    // 3. Définir une distribution uniforme (bornes incluses)
    std::uniform_int_distribution<int> dis(1,getNombreCartes());

    for(int i=0;i<(getNombreCartes())/2;i++)
    {
        Carte c1(i+1);
        cartes.push_back(c1);
        Carte c2(i+1);
        cartes.push_back(c2);
    }

    int j;
    Carte aux;
    for(int i=0;i<getNombreCartes();i++)
    {
        j=dis(gen);
        aux=cartes[i];
        cartes[i]=cartes[j-1];
        cartes[j-1]=aux;
    }
}
Carte& Plateau::getCarte(int index)
{
    //Carte& c=cartes[index];
    return cartes[index];
}
int Plateau::getLignes() const
{
    return lignes;
}
int Plateau::getColonnes() const
{
    return colonnes;
}
int Plateau::getNombreCartes() const
{
    return lignes*colonnes;
}