#include "mainwindow.h"
#include <QMessageBox>
#include <QIcon>
#include <QPixmap>
#include <QFile>
#include <QEasingCurve>
#include <QStyle>
#include <QDialog>
#include <QTextBrowser>
#include <QApplication>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), jeu(8), gestionnaireScores("scores.txt"), verrouClic(false), secondes(0),conditionsAcceptees(false)
{
    QSettings settings("MemoryGame", "MemoryGame");
    settings.setValue("conditionsAcceptees", false);
    conditionsAcceptees = false;//Ces 3 lignes sont pour tester les conditions d'utilisation
    //quand tu les met enlève la ligne 37

    setWindowTitle("Memory Game - POO C++ / Qt");
    resize(1000, 800);
    showMaximized();

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    setupPageAccueil();
    setupPageJeu();

    stackedWidget->addWidget(pageAccueil); // Index 0 : Accueil
    stackedWidget->addWidget(pageJeu);     // Index 1 : Jeu

    chronoTimer = new QTimer(this);
    connect(chronoTimer, &QTimer::timeout, this, &MainWindow::updateChrono);

    //QSettings settings("MemoryGame", "MemoryGame");//ligne 37
    conditionsAcceptees = settings.value("conditionsAcceptees", false).toBool();

    applyStylesheet();
}

MainWindow::~MainWindow() {}

void MainWindow::setupPageAccueil()
{
    pageAccueil = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(pageAccueil);
    mainLayout->setContentsMargins(25, 20, 25, 20);

    // Barre supérieure pour l'accueil (bouton reset en haut à droite)
    QHBoxLayout *topBarAccueil = new QHBoxLayout();
    btnResetScores = new QPushButton("Réinitialiser les scores", pageAccueil);
    btnResetScores->setObjectName("btnAction");
    btnResetScores->setCursor(Qt::PointingHandCursor);
    topBarAccueil->addStretch();
    topBarAccueil->addWidget(btnResetScores);

    mainLayout->addLayout(topBarAccueil);
    mainLayout->addStretch();

    QLabel *titre = new QLabel("MEMORY GAME", pageAccueil);
    titre->setObjectName("titreAccueil");
    titre->setAlignment(Qt::AlignCenter);

    QLabel *sousTitre = new QLabel("Choisissez la taille du plateau :", pageAccueil);
    sousTitre->setObjectName("sousTitreAccueil");
    sousTitre->setAlignment(Qt::AlignCenter);

    comboDimension = new QComboBox(pageAccueil);
    comboDimension->setObjectName("comboDim");
    comboDimension->addItem("4 cartes (2 x 2) — Très facile", 4);
    comboDimension->addItem("6 cartes (2 x 3) — Facile", 6);
    comboDimension->addItem("8 cartes (2 x 4) — Moyen", 8);
    comboDimension->addItem("12 cartes (3 x 4) — Difficile", 12);
    comboDimension->addItem("16 cartes (4 x 4) — Expert", 16);
    comboDimension->setCurrentIndex(2); // 8 cartes par défaut
    comboDimension->setFixedWidth(320);

    // Label pour afficher le record sur l'accueil
    labelRecordAccueil = new QLabel(pageAccueil);
    labelRecordAccueil->setObjectName("labelRecordAccueil");
    labelRecordAccueil->setAlignment(Qt::AlignCenter);
    mettreAJourLabelRecordAccueil();

    btnJouer = new QPushButton("JOUER", pageAccueil);
    btnJouer->setObjectName("btnJouer");
    btnJouer->setFixedSize(220, 56);
    btnJouer->setCursor(Qt::PointingHandCursor);

    connect(btnJouer, &QPushButton::clicked, this, &MainWindow::lancerPartie);
    connect(comboDimension, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int){
        mettreAJourLabelRecordAccueil();
    });
    connect(btnResetScores, &QPushButton::clicked, this, [this]() {
        gestionnaireScores.reinitialiser();
        mettreAJourLabelRecordAccueil();
        QMessageBox::information(this, "Réinitialisation", "Tous les scores ont été effacés !");
    });

    mainLayout->addWidget(titre, 0, Qt::AlignCenter);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(sousTitre, 0, Qt::AlignCenter);
    mainLayout->addSpacing(15);
    mainLayout->addWidget(comboDimension, 0, Qt::AlignCenter);
    mainLayout->addSpacing(15);
    mainLayout->addWidget(labelRecordAccueil, 0, Qt::AlignCenter);
    mainLayout->addSpacing(25);
    mainLayout->addWidget(btnJouer, 0, Qt::AlignCenter);
    mainLayout->addStretch();

    btnConditionsLink = new QPushButton("Conditions d'utilisation", pageAccueil);
    btnConditionsLink->setCursor(Qt::PointingHandCursor);
    btnConditionsLink->setFlat(true);
    btnConditionsLink->setFocusPolicy(Qt::NoFocus);
    btnConditionsLink->setStyleSheet(
        "QPushButton {"
        "  background: transparent;"
        "  border: none;"
        "  outline: none;"
        "  color: #94A3B8;"
        "  text-decoration: underline;"
        "  font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "  color: #38BDF8;"
        "}"
        );

    connect(btnConditionsLink, &QPushButton::clicked, this, [this]() {
        afficherConditionsUtilisation(false);
    });
    mainLayout->addWidget(btnConditionsLink, 0, Qt::AlignCenter);
}

