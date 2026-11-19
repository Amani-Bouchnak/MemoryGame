#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <vector>
#include "jeu.h"
#include "score.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void lancerPartie();
    void retourAccueil();
    void onCarteClicked(int index);
    void updateChrono();

private:
    void setupPageAccueil();
    void setupPageJeu();
    void applyStylesheet();
    void creerGrilleGraphique();
    void mettreAJourAffichage();
    void mettreAJourLabelRecordAccueil();
    void animerCarte(QPushButton *btn);

    // Moteur du jeu
    Jeu jeu;
    Score gestionnaireScores;
    std::vector<QPushButton*> boutonsCartes;
    bool verrouClic; // Empêche de cliquer pendant l'animation

    // Gestion des 2 pages
    QStackedWidget *stackedWidget;
    QWidget *pageAccueil;
    QWidget *pageJeu;

    // Éléments Page 1 (Accueil)
    QComboBox *comboDimension;
    QLabel *labelRecordAccueil;
    QPushButton *btnJouer;
    QPushButton *btnResetScores;

    // Éléments Page 2 (Jeu)
    QLabel *labelCoups;
    QLabel *labelPaires;
    QLabel *labelChrono;
    QLabel *labelMeilleurScore;
    QPushButton *btnAccueil;
    QPushButton *btnRejouer;
    QWidget *gridContainer;
    QGridLayout *gridLayout;

    // Chronomètre
    QTimer *chronoTimer;
    int secondes;

    // Condintions d'utilisation
    bool conditionsAcceptees;
    QPushButton *btnConditionsLink;
    void afficherConditionsUtilisation(bool lancerApresAcceptation = false);
};

#endif // MAINWINDOW_H