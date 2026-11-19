#ifndef SCORE_H
#define SCORE_H

#include <string>
#include <map>

struct Record {
    int coups;
    int secondes;
};

class Score
{
private:
    std::string nomFichier;
    // Associe chaque taille de plateau (4, 6, 8, 12, 16...) à son meilleur Record
    std::map<int, Record> meilleursScores;

    void charger();
    void sauvegarder() const;

public:
    Score(const std::string& fichier = "scores.txt");

    // Enregistre le score s'il bat le record actuel (retourne true si nouveau record)
    bool enregistrerScore(int dimension, int coups, int secondes);

    // Indique si un record a déjà été établi pour cette dimension
    bool aUnRecord(int dimension) const;

    // Retourne le record enregistré pour cette dimension (ou {-1, -1} si aucun)
    Record getRecord(int dimension) const;

    // Remet tous les records à zéro et vide le fichier de sauvegarde
    void reinitialiser();
};

#endif // SCORE_H