void MainWindow::setupPageJeu()
{
    pageJeu = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(pageJeu);
    mainLayout->setContentsMargins(25, 20, 25, 20);
    mainLayout->setSpacing(15);

    // Barre supérieure (Navigation + Stats)
    QHBoxLayout *topBar = new QHBoxLayout();

    btnAccueil = new QPushButton("← Menu", pageJeu);
    btnAccueil->setObjectName("btnAction");
    btnAccueil->setCursor(Qt::PointingHandCursor);

    labelCoups = new QLabel("Coups : 0", pageJeu);
    labelCoups->setObjectName("statLabel");

    labelPaires = new QLabel("Paires : 0", pageJeu);
    labelPaires->setObjectName("statLabel");

    labelChrono = new QLabel("Temps : 00:00", pageJeu);
    labelChrono->setObjectName("statLabel");

    labelMeilleurScore = new QLabel("Record : -", pageJeu);
    labelMeilleurScore->setObjectName("statLabel");

    btnRejouer = new QPushButton("↻ Rejouer", pageJeu);
    btnRejouer->setObjectName("btnAction");
    btnRejouer->setCursor(Qt::PointingHandCursor);

    topBar->addWidget(btnAccueil);
    topBar->addStretch();
    topBar->addWidget(labelCoups);
    topBar->addSpacing(15);
    topBar->addWidget(labelPaires);
    topBar->addSpacing(15);
    topBar->addWidget(labelChrono);
    topBar->addSpacing(15);
    topBar->addWidget(labelMeilleurScore);
    topBar->addStretch();
    topBar->addWidget(btnRejouer);

    // Zone centrale de la grille
    gridContainer = new QWidget(pageJeu);
    gridLayout = new QGridLayout(gridContainer);
    gridLayout->setSpacing(14);
    gridLayout->setAlignment(Qt::AlignCenter);

    mainLayout->addLayout(topBar);
    mainLayout->addWidget(gridContainer, 1);

    connect(btnAccueil, &QPushButton::clicked, this, &MainWindow::retourAccueil);
    connect(btnRejouer, &QPushButton::clicked, this, &MainWindow::lancerPartie);
}

void MainWindow::mettreAJourLabelRecordAccueil()
{
    int dim = comboDimension->currentData().toInt();
    if (gestionnaireScores.aUnRecord(dim)) {
        Record r = gestionnaireScores.getRecord(dim);
        int min = r.secondes / 60;
        int sec = r.secondes % 60;
        labelRecordAccueil->setText(QString("Meilleur score : %1 coups (%2:%3)")
                                        .arg(r.coups)
                                        .arg(min, 2, 10, QChar('0'))
                                        .arg(sec, 2, 10, QChar('0')));
    } else {
        labelRecordAccueil->setText("Aucun record pour cette dimension");
    }
}

