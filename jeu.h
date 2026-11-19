#ifndef JEU_H
#define JEU_H

#include "plateau.h"

class Jeu {
private:
    Plateau plateau;
    int indexPremiereCarte; // -1 si aucune carte n'est retournée
    int indexDeuxiemeCarte; // -1 si une seule (ou aucune) carte est retournée
    int coups;
    int pairesTrouvees;

public:
    Jeu(int dim = 8);

    // Lance ou relance une partie
    void demarrerNouvellePartie(int dim);

    // Gère le clic logique sur la carte à la position 'index'
    // Retourne true si la carte a bien pu être retournée
    bool retournerCarte(int index);

    // Vérifie si les 2 cartes retournées sont identiques (met à jour estTrouvee)
    bool verifierPaire();

    // Cache à nouveau les 2 cartes si elles ne correspondaient pas
    void cacherCartesNonValidees();

    // Indique si toutes les paires ont été trouvées
    bool estPartieTerminee() const;

    // Accesseurs utiles pour l'interface graphique (MainWindow)
    Plateau& getPlateau();
    int getCoups() const;
    int getPairesTrouvees() const;
    bool deuxCartesRetournees() const;
};

#endif // JEU_H