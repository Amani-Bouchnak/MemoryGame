#include "score.h"
#include <fstream>

Score::Score(const std::string& fichier): nomFichier(fichier)
{
    charger();
}
void Score::charger()
{
    std::ifstream fichier(nomFichier);
    if(!fichier.is_open()) return;
    int dim,coups,sec;
    while(fichier>>dim>>coups>>sec)
    {
        meilleursScores[dim] = {coups, sec};
    }
}
void Score::sauvegarder() const
{
    std::ofstream fichier(nomFichier);
    if(!fichier.is_open()) return;
    for (const auto& [dim, rec] : meilleursScores)
    {
        fichier<<dim<<" "<<rec.coups<<" "<<rec.secondes<<"\n";
    }
}
bool Score::aUnRecord(int dimension) const
{
    return meilleursScores.find(dimension) != meilleursScores.end();
}

Record Score::getRecord(int dimension) const
{
    auto it = meilleursScores.find(dimension);

    if (it != meilleursScores.end()) {
        return it->second; // it->second contient la structure Record (coups et secondes)
    }

    return {100, 1000};
}
bool Score::enregistrerScore(int dimension, int coups, int secondes)
{
    if(!aUnRecord(dimension) || coups<meilleursScores[dimension].coups || (coups==meilleursScores[dimension].coups && secondes<meilleursScores[dimension].secondes) )
    {
        Record r={coups,secondes};
        meilleursScores[dimension]=r;
        sauvegarder();
        return true;
    }
    return false;
}
void Score::reinitialiser()
{
    meilleursScores.clear();
    sauvegarder();
}