void MainWindow::lancerPartie()
{
    if (!conditionsAcceptees) {
        afficherConditionsUtilisation(true);
        return;
    }
    int dim = comboDimension->currentData().toInt();
    jeu.demarrerNouvellePartie(dim);

    secondes = 0;
    verrouClic = false;
    labelChrono->setText("Temps : 00:00");

    // Afficher le record actuel dans la barre de jeu
    if (gestionnaireScores.aUnRecord(dim)) {
        Record r = gestionnaireScores.getRecord(dim);
        labelMeilleurScore->setText(QString("Record : %1 coups (%2s)").arg(r.coups).arg(r.secondes));
    } else {
        labelMeilleurScore->setText("Record : Aucun");
    }

    chronoTimer->start(1000);

    creerGrilleGraphique();
    mettreAJourAffichage();

    // Basculer sur la page 2 (Jeu)
    stackedWidget->setCurrentIndex(1);
}

void MainWindow::retourAccueil()
{
    chronoTimer->stop();
    mettreAJourLabelRecordAccueil();
    stackedWidget->setCurrentIndex(0);
}

void MainWindow::creerGrilleGraphique()
{
    // Nettoyer les anciens boutons
    for (QPushButton *btn : boutonsCartes) {
        gridLayout->removeWidget(btn);
        delete btn;
    }
    boutonsCartes.clear();

    Plateau &p = jeu.getPlateau();
    int lignes = p.getLignes();
    int colonnes = p.getColonnes();

    // Adapter la taille des cartes selon la grille
    int tailleCarte = (lignes <= 2) ? 145 : (lignes == 3 ? 125 : 110);

    for (int i = 0; i < p.getNombreCartes(); ++i) {
        QPushButton *btn = new QPushButton(gridContainer);
        btn->setFixedSize(tailleCarte, tailleCarte);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setProperty("etat", "cachee");

        int row = i / colonnes;
        int col = i % colonnes;
        gridLayout->addWidget(btn, row, col);

        boutonsCartes.push_back(btn);

        connect(btn, &QPushButton::clicked, this, [this, i]() {
            onCarteClicked(i);
        });
    }
}

