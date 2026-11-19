#ifndef CARTE_H
#define CARTE_H

class Carte {
private:
    int id;
    bool estRetournee;
    bool estTrouvee;

public:
    Carte(); // Constructeur par défaut
    Carte(int id); // Constructeur avec paramètre

    int getId() const;
    bool getEstRetournee() const;
    void setEstRetournee(bool b);

    bool getEstTrouvee() const;
    void setEstTrouvee(bool b);
};

#endif // CARTE_H