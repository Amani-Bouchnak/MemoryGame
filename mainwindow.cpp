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
#include <QUrl>
#include <QDebug>
#include <QMediaPlayer>
#include <QAudioOutput>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    jeu(8),
    gestionnaireScores("scores.txt"),
    verrouClic(false),
    secondes(0),
    conditionsAcceptees(false),
    modeSombre(true)
{

    QSettings settings("MemoryGame", "MemoryGame");
    modeSombre = settings.value("modeSombre", true).toBool();
    conditionsAcceptees = settings.value("conditionsAcceptees", false).toBool();
    //conditionsAcceptees = false;

    setWindowTitle("Memory Game - POO C++ / Qt");
    resize(1000, 800);
    showMaximized();

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    setupPageAccueil();
    setupPageJeu();

    stackedWidget->addWidget(pageAccueil); // Index 0 : Accueil
    stackedWidget->addWidget(pageJeu);     // Index 1 : Jeu
    stackedWidget->setCurrentIndex(0);

    chronoTimer = new QTimer(this);
    connect(chronoTimer, &QTimer::timeout, this, &MainWindow::updateChrono);

    initialiserSons();

    applyStylesheet();
}

MainWindow::~MainWindow() {}

void MainWindow::setupPageAccueil()
{
    pageAccueil = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(pageAccueil);
    mainLayout->setContentsMargins(25, 20, 25, 20);

    // Barre supérieure pour l'accueil

    btnInfos = new QPushButton("?", pageAccueil);
    btnInfos->setObjectName("btnInfos");
    btnInfos->setCursor(Qt::PointingHandCursor);
    btnInfos->setFocusPolicy(Qt::NoFocus);
    btnInfos->setFixedSize(40, 40);
    btnInfos->setToolTip("À propos du jeu et du développeur");

    QHBoxLayout *topBarAccueil = new QHBoxLayout();
    btnResetScores = new QPushButton("Réinitialiser les scores", pageAccueil);
    btnResetScores->setObjectName("btnAction");
    btnResetScores->setCursor(Qt::PointingHandCursor);

    btnThemeToggle = new QPushButton(pageAccueil);
    btnThemeToggle->setObjectName("btnThemeToggle");
    btnThemeToggle->setCursor(Qt::PointingHandCursor);
    btnThemeToggle->setFocusPolicy(Qt::NoFocus);
    btnThemeToggle->setFixedSize(40, 40);
    btnThemeToggle->setToolTip("Changer de thème (Sombre / Clair)");

    btnSonToggle = new QPushButton(pageAccueil);
    btnSonToggle->setObjectName("btnSonToggle");
    btnSonToggle->setCursor(Qt::PointingHandCursor);
    btnSonToggle->setFocusPolicy(Qt::NoFocus);
    btnSonToggle->setFixedSize(40, 40);
    btnSonToggle->setToolTip("Activer / Couper le son");

    topBarAccueil->addWidget(btnInfos);
    topBarAccueil->addStretch();
    topBarAccueil->addWidget(btnSonToggle);
    topBarAccueil->addSpacing(10);
    topBarAccueil->addWidget(btnThemeToggle);
    topBarAccueil->addSpacing(10);
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
    connect(btnResetScores, &QPushButton::clicked, this, &MainWindow::afficherConfirmationReset);
    connect(btnThemeToggle, &QPushButton::clicked, this, [this]() {
        modeSombre = !modeSombre;
        QSettings settings("MemoryGame", "MemoryGame");
        settings.setValue("modeSombre", modeSombre);
        applyStylesheet();
    });
    connect(btnSonToggle, &QPushButton::clicked, this, &MainWindow::basculerSon);
    connect(btnInfos, &QPushButton::clicked, this, &MainWindow::afficherInfosJeu);

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
    btnConditionsLink->setObjectName("btnConditionsLink");
    btnConditionsLink->setCursor(Qt::PointingHandCursor);
    btnConditionsLink->setFlat(true);
    btnConditionsLink->setFocusPolicy(Qt::NoFocus);

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

    gridContainer->setObjectName("gridContainer");

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
    jouerSon("flip");
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
    jouerSon("flip");
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
        jouerSon("flip");
        animerCarte(boutonsCartes[index]);
        mettreAJourAffichage();

        if (jeu.deuxCartesRetournees()) {
            if (jeu.verifierPaire()) {
                mettreAJourAffichage();

                if (jeu.estPartieTerminee()) {
                    chronoTimer->stop();
                    verrouClic = true;

                    int dim = comboDimension->currentData().toInt();
                    int coupsFait = jeu.getCoups();
                    int tempsFait = secondes;

                    bool nouveauRecord = gestionnaireScores.enregistrerScore(dim, coupsFait, tempsFait);

                    Record r = gestionnaireScores.getRecord(dim);
                    int minR = r.secondes / 60;
                    int secR = r.secondes % 60;
                    QString recordStr = QString("%1 coups (%2:%3)")
                                            .arg(r.coups)
                                            .arg(minR, 2, 10, QChar('0'))
                                            .arg(secR, 2, 10, QChar('0'));

                    if (nouveauRecord) {
                        labelMeilleurScore->setText(QString("★ NOUVEAU RECORD : %1").arg(recordStr));
                    } else {
                        labelMeilleurScore->setText(QString("Record : %1").arg(recordStr));
                    }

                    jouerSon(nouveauRecord ? "record" : "win");

                    int dureeFlash = nouveauRecord ? 1100 : 750;
                    jouerEffetVictoire(nouveauRecord);

                    QTimer::singleShot(dureeFlash, this, [this]() {
                        verrouClic = false;
                    });
                } else {
                    jouerSon("match");
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
    if (btnThemeToggle) {
        btnThemeToggle->setText(modeSombre ? "☀" : "🌙");
    }

    if (modeSombre) {
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
            #btnThemeToggle {
                background-color: #1E293B;
                color: #FBBF24;
                border: 1px solid #334155;
                border-radius: 20px;
                font-size: 18px;
            }
            #btnThemeToggle:hover {
                background-color: #334155;
            }
            #btnConditionsLink {
                background: transparent;
                border: none;
                outline: none;
                color: #94A3B8;
                text-decoration: underline;
                font-size: 13px;
            }
            #btnConditionsLink:hover {
                color: #38BDF8;
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
            QPushButton[etat="victoire"] {
                background-color: #BBF7D0;
                border: 3px solid #4ADE80;
                border-radius: 14px;
                font-size: 32px;
                font-weight: bold;
                color: #052E16;
            }
            QPushButton[etat="record"] {
                background-color: #FEF08A;
                border: 3px solid #FBBF24;
                border-radius: 14px;
                font-size: 32px;
                font-weight: bold;
                color: #422006;
            }
            #btnSonToggle {
                background-color: #1E293B;
                color: #38BDF8;
                border: 1px solid #334155;
                border-radius: 20px;
                font-size: 18px;
            }
            #btnSonToggle:hover {
                background-color: #334155;
            }
            #confirmCard {
                background-color: #1E293B;
                border: 1px solid #334155;
                border-radius: 16px;
            }
            #btnAnnulerReset {
                background-color: #334155;
                color: #F8FAFC;
                border-radius: 8px;
                font-weight: bold;
                padding: 0 16px;
            }
            #btnAnnulerReset:hover {
                background-color: #475569;
            }
            #btnConfirmerReset {
                background-color: #EF4444;
                color: #FFFFFF;
                border-radius: 8px;
                font-weight: bold;
                padding: 0 16px;
            }
            #btnConfirmerReset:hover {
                background-color: #DC2626;
            }
            /* Bouton d'information rond avec contour */
            QPushButton#btnInfos {
                background-color: transparent;
                color: #E2E8F0;
                border: 2px solid #475569;
                border-radius: 20px;
                font-size: 17px;
                font-weight: bold;
                padding: 0px;
            }
            QPushButton#btnInfos:hover {
                background-color: #1E293B;
                border-color: #38BDF8;
                color: #38BDF8;
            }

            /* Fenêtre À propos */
            QDialog#infoDialog {
                background-color: #18202C;
                border: 1px solid #2D3A4E;
                border-radius: 12px;
            }
            QLabel#titreInfo {
                font-size: 18px;
                font-weight: bold;
                color: #FFFFFF;
                background: transparent;
            }
            QLabel#descInfo {
                font-size: 13px;
                color: #CBD5E1;
                background: transparent;
            }
            QLabel#devInfo {
                font-size: 13px;
                color: #E2E8F0;
                background: transparent;
            }
            QPushButton#btnFermerInfo {
                background-color: #0284C7;
                color: #FFFFFF;
                border: none;
                border-radius: 8px;
                padding: 8px 18px;
                font-weight: bold;
            }
            QPushButton#btnFermerInfo:hover {
                background-color: #0369A1;
            }
        )");
    } else {
        setStyleSheet(R"(
            QMainWindow, QWidget {
                background-color: #F1F5F9;
                color: #0F172A;
                font-family: 'Segoe UI', Arial, sans-serif;
            }
            #titreAccueil {
                font-size: 44px;
                font-weight: bold;
                color: #0284C7;
                letter-spacing: 3px;
            }
            #sousTitreAccueil {
                font-size: 18px;
                color: #475569;
            }
            #labelRecordAccueil {
                font-size: 16px;
                color: #0369A1;
                font-weight: bold;
            }
            #comboDim {
                background-color: #FFFFFF;
                border: 2px solid #CBD5E1;
                border-radius: 10px;
                padding: 12px 16px;
                font-size: 16px;
                color: #0F172A;
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
                background-color: #FFFFFF;
                color: #0F172A;
                border: 1px solid #CBD5E1;
                border-radius: 8px;
                padding: 8px 16px;
                font-size: 14px;
                font-weight: bold;
            }
            #btnAction:hover {
                background-color: #E2E8F0;
            }
            #btnThemeToggle {
                background-color: #FFFFFF;
                color: #0F172A;
                border: 1px solid #CBD5E1;
                border-radius: 20px;
                font-size: 18px;
            }
            #btnThemeToggle:hover {
                background-color: #E2E8F0;
            }
            #btnConditionsLink {
                background: transparent;
                border: none;
                outline: none;
                color: #475569;
                text-decoration: underline;
                font-size: 13px;
            }
            #btnConditionsLink:hover {
                color: #0284C7;
            }
            #statLabel {
                font-size: 16px;
                font-weight: bold;
                color: #0F172A;
                background-color: #FFFFFF;
                border: 1px solid #CBD5E1;
                padding: 8px 16px;
                border-radius: 8px;
            }
            QPushButton[etat="cachee"] {
                background-color: #E0F2FE;
                border: 2px solid #0284C7;
                border-radius: 14px;
                font-size: 32px;
                font-weight: bold;
                color: #0369A1;
            }
            QPushButton[etat="cachee"]:hover {
                background-color: #BAE6FD;
                border-color: #0284C7;
            }
            QPushButton[etat="visible"] {
                background-color: #FEF3C7;
                border: 3px solid #D97706;
                border-radius: 14px;
                font-size: 32px;
                font-weight: bold;
                color: #0F172A;
            }
            QPushButton[etat="trouvee"] {
                background-color: #DCFCE7;
                border: 3px solid #16A34A;
                border-radius: 14px;
                font-size: 32px;
                font-weight: bold;
                color: #14532D;
            }
            QPushButton[etat="victoire"] {
                background-color: #86EFAC;
                border: 3px solid #15803D;
                border-radius: 14px;
                font-size: 32px;
                font-weight: bold;
                color: #052E16;
            }
            QPushButton[etat="record"] {
                background-color: #FDE047;
                border: 3px solid #CA8A04;
                border-radius: 14px;
                font-size: 32px;
                font-weight: bold;
                color: #422006;
            }
            #btnSonToggle {
                background-color: #FFFFFF;
                color: #0284C7;
                border: 1px solid #CBD5E1;
                border-radius: 20px;
                font-size: 18px;
            }
            #btnSonToggle:hover {
                background-color: #E2E8F0;
            }
            #confirmCard {
                background-color: #FFFFFF;
                border: 1px solid #CBD5E1;
                border-radius: 16px;
            }
            #btnAnnulerReset {
                background-color: #E2E8F0;
                color: #0F172A;
                border-radius: 8px;
                font-weight: bold;
                padding: 0 16px;
            }
            #btnAnnulerReset:hover {
                background-color: #CBD5E1;
            }
            #btnConfirmerReset {
                background-color: #EF4444;
                color: #FFFFFF;
                border-radius: 8px;
                font-weight: bold;
                padding: 0 16px;
            }
            #btnConfirmerReset:hover {
                background-color: #DC2626;
            }
            /* Bouton d'information rond avec contour */
            QPushButton#btnInfos {
                background-color: transparent;
                color: #1E293B;
                border: 2px solid #94A3B8;
                border-radius: 20px;
                font-size: 17px;
                font-weight: bold;
                padding: 0px;
            }
            QPushButton#btnInfos:hover {
                background-color: #E2E8F0;
                border-color: #0284C7;
                color: #0284C7;
            }

            /* Fenêtre À propos */
            QDialog#infoDialog {
                background-color: #FFFFFF;
                border: 1px solid #CBD5E1;
                border-radius: 12px;
            }
            QLabel#titreInfo {
                font-size: 18px;
                font-weight: bold;
                color: #0F172A;
                background: transparent;
            }
            QLabel#descInfo {
                font-size: 13px;
                color: #475569;
                background: transparent;
            }
            QLabel#devInfo {
                font-size: 13px;
                color: #1E293B;
                background: transparent;
            }
            QPushButton#btnFermerInfo {
                background-color: #0284C7;
                color: #FFFFFF;
                border: none;
                border-radius: 8px;
                padding: 8px 18px;
                font-weight: bold;
            }
            QPushButton#btnFermerInfo:hover {
                background-color: #0369A1;
            }
        )");
    }
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
    titre->setStyleSheet(modeSombre
                             ? "font-size: 20px; font-weight: bold; color: #38BDF8;"
                             : "font-size: 20px; font-weight: bold; color: #0284C7;");

    QTextBrowser *texte = new QTextBrowser(&dialog);
    texte->setOpenExternalLinks(false);
    texte->setHtml(
        "<p>Bienvenue dans <b>Memory Game</b> — Édition Desktop.</p>"
        "<p><b>1. Utilisation du jeu :</b> En acceptant ces conditions d'utilisation, vous vous engagez "
        "à utiliser l'application dans le cadre d'un usage normal et personnel de divertissement.</p>"
        "<p><b>2. Sauvegarde locale des scores et préférences :</b> Vos meilleurs scores "
        "(nombre de coups et temps réalisé selon la taille de la grille) ainsi que vos préférences "
        "d'interface (thème sombre ou clair, état du son et acceptation des conditions) sont enregistrés "
        "localement sur votre appareil (notamment via <code>scores.txt</code> et les paramètres locaux). "
        "Vous pouvez réinitialiser vos records à tout moment depuis l'écran d'accueil.</p>"
        "<p><b>3. Confidentialité :</b> Aucune donnée personnelle ni statistique de jeu n'est "
        "collectée, transmise ou partagée avec un serveur externe.</p>"
        "<p><b>4. Audio et affichage :</b> L'application propose des effets sonores et un basculement "
        "entre mode sombre et mode clair ; vous pouvez couper le son ou changer de thème à tout moment.</p>"
        );
    texte->setStyleSheet(modeSombre
                             ? "background-color: #1E293B; color: #F1F5F9; border: 1px solid #334155; "
                               "border-radius: 8px; padding: 10px; font-size: 14px;"
                             : "background-color: #FFFFFF; color: #0F172A; border: 1px solid #CBD5E1; "
                               "border-radius: 8px; padding: 10px; font-size: 14px;");

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
        btnRefuser->setStyleSheet(modeSombre
                                      ? "background-color: #1E293B; color: #F1F5F9; border: 1px solid #475569; "
                                        "border-radius: 8px; padding: 10px 16px; font-weight: bold;"
                                      : "background-color: #E2E8F0; color: #0F172A; border: 1px solid #CBD5E1; "
                                        "border-radius: 8px; padding: 10px 16px; font-weight: bold;");

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
void MainWindow::jouerEffetVictoire(bool nouveauRecord)
{
    QString etatFlash = nouveauRecord ? "record" : "victoire";

    // Applique l'état lumineux sur toutes les cartes d'un coup
    for (QPushButton *btn : boutonsCartes) {
        if (!btn) continue;
        btn->setProperty("etat", etatFlash);
        btn->style()->unpolish(btn);
        btn->style()->polish(btn);
        btn->update();
    }

    int dureeFlash = nouveauRecord ? 1100 : 750;

    // Deuxième impulsion rapide uniquement pour un nouveau record
    if (nouveauRecord) {
        QTimer::singleShot(350, this, [this]() {
            for (QPushButton *btn : boutonsCartes) {
                if (!btn) continue;
                btn->setProperty("etat", "trouvee");
                btn->style()->unpolish(btn);
                btn->style()->polish(btn);
            }
        });

        QTimer::singleShot(550, this, [this]() {
            for (QPushButton *btn : boutonsCartes) {
                if (!btn) continue;
                btn->setProperty("etat", "record");
                btn->style()->unpolish(btn);
                btn->style()->polish(btn);
            }
        });
    }

    // Retour propre à l'état "trouvee"
    QTimer::singleShot(dureeFlash, this, [this]() {
        for (QPushButton *btn : boutonsCartes) {
            if (!btn) continue;
            btn->setProperty("etat", "trouvee");
            btn->style()->unpolish(btn);
            btn->style()->polish(btn);
            btn->update();
        }
        if (gridContainer) {
            gridContainer->update();
        }
    });
}
void MainWindow::initialiserSons()
{
    QSettings settings("MemoryGame", "MemoryGame");
    sonActif = settings.value("audio/sonActif", true).toBool();
    mettreAJourBoutonSon();

    struct SonConfig {
        QString cle;
        QString ressource;
        float volume;
    };

    const QList<SonConfig> configs = {
        {"flip",   "qrc:/sounds/flip.wav",       0.45f},
        {"match",  "qrc:/sounds/match.wav",      0.65f},
        {"win",    "qrc:/sounds/win-record.wav", 0.80f},
        {"record", "qrc:/sounds/win-record.wav", 0.90f}
    };

    for (const auto &cfg : configs) {
        QMediaPlayer *player = new QMediaPlayer(this);
        QAudioOutput *output = new QAudioOutput(this);
        output->setVolume(cfg.volume);
        player->setAudioOutput(output);
        player->setSource(QUrl(cfg.ressource));

        effetsAudio.insert(cfg.cle, {player, output});
    }
}