void MainWindow::animerCarte(QPushButton *btn)
{
    // Effet de fondu + zoom visuel lors du retournement
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(btn);
    btn->setGraphicsEffect(eff);

    QPropertyAnimation *anim = new QPropertyAnimation(eff, "opacity", btn);
    anim->setDuration(220);
    anim->setStartValue(0.25);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::onCarteClicked(int index)
{
    if (verrouClic) return;

    if (jeu.retournerCarte(index)) {
        animerCarte(boutonsCartes[index]);
        mettreAJourAffichage();

        if (jeu.deuxCartesRetournees()) {
            if (jeu.verifierPaire()) {
                mettreAJourAffichage();

                if (jeu.estPartieTerminee()) {
                    chronoTimer->stop();

                    int dim = comboDimension->currentData().toInt();
                    int coupsFait = jeu.getCoups();
                    int tempsFait = secondes;

                    // Enregistrement du score
                    bool nouveauRecord = gestionnaireScores.enregistrerScore(dim, coupsFait, tempsFait);

                    // Mise à jour immédiate du label de score dans la partie
                    Record r = gestionnaireScores.getRecord(dim);
                    labelMeilleurScore->setText(QString("Record : %1 coups (%2s)").arg(r.coups).arg(r.secondes));

                    QTimer::singleShot(300, this, [this, nouveauRecord, coupsFait]() {
                        QString msg = QString("Félicitations ! Plateau terminé en %1 coups (%2) !")
                                          .arg(coupsFait)
                                          .arg(labelChrono->text().mid(8));
                        if (nouveauRecord) {
                            msg += "\n\n🏆 Nouveau Record Établi !";
                        }
                        QMessageBox::information(this, "Victoire !", msg);
                    });
                }
            } else {
                verrouClic = true;
                QTimer::singleShot(850, this, [this]() {
                    jeu.cacherCartesNonValidees();
                    mettreAJourAffichage();
                    verrouClic = false;
                });
            }
        }
    }
}

void MainWindow::mettreAJourAffichage()
{
    Plateau &p = jeu.getPlateau();
    labelCoups->setText(QString("Coups : %1").arg(jeu.getCoups()));
    labelPaires->setText(QString("Paires : %1 / %2")
                             .arg(jeu.getPairesTrouvees())
                             .arg(p.getNombreCartes() / 2));

    for (int i = 0; i < p.getNombreCartes(); ++i) {
        Carte &c = p.getCarte(i);
        QPushButton *btn = boutonsCartes[i];

        if (c.getEstTrouvee() || c.getEstRetournee()) {
            // Chemin de l'image embarquée dans les ressources Qt
            QString cheminImage = QString(":/images/%1.png").arg(c.getId());

            if (QFile::exists(cheminImage)) {
                btn->setText("");
                btn->setIcon(QIcon(QPixmap(cheminImage)));
                btn->setIconSize(QSize(btn->width() - 18, btn->height() - 18));
            } else {
                // Secours si l'image n'est pas encore ajoutée : affiche le numéro
                btn->setIcon(QIcon());
                btn->setText(QString::number(c.getId()));
            }

            btn->setProperty("etat", c.getEstTrouvee() ? "trouvee" : "visible");
        } else {
            btn->setIcon(QIcon());
            btn->setText("?");
            btn->setProperty("etat", "cachee");
        }

        // Forcer Qt à réappliquer le style CSS selon la propriété "etat"
        btn->style()->unpolish(btn);
        btn->style()->polish(btn);
    }
}

void MainWindow::updateChrono()
{
    secondes++;
    int min = secondes / 60;
    int sec = secondes % 60;
    labelChrono->setText(QString("Temps : %1:%2")
                             .arg(min, 2, 10, QChar('0'))
                             .arg(sec, 2, 10, QChar('0')));
}

void MainWindow::applyStylesheet()
{
    setStyleSheet(R"(
        QMainWindow, QWidget {
            background-color: #161822;
            color: #F1F5F9;
            font-family: 'Segoe UI', Arial, sans-serif;
        }
        #titreAccueil {
            font-size: 44px;
            font-weight: bold;
            color: #38BDF8;
            letter-spacing: 3px;
        }
        #sousTitreAccueil {
            font-size: 18px;
            color: #94A3B8;
        }
        #labelRecordAccueil {
            font-size: 16px;
            color: #38BDF8;
            font-weight: bold;
        }
        #comboDim {
            background-color: #1E293B;
            border: 2px solid #334155;
            border-radius: 10px;
            padding: 12px 16px;
            font-size: 16px;
            color: #F8FAFC;
        }
        #comboDim::drop-down {
            border: none;
        }
        #btnJouer {
            background-color: #0284C7;
            color: white;
            font-size: 20px;
            font-weight: bold;
            border-radius: 14px;
            letter-spacing: 2px;
        }
        #btnJouer:hover {
            background-color: #0369A1;
        }
        #btnAction {
            background-color: #1E293B;
            border: 1px solid #334155;
            border-radius: 8px;
            padding: 8px 16px;
            font-size: 14px;
            font-weight: bold;
        }
        #btnAction:hover {
            background-color: #334155;
        }
        #statLabel {
            font-size: 16px;
            font-weight: bold;
            color: #E2E8F0;
            background-color: #1E293B;
            padding: 8px 16px;
            border-radius: 8px;
        }
        QPushButton[etat="cachee"] {
            background-color: #1E293B;
            border: 2px solid #38BDF8;
            border-radius: 14px;
            font-size: 32px;
            font-weight: bold;
            color: #38BDF8;
        }
        QPushButton[etat="cachee"]:hover {
            background-color: #273549;
            border-color: #7DD3FC;
        }
        QPushButton[etat="visible"] {
            background-color: #F8FAFC;
            border: 3px solid #F59E0B;
            border-radius: 14px;
            font-size: 32px;
            font-weight: bold;
            color: #0F172A;
        }
        QPushButton[etat="trouvee"] {
            background-color: #DCFCE7;
            border: 3px solid #22C55E;
            border-radius: 14px;
            font-size: 32px;
            font-weight: bold;
            color: #14532D;
        }
    )");
}
void MainWindow::afficherConditionsUtilisation(bool lancerApresAcceptation)
{
    QDialog dialog(this);
    dialog.setWindowTitle("Conditions d'utilisation");
    dialog.setModal(true);
    dialog.resize(520, 360);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(14);

    QLabel *titre = new QLabel("Conditions d'utilisation", &dialog);
    titre->setStyleSheet("font-size: 20px; font-weight: bold; color: #38BDF8;");

    QTextBrowser *texte = new QTextBrowser(&dialog);
    texte->setOpenExternalLinks(false);
    texte->setHtml(
        "<p>Bienvenue dans <b>Memory Game</b>.</p>"
        "<p>En acceptant ces conditions d'utilisation, vous acceptez de jouer normalement et "
        "vous êtes informé que vos meilleurs scores (nombre de coups et temps par taille de grille) "
        "sont enregistrés localement dans <code>scores.txt</code>.</p>"
        "<p>Aucune donnée personnelle n'est collectée ni envoyée vers l'extérieur.</p>"
        );
    texte->setStyleSheet(
        "background-color: #1E293B; color: #F1F5F9; border: 1px solid #334155; "
        "border-radius: 8px; padding: 10px; font-size: 14px;"
        );

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    bool accepteMaintenant = false;
    bool refuseDeQuitter = false;

    if (conditionsAcceptees) {
        QPushButton *btnFermer = new QPushButton("Fermer", &dialog);
        btnFermer->setCursor(Qt::PointingHandCursor);
        btnFermer->setStyleSheet(
            "background-color: #0284C7; color: white; border: none; "
            "border-radius: 8px; padding: 10px 18px; font-weight: bold;"
            );
        connect(btnFermer, &QPushButton::clicked, &dialog, &QDialog::accept);
        btnLayout->addWidget(btnFermer);
    } else {
        QPushButton *btnRefuser = new QPushButton("Refuser et quitter", &dialog);
        btnRefuser->setCursor(Qt::PointingHandCursor);
        btnRefuser->setStyleSheet(
            "background-color: #F8FAFC; color: #0F172A; border: 1px solid #CBD5E1; "
            "border-radius: 8px; padding: 10px 16px; font-weight: bold;"
            );

        QPushButton *btnAccepter = new QPushButton("Accepter et continuer", &dialog);
        btnAccepter->setCursor(Qt::PointingHandCursor);
        btnAccepter->setStyleSheet(
            "background-color: #0284C7; color: white; border: none; "
            "border-radius: 8px; padding: 10px 18px; font-weight: bold;"
            );

        connect(btnAccepter, &QPushButton::clicked, &dialog, [&]() {
            conditionsAcceptees = true;
            accepteMaintenant = true;
            QSettings settings("MemoryGame", "MemoryGame");
            settings.setValue("conditionsAcceptees", true);
            dialog.accept();
        });

        connect(btnRefuser, &QPushButton::clicked, &dialog, [&]() {
            refuseDeQuitter = true;
            dialog.reject();
        });

        btnLayout->addWidget(btnRefuser);
        btnLayout->addWidget(btnAccepter);
    }

    layout->addWidget(titre);
    layout->addWidget(texte, 1);
    layout->addLayout(btnLayout);

    dialog.exec();

    if (refuseDeQuitter) {
        qApp->quit();
        return;
    }

    if (accepteMaintenant && lancerApresAcceptation) {
        lancerPartie();
    }
}