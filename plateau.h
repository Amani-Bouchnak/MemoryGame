#ifndef PLATEAU_H
#define PLATEAU_H

#include <vector>
#include "carte.h"

class Plateau {
private:
    int lignes;
    int colonnes;
    std::vector<Carte> cartes; // Contient toutes les cartes du plateau

public:
    // Constructeur : initialise les dimensions (ex: 4 lignes, 4 colonnes pour 16 cartes)
    Plateau(int dim=8);

    // Initialise les paires de cartes et les mélange
    void initialiserPlateau();

    // Permet de récupérer une carte à une position précise (pour l'affichage ou la logique)
    Carte& getCarte(int index);

    // Getters pour les dimensions
    int getLignes() const;
    int getColonnes() const;
    int getNombreCartes() const;
};

#endif // PLATEAU_H