void MainWindow::jouerSon(const QString &cle)
{
    if (!sonActif) return;

    if (!effetsAudio.contains(cle)) return;

    QMediaPlayer *player = effetsAudio[cle].player;
    if (!player) return;

    player->stop();
    player->setPosition(0);
    player->play();
}
void MainWindow::mettreAJourBoutonSon()
{
    if (!btnSonToggle) return;
    btnSonToggle->setText(sonActif ? "🔊" : "🔇");
}

void MainWindow::basculerSon()
{
    sonActif = !sonActif;

    QSettings settings("MemoryGame", "MemoryGame");
    settings.setValue("audio/sonActif", sonActif);
    settings.sync();

    mettreAJourBoutonSon();

    if (sonActif) {
        jouerSon("flip");
    }
}
void MainWindow::setupPageConfirmReset()
{
    pageConfirmReset = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(pageConfirmReset);
    mainLayout->setAlignment(Qt::AlignCenter);

    QFrame *card = new QFrame(pageConfirmReset);
    card->setObjectName("confirmCard");
    card->setFixedWidth(420);

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setSpacing(18);
    cardLayout->setContentsMargins(28, 28, 28, 28);

    QLabel *iconLabel = new QLabel("⚠️", card);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("font-size: 38px;");

    QLabel *titreLabel = new QLabel("Réinitialiser les scores ?", card);
    titreLabel->setObjectName("titreConfirm");
    titreLabel->setAlignment(Qt::AlignCenter);
    titreLabel->setStyleSheet("font-size: 20px; font-weight: bold;");

    QLabel *descLabel = new QLabel(
        "Cette action supprimera définitivement tous vos meilleurs records "
        "et statistiques enregistrés. Voulez-vous continuer ?",
        card
        );
    descLabel->setWordWrap(true);
    descLabel->setAlignment(Qt::AlignCenter);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(12);

    btnAnnulerReset = new QPushButton("Annuler", card);
    btnAnnulerReset->setObjectName("btnAnnulerReset");
    btnAnnulerReset->setCursor(Qt::PointingHandCursor);
    btnAnnulerReset->setFixedHeight(40);

    btnConfirmerReset = new QPushButton("Oui, réinitialiser", card);
    btnConfirmerReset->setObjectName("btnConfirmerReset");
    btnConfirmerReset->setCursor(Qt::PointingHandCursor);
    btnConfirmerReset->setFixedHeight(40);

    btnLayout->addWidget(btnAnnulerReset);
    btnLayout->addWidget(btnConfirmerReset);

    cardLayout->addWidget(iconLabel);
    cardLayout->addWidget(titreLabel);
    cardLayout->addWidget(descLabel);
    cardLayout->addSpacing(8);
    cardLayout->addLayout(btnLayout);

    mainLayout->addWidget(card);

    connect(btnAnnulerReset, &QPushButton::clicked, this, [this]() {
        jouerSon("flip");
        stackedWidget->setCurrentIndex(0);
    });

    connect(btnConfirmerReset, &QPushButton::clicked, this, &MainWindow::confirmerResetScores);
}
void MainWindow::afficherConfirmationReset()
{
    jouerSon("flip");

    QDialog dlg(this);
    dlg.setWindowTitle("Réinitialiser les scores");
    dlg.setModal(true);
    dlg.setFixedWidth(370);

    dlg.setStyleSheet(R"(
        QDialog {
            background-color: #18202C;
            border: 1px solid #2D3A4E;
            border-radius: 12px;
        }
        QLabel {
            color: #F1F5F9;
            background: transparent;
        }
        QPushButton#btnAnnuler {
            background-color: #283548;
            color: #E2E8F0;
            border: 1px solid #3B4D66;
            border-radius: 8px;
            padding: 8px 14px;
            font-weight: bold;
        }
        QPushButton#btnAnnuler:hover {
            background-color: #33445C;
        }
        QPushButton#btnConfirmer {
            background-color: #DC2626;
            color: #FFFFFF;
            border: none;
            border-radius: 8px;
            padding: 8px 14px;
            font-weight: bold;
        }
        QPushButton#btnConfirmer:hover {
            background-color: #B91C1C;
        }
    )");

    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    layout->setContentsMargins(20, 18, 20, 18);
    layout->setSpacing(12);

    QLabel *titre = new QLabel("Réinitialiser les scores ?", &dlg);
    titre->setStyleSheet("font-size: 16px; font-weight: bold;");

    QLabel *message = new QLabel(
        "Tous les meilleurs scores enregistrés seront effacés.",
        &dlg
        );
    message->setWordWrap(true);
    message->setStyleSheet("font-size: 13px; color: #CBD5E1;");

    QHBoxLayout *boutons = new QHBoxLayout();
    boutons->setSpacing(10);
    boutons->addStretch();

    QPushButton *btnAnnuler = new QPushButton("Annuler", &dlg);
    btnAnnuler->setObjectName("btnAnnuler");
    btnAnnuler->setCursor(Qt::PointingHandCursor);

    QPushButton *btnConfirmer = new QPushButton("Oui, réinitialiser", &dlg);
    btnConfirmer->setObjectName("btnConfirmer");
    btnConfirmer->setCursor(Qt::PointingHandCursor);

    boutons->addWidget(btnAnnuler);
    boutons->addWidget(btnConfirmer);

    layout->addWidget(titre);
    layout->addWidget(message);
    layout->addSpacing(4);
    layout->addLayout(boutons);

    connect(btnAnnuler, &QPushButton::clicked, &dlg, &QDialog::reject);
    connect(btnConfirmer, &QPushButton::clicked, &dlg, &QDialog::accept);

    if (dlg.exec() == QDialog::Accepted) {
        confirmerResetScores();
    }
}

void MainWindow::confirmerResetScores()
{
    jouerSon("flip");

    gestionnaireScores.reinitialiser();
    mettreAJourLabelRecordAccueil();
}
void MainWindow::afficherInfosJeu()
{
    jouerSon("flip");

    QDialog dlg(this);
    dlg.setWindowTitle("À propos — Memory Game");
    dlg.setModal(true);
    dlg.setFixedWidth(480);
    dlg.setObjectName("infoDialog");

    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    layout->setContentsMargins(26, 24, 26, 24);
    layout->setSpacing(14);

    QLabel *titre = new QLabel("Memory Game — Édition Desktop", &dlg);
    titre->setObjectName("titreInfo");

    QLabel *descJeu = new QLabel(
        "Conçu et développé en C++ avec le framework Qt, ce jeu de mémoire met à l'épreuve "
        "votre concentration et votre rapidité d'observation à travers plusieurs niveaux de difficulté "
        "(de 4 à 16 cartes).\n\n"
        "L'application intègre un suivi précis des performances (chronomètre et compteur de coups), "
        "une sauvegarde persistante des meilleurs records par dimension, des animations fluides, "
        "un retour sonore interactif ainsi qu'une interface adaptative en mode sombre et clair.",
        &dlg
        );
    descJeu->setObjectName("descInfo");
    descJeu->setWordWrap(true);

    QString couleurLien = modeSombre ? "#38BDF8" : "#0284C7";
    QLabel *devInfo = new QLabel(
        QString(
            "<b>Développeuse :</b> Amani Bouchnak<br>"
            "<b>Formation :</b> Cycle Préparatoire Intégré en Informatique (ISIMM)<br>"
            "<b>Technologies :</b> C++, Qt Widgets, QSettings, QMediaPlayer<br><br>"
            "<b>LinkedIn :</b> <a href=\"https://www.linkedin.com/in/amani-bouchnak-90aaa5374/\" "
            "style=\"color: %1; text-decoration: none;\">linkedin.com/in/amani-bouchnak</a><br>"
            "<b>Contact :</b> <a href=\"mailto:amanibouchnak69@gmail.com\" "
            "style=\"color: %1; text-decoration: none;\">amanibouchnak69@gmail.com</a>"
            ).arg(couleurLien),
        &dlg
        );
    devInfo->setObjectName("devInfo");
    devInfo->setTextFormat(Qt::RichText);
    devInfo->setTextInteractionFlags(Qt::TextBrowserInteraction);
    devInfo->setOpenExternalLinks(true);
    devInfo->setWordWrap(true);

    QHBoxLayout *basLayout = new QHBoxLayout();
    basLayout->addStretch();

    QPushButton *btnFermer = new QPushButton("Fermer", &dlg);
    btnFermer->setObjectName("btnFermerInfo");
    btnFermer->setCursor(Qt::PointingHandCursor);
    basLayout->addWidget(btnFermer);

    layout->addWidget(titre);
    layout->addWidget(descJeu);
    layout->addSpacing(4);
    layout->addWidget(devInfo);
    layout->addSpacing(8);
    layout->addLayout(basLayout);

    connect(btnFermer, &QPushButton::clicked, &dlg, &QDialog::accept);

    dlg.exec();
}