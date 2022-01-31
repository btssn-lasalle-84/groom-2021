#include "IHMGroom.h"
#include "ui_IHMGroom.h"

/**
 * @file IHMGroom.cpp
 * @brief Définition de la classe IHMGroom
 * @author ymota <motayuri13@gmail.com>
 * @version 1.1
 * $LastChangedRevision: 148 $
 * $LastChangedDate: 2021-06-15 04:00:52 +0200 (mar. 15 juin 2021) $
 */

/**
 * @brief Constructeur de la classe IHMGroom
 *
 * @fn IHMGroom::IHMGroom
 * @param parent L'adresse de l'objet parent, si 0 IHMGroom sera la fenêtre principale
 */
IHMGroom::IHMGroom(QWidget *parent) : QMainWindow(parent), uiIHMGroom(new Ui::IHMGroom), etatGroom(EtatGroom::Libre), etatSonnette(EtatSonnette::Active), etatPresence(EtatPresence::Arret), sonnetteOff(":sonnette-off.png"), sonnetteOn(":sonnette-on.png")
{
    qDebug() << Q_FUNC_INFO;
    uiIHMGroom->setupUi(this);
    communicationGroom = new CommunicationGroom(this);

    chargerParametres();

    initialiserIconeSysteme();

    initialiserIHM();

    rechercherGroom();

    #ifdef TEST_NOTIFICATION
    connect(uiIHMGroom->boutonPoussoirSonnette, SIGNAL(clicked(bool)), this, SLOT(testerNotification()));
    #endif
}

/**
 * @brief Destructeur de la classe IHMGroom
 *
 * @fn IHMGroom::~IHMGroom
 * @details Sauvegarde les paramètres du Groom dans le fihcier .ini
 */
IHMGroom::~IHMGroom()
{
    sauvegarderParametres();

    delete uiIHMGroom;
    qDebug() << Q_FUNC_INFO;
}

/**
 * @brief Méthode redéfinie qui est appelée automatiquement lors d'une demande de fermeture
 *
 * @fn IHMGroom::closeEvent(QCloseEvent *event)
 * @param event L'événement de fermeture reçu
 */
void IHMGroom::closeEvent(QCloseEvent *event)
{
    if (iconeSysteme->isVisible())
    {
        if(etatInitialIconeSysteme)
        {
            QMessageBox::information(this, "IHMGroom", "Le programme continue à s'éxécuter. Utiliser le menu Quitter pour mettre fin à l'application.");
            etatInitialIconeSysteme = false;
        }
        hide();
        event->ignore();
    }
}

/**
 * @brief Méthode qui permet à l'application de s'installer dans la barre système
 *
 * @fn IHMGroom::initialiserIconeSysteme()
 */
void IHMGroom::initialiserIconeSysteme()
{
    // Crée les actions
    actionMinimiser = new QAction(QString::fromUtf8("Minimiser"), this);
    actionMaximiser = new QAction(QString::fromUtf8("Maximiser"), this);
    actionRestaurer = new QAction(QString::fromUtf8("Restaurer"), this);
    actionQuitter = new QAction(QString::fromUtf8("&Quitter"), this);
    barreEtat = statusBar();
    calendrier = new Calendrier(this);
    uiIHMGroom->widgetCalendrier->setMinimumDate(QDate(1900, 1, 1));
    uiIHMGroom->widgetCalendrier->setMaximumDate(QDate(3000, 1, 1));
    uiIHMGroom->widgetCalendrier->setGridVisible(true);
    QMenu *fileMenu = new QMenu(tr("&Fichier"), this);
    menuBar()->addMenu(fileMenu);
    fileMenu->addAction(tr("&Ouvrir un fichier de calendrier"), this, SLOT(ouvrirFichier()), QKeySequence::New);
    fileMenu->addAction(tr("&Ouvrir un calendrier avec une URL"), this, SLOT(ouvrirURL()), QKeySequence::Open);
    fileMenu->addAction(tr("&Effacer"), this, SLOT(effacer()),  QKeySequence(tr("Suppr", "Suppression")));
    fileMenu->addAction(tr("&Quitter"), qApp, SLOT(quit()), QKeySequence::Quit);
    menuBar()->setStyleSheet("background-color: rgb(176, 193, 173);");

    // Connecte les actions
    connect(actionMinimiser, SIGNAL(triggered(bool)), this, SLOT(hide()));
    connect(actionMaximiser, SIGNAL(triggered(bool)), this, SLOT(showMaximized()));
    connect(actionRestaurer, SIGNAL(triggered(bool)), this, SLOT(showNormal()));
    connect(actionQuitter, SIGNAL(triggered(bool)), qApp, SLOT(quit()));

    // Crée le menu
    menuIconeSysteme = new QMenu(this);
    menuIconeSysteme->addAction(actionMinimiser);
    menuIconeSysteme->addAction(actionMaximiser);
    menuIconeSysteme->addAction(actionRestaurer);
    menuIconeSysteme->addSeparator();
    menuIconeSysteme->addAction(actionQuitter);

    // Crée l'icône pour la barre de tâche
    iconeSysteme = new QSystemTrayIcon(this);
    iconeSysteme->setContextMenu(menuIconeSysteme);
    iconeSysteme->setToolTip("Groom");
    QIcon icone(":/groom.png");
    iconeSysteme->setIcon(icone);
    setWindowIcon(icone);

    connect(iconeSysteme, SIGNAL(messageClicked()), this, SLOT(acquitterNotification()));
    //connect(iconeSysteme, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(aActiveIconeSysteme(QSystemTrayIcon::ActivationReason)));

    iconeSysteme->show();
    etatInitialIconeSysteme = true;
}

/**
 * @brief Méthode qui permet d'afficher une notification système
 *
 * @fn IHMGroom::afficherNotification(QString titre, QString message, int duree)
 * @param titre Le titre de la notification
 * @param message Le message de la notification
 * @param duree La durée en millisecondes de la notification (par défaut 5s)
 */
void IHMGroom::afficherNotification(QString titre, QString message, QSystemTrayIcon::MessageIcon icone, int duree)
{
    //QIcon icone(":/groom.png");
    iconeSysteme->showMessage(titre, message, icone, duree); // duree en ms
    /*
        QSystemTrayIcon::NoIcon
        QSystemTrayIcon::Information
        QSystemTrayIcon::Warning
        QSystemTrayIcon::Critical
    */
}

/**
 * @brief Méthode qui permet d'acquitter une notification
 *
 * @fn IhmGroom::acquitterNotification()
 */
void IHMGroom::acquitterNotification()
{
    qDebug() << Q_FUNC_INFO;
}

#ifdef TEST_NOTIFICATION
/**
 * @brief Teste le système de notification
 *
 * @fn IHMGroom::testerNotification()
 */
void IHMGroom::testerNotification()
{
    afficherNotification("Groom", "Quelqu'un a sonné !");
}
#endif

/**
 * @brief Initialise l'IHM GROOM
 *
 * @fn IHMGroom::initialiserIHM()
 */
void IHMGroom::initialiserIHM()
{
    uiIHMGroom->listeBluetooth->clear();
    uiIHMGroom->boutonRechercherBluetooth->setEnabled(true);
    uiIHMGroom->labelEtatConnexion->setText("État : déconnecté");
    uiIHMGroom->boutonConnecterBluetooth->setEnabled(false);
    uiIHMGroom->boutonDeconnecterBluetooth->setEnabled(false);
    connect(uiIHMGroom->boutonRechercherBluetooth, SIGNAL(clicked(bool)), this, SLOT(rechercherGroom()));
    connect(uiIHMGroom->boutonConnecterBluetooth, SIGNAL(clicked(bool)), this, SLOT(connecterGroom()));
    connect(uiIHMGroom->boutonDeconnecterBluetooth, SIGNAL(clicked(bool)), this, SLOT(deconnecterGroom()));

    connect(uiIHMGroom->boutonEnvoyerOccupant, SIGNAL(clicked(bool)), this, SLOT(envoyerAffichageOccupant()));

    uiIHMGroom->lineEditNom->setPlaceholderText("Nom");
    uiIHMGroom->lineEditPrenom->setPlaceholderText("Prénom");
    uiIHMGroom->lineEditFonction->setPlaceholderText("Fonction");
    uiIHMGroom->zoneTexteMessage->setPlaceholderText("Message à afficher");
    uiIHMGroom->boutonEnvoyerOccupant->setEnabled(false);
    uiIHMGroom->lineEditNom->setEnabled(false);
    uiIHMGroom->lineEditPrenom->setEnabled(false);
    uiIHMGroom->lineEditFonction->setEnabled(false);
    uiIHMGroom->listeOccupant->setEnabled(false);
    uiIHMGroom->boutonViderFormulaire->setEnabled(false);
    uiIHMGroom->zoneTexteMessage->setEnabled(false);
    uiIHMGroom->boutonEnvoyerMessage->setEnabled(false);
    uiIHMGroom->boutonRadioLibre->setEnabled(false);
    uiIHMGroom->boutonRadioOccupe->setEnabled(false);
    uiIHMGroom->boutonRadioAbsent->setEnabled(false);
    uiIHMGroom->boutonRadioEntrer->setEnabled(false);
    uiIHMGroom->boutonPoussoirSonnette->setEnabled(false);
    uiIHMGroom->boutonActiverDesactiverPresence->setEnabled(false);

    uiIHMGroom->boutonAjouterOccupant->setEnabled(false);
    uiIHMGroom->boutonEditerOccupant->setEnabled(false);
    uiIHMGroom->boutonSupprimerOccupant->setEnabled(false);
    connect(uiIHMGroom->listeOccupant, SIGNAL(activated(int)), this, SLOT(selectionnerOccupant(int)));
    connect(uiIHMGroom->boutonAjouterOccupant, SIGNAL(clicked(bool)), this, SLOT(ajouterOccupant()));
    connect(uiIHMGroom->boutonEditerOccupant, SIGNAL(clicked(bool)), this, SLOT(modifierOccupant()));
    connect(uiIHMGroom->boutonSupprimerOccupant, SIGNAL(clicked(bool)), this, SLOT(supprimerOccupant()));
    connect(uiIHMGroom->boutonRadioLibre, SIGNAL(clicked(bool)), this, SLOT(envoyerEtatLibre()));
    connect(uiIHMGroom->boutonRadioAbsent, SIGNAL(clicked(bool)), this, SLOT(envoyerEtatAbsent()));
    connect(uiIHMGroom->boutonRadioOccupe, SIGNAL(clicked(bool)), this, SLOT(envoyerEtatOccupe()));
    uiIHMGroom->boutonRadioEntrer->setEnabled(false);
    connect(uiIHMGroom->boutonRadioEntrer, SIGNAL(clicked(bool)), this, SLOT(envoyerEtatEntrer()));
    uiIHMGroom->boutonPoussoirSonnette->setEnabled(false);
    connect(uiIHMGroom->boutonPoussoirSonnette, SIGNAL(clicked(bool)), this, SLOT(envoyerEtatSonnette()));
    uiIHMGroom->boutonActiverDesactiverPresence->setEnabled(false);
    connect(uiIHMGroom->boutonActiverDesactiverPresence, SIGNAL(clicked(bool)), this, SLOT(envoyerEtatPresence()));
    connect(uiIHMGroom->boutonEnvoyerMessage, SIGNAL(clicked(bool)), this, SLOT(envoyerMessageOccupant()));
    connect(uiIHMGroom->widgetCalendrier, SIGNAL(clicked(QDate)), this, SLOT(selectionnerDate(QDate)));
    connect(uiIHMGroom->boutonViderFormulaire, SIGNAL(clicked(bool)), this, SLOT(viderFormulaire()));

    connect(communicationGroom, SIGNAL(groomDetecte(QString,QString)), this, SLOT(detecterGroom(QString,QString)));
    connect(communicationGroom, SIGNAL(connexionGroom(QString,QString)), this, SLOT(afficherConnexionGroom(QString,QString)));
    connect(communicationGroom, SIGNAL(deconnexionGroom(QString,QString)), this, SLOT(afficherDeconnexionGroom(QString,QString)));
    connect(communicationGroom, SIGNAL(rechercheTerminee()), this, SLOT(arreterRechercheGroom()));
    connect(communicationGroom, SIGNAL(actualiserEtats(int,int,int,int,int)), this, SLOT(actualiserIndicateurs(int,int,int,int,int)));
}

/**
 * @brief Méthode qui permet de sélectionner une date
 *
 * @param dateSelectionnee
 */

void IHMGroom::recupererRequeteCalendrier()
{
    QUrl
    hostURL("https://calendar.google.com/calendar/ical/robertutilisateur84%40gmail.com/private-ab9f00fcf91e8c05d89d6e5c8a1234f7/basic.ics");
    requete.setUrl(hostURL);
    requete.setRawHeader("User-Agent", "CalendarClient_ICS");
    requete.setRawHeader("Depth", "0");
    requete.setRawHeader("Prefer", "return-minimal");
    requete.setRawHeader("Content-Type", "text/xml; charset=utf-8");
    requete.setRawHeader("Content-Length", "0");

    QSslConfiguration configuration = requete.sslConfiguration();
    configuration.setPeerVerifyMode(QSslSocket::VerifyNone);
    requete.setSslConfiguration(configuration);

    networkReply = networkManager.get(requete);

    if (networkReply != nullptr)
    {
      connect(networkReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleHTTPError()));
      connect(networkReply, SIGNAL(finished()), this, SLOT(traiterRequete()));
    }
}

void IHMGroom::traiterRequete()
{
    if (networkReply != nullptr)
    {
        QBuffer buffer(this);
        buffer.setData(networkReply->readAll());

        buffer.open(QIODevice::ReadOnly);
        bool trouve = false;
        do
        {
            QString ligne = buffer.readLine();
            if (!ligne.isNull())
            {
                if (ligne.startsWith("X-WR-CALNAME"))
                {
                    QString nomCalendrier = ligne.trimmed();
                    nomCalendrier.remove(";LANGUAGE=fr", Qt::CaseInsensitive);
                    nomCalendrier.remove("X-WR-CALNAME:", Qt::CaseInsensitive);
                    trouve = true;
                }
            }
        }
        while (!buffer.atEnd() && !trouve);
        buffer.seek(0);
        buffer.close();

        //contenuCalendrier = new QTextStream(buffer.data());

        // ...
    }
}

/**
 * @brief Méthode qui permet de sélectionner un occupant
 *
 * @fn IHMGroom::selectionnerOccupant()
 * @param index l'index dans la liste déroulante listeOccupant
 */
void IHMGroom::selectionnerOccupant(int index)
{
    if(occupants.size() < 1)
        return;
    indexOccupant = index + 1;
    qDebug() << Q_FUNC_INFO << "Occupant courant" << indexOccupant;
    uiIHMGroom->lineEditNom->setText(occupants[index].nom);
    uiIHMGroom->lineEditPrenom->setText(occupants[index].prenom);
    uiIHMGroom->lineEditFonction->setText(occupants[index].fonction);
    uiIHMGroom->boutonAjouterOccupant->setEnabled(true);
    uiIHMGroom->boutonEditerOccupant->setEnabled(true);
    uiIHMGroom->boutonSupprimerOccupant->setEnabled(true);
}

/**
 * @brief
 *
 * @fn IHMGroom::ajouterOccupant()
 */
void IHMGroom::ajouterOccupant()
{
    if(!(uiIHMGroom->lineEditNom->text() == "" || uiIHMGroom->lineEditPrenom->text() == "" || uiIHMGroom->lineEditFonction->text() == ""))
    {
        if((!uiIHMGroom->lineEditNom->text().isEmpty() || !uiIHMGroom->lineEditPrenom->text().isEmpty() || !uiIHMGroom->lineEditFonction->text().isEmpty()) && ((uiIHMGroom->lineEditNom->text() != occupants[uiIHMGroom->listeOccupant->currentIndex()].nom) || (uiIHMGroom->lineEditPrenom->text() != occupants[uiIHMGroom->listeOccupant->currentIndex()].prenom) || (uiIHMGroom->lineEditFonction->text() != occupants[uiIHMGroom->listeOccupant->currentIndex()].fonction)))
        {
            qDebug() << Q_FUNC_INFO;
            QString contenuNom = uiIHMGroom->lineEditNom->text();
            QString contenuPrenom = uiIHMGroom->lineEditPrenom->text();
            QString contenuFonction = uiIHMGroom->lineEditFonction->text();

            Occupant nouveau(contenuNom, contenuPrenom, contenuFonction);
            occupants.push_back(nouveau);
            uiIHMGroom->listeOccupant->setCurrentIndex(uiIHMGroom->listeOccupant->findText(uiIHMGroom->listeOccupant->currentText()));

            rechargerListeOccupants();
        }

        if(uiIHMGroom->lineEditNom->text() == uiIHMGroom->listeOccupant->findText(uiIHMGroom->listeOccupant->currentText()))
        {
            uiIHMGroom->lineEditNom->clear();
            uiIHMGroom->lineEditPrenom->clear();
            uiIHMGroom->lineEditFonction->clear();
        }
    }
    else
    {
        QSystemTrayIcon::MessageIcon icone = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information);
        afficherNotification("Groom", "Complétez tous les champs du formulaire !", icone);
    }
}

/**
 * @brief Méthode qui permet d'ajouter un occupant
 *
 * @fn IHMGroom::modifierOccupant()
 */
void IHMGroom::modifierOccupant()
{
    if(!(uiIHMGroom->lineEditNom->text() == "" || uiIHMGroom->lineEditPrenom->text() == "" || uiIHMGroom->lineEditFonction->text() == ""))
    {
        qDebug() << Q_FUNC_INFO << uiIHMGroom->listeOccupant->currentIndex() << indexOccupant;

        if((!uiIHMGroom->lineEditNom->text().isEmpty() || !uiIHMGroom->lineEditPrenom->text().isEmpty() || !uiIHMGroom->lineEditFonction->text().isEmpty()) && ((uiIHMGroom->lineEditNom->text() != occupants[uiIHMGroom->listeOccupant->currentIndex()].nom) || (uiIHMGroom->lineEditPrenom->text() != occupants[uiIHMGroom->listeOccupant->currentIndex()].prenom) || (uiIHMGroom->lineEditFonction->text() != occupants[uiIHMGroom->listeOccupant->currentIndex()].fonction)))
        {
            QString contenuNom = uiIHMGroom->lineEditNom->text();
            QString contenuPrenom = uiIHMGroom->lineEditPrenom->text();
            QString contenuFonction = uiIHMGroom->lineEditFonction->text();

            // modifier l'objet Occupant qui EST dans le QVector
            occupants[uiIHMGroom->listeOccupant->currentIndex()].nom = contenuNom;
            occupants[uiIHMGroom->listeOccupant->currentIndex()].prenom = contenuPrenom;
            occupants[uiIHMGroom->listeOccupant->currentIndex()].fonction = contenuFonction;

            uiIHMGroom->listeOccupant->setItemText(uiIHMGroom->listeOccupant->currentIndex(), occupants[uiIHMGroom->listeOccupant->currentIndex()].nom + " " + occupants[uiIHMGroom->listeOccupant->currentIndex()].prenom + " - " + occupants[uiIHMGroom->listeOccupant->currentIndex()].fonction);
        }
    }
    else
    {
        QSystemTrayIcon::MessageIcon icone = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information);
        afficherNotification("Groom", "Complétez tous les champs du formulaire !", icone);
    }
}

/**
 * @brief Méthode qui permet de supprimer un occupant
 *
 * @fn IHMGroom::supprimerOccupant()
 */
void IHMGroom::supprimerOccupant()
{
    qDebug() << Q_FUNC_INFO << uiIHMGroom->listeOccupant->currentIndex() << indexOccupant;

    occupants.removeAt(uiIHMGroom->listeOccupant->currentIndex());
    uiIHMGroom->listeOccupant->view()->update();

    rechargerListeOccupants();

    uiIHMGroom->lineEditNom->clear();
    uiIHMGroom->lineEditPrenom->clear();
    uiIHMGroom->lineEditFonction->clear();
}

/**
 * @brief Méthode déclenchée lorsque le module Bluetooth GROOM est détectée
 *
 * @fn IHMGroom::detecterGroom()
 * @param nomPeripherique Le nom du module Bluetooth GROOM
 * @param adressePeripherique L'adresse MAC du module Bluetooth GROOM
 */
void IHMGroom::detecterGroom(QString nomPeripherique, QString adressePeripherique)
{
    QString module = nomPeripherique + " [" + adressePeripherique + "]";
    qDebug() << Q_FUNC_INFO << module;

    // déjà présent dans la liste ?
    if(uiIHMGroom->listeBluetooth->findText(module) == -1)
    {
        uiIHMGroom->listeBluetooth->addItem(module);
        uiIHMGroom->boutonConnecterBluetooth->setEnabled(true);
    }
}

/**
 * @brief Méthode qui permet de se connecter au périphérique voulu
 *
 * @fn IHMGroom::connecterGroom()
 */
void IHMGroom::connecterGroom()
{
    qDebug() << Q_FUNC_INFO;
    communicationGroom->connecter();    
}

/**
 * @brief Méthode qui permet de se déconnecter du périphérique voulu
 *
 * @fn IHMGroom::deconnecterGroom()
 */
void IHMGroom::deconnecterGroom()
{
    qDebug() << Q_FUNC_INFO;
    envoyerEtatAbsent();
    int Groom = 1;
    int Sonnette = 0;
    int Presence = 0;
    int ModeSonnette = 0;
    int ModePresence = 0;
    actualiserIndicateurs(Groom, Sonnette, Presence, ModeSonnette, ModePresence);
    communicationGroom->deconnecter();
}

/**
 * @brief Méthode qui permet d'afficher l'état "connecté"
 *
 * @param nomPeripherique
 * @param adressePeripherique
 */
void IHMGroom::afficherConnexionGroom(QString nomPeripherique, QString adressePeripherique)
{
    qDebug() << Q_FUNC_INFO << nomPeripherique << adressePeripherique;
    uiIHMGroom->listeBluetooth->setEnabled(false);
    uiIHMGroom->boutonRechercherBluetooth->setEnabled(false);
    uiIHMGroom->boutonConnecterBluetooth->setEnabled(false);
    uiIHMGroom->boutonDeconnecterBluetooth->setEnabled(true);
    uiIHMGroom->labelEtatConnexion->setText("État : connecté");

    uiIHMGroom->boutonEnvoyerOccupant->setEnabled(true);
    uiIHMGroom->lineEditNom->setEnabled(true);
    uiIHMGroom->lineEditPrenom->setEnabled(true);
    uiIHMGroom->lineEditFonction->setEnabled(true);
    uiIHMGroom->listeOccupant->setEnabled(true);
    uiIHMGroom->boutonViderFormulaire->setEnabled(true);
    uiIHMGroom->zoneTexteMessage->setEnabled(true);
    uiIHMGroom->boutonEnvoyerMessage->setEnabled(true);
    uiIHMGroom->boutonRadioLibre->setEnabled(true);
    uiIHMGroom->boutonRadioOccupe->setEnabled(true);
    uiIHMGroom->boutonRadioAbsent->setEnabled(true);
    uiIHMGroom->boutonRadioEntrer->setEnabled(true);
    uiIHMGroom->boutonPoussoirSonnette->setEnabled(true);
    uiIHMGroom->boutonActiverDesactiverPresence->setEnabled(true);
    uiIHMGroom->boutonAjouterOccupant->setEnabled(true);
    uiIHMGroom->boutonSupprimerOccupant->setEnabled(true);
    uiIHMGroom->boutonEditerOccupant->setEnabled(true);
    uiIHMGroom->boutonRadioLibre->setChecked(true);
    envoyerEtatLibre();
    int Groom = 0;
    int Sonnette = 0;
    int Presence = 0;
    int ModeSonnette = 1;
    int ModePresence = 0;
    actualiserIndicateurs(Groom, Sonnette, Presence, ModeSonnette, ModePresence);
    uiIHMGroom->labelSonnette->setPixmap(sonnetteOn.scaled(uiIHMGroom->labelSonnette->width(), uiIHMGroom->labelSonnette->height(), Qt::KeepAspectRatio));

    int index = uiIHMGroom->listeOccupant->currentIndex();
    selectionnerOccupant(index);
}

/**
 * @brief Méthode qui permet d'afficher l'état "déconnecté"
 *
 * @param nomPeripherique
 * @param adressePeripherique
 */
void IHMGroom::afficherDeconnexionGroom(QString nomPeripherique, QString adressePeripherique)
{
    qDebug() << Q_FUNC_INFO << nomPeripherique << adressePeripherique;
    uiIHMGroom->listeBluetooth->setEnabled(true);
    uiIHMGroom->boutonRechercherBluetooth->setEnabled(true);
    uiIHMGroom->boutonConnecterBluetooth->setEnabled(true);
    uiIHMGroom->boutonDeconnecterBluetooth->setEnabled(false);
    uiIHMGroom->labelEtatConnexion->setText("État : déconnecté");

    uiIHMGroom->boutonEnvoyerOccupant->setEnabled(false);
    uiIHMGroom->lineEditNom->setEnabled(false);
    uiIHMGroom->lineEditPrenom->setEnabled(false);
    uiIHMGroom->lineEditFonction->setEnabled(false);
    uiIHMGroom->listeOccupant->setEnabled(false);
    uiIHMGroom->boutonViderFormulaire->setEnabled(false);
    uiIHMGroom->zoneTexteMessage->setEnabled(false);
    uiIHMGroom->boutonEnvoyerMessage->setEnabled(false);
    uiIHMGroom->boutonRadioLibre->setEnabled(false);
    uiIHMGroom->boutonRadioOccupe->setEnabled(false);
    uiIHMGroom->boutonRadioAbsent->setEnabled(false);
    uiIHMGroom->boutonRadioEntrer->setEnabled(false);
    uiIHMGroom->boutonPoussoirSonnette->setEnabled(false);
    uiIHMGroom->boutonActiverDesactiverPresence->setEnabled(false);
    uiIHMGroom->boutonAjouterOccupant->setEnabled(false);
    uiIHMGroom->boutonSupprimerOccupant->setEnabled(false);
    uiIHMGroom->boutonEditerOccupant->setEnabled(false);
    uiIHMGroom->labelEtatSonnette->setText("État : ...");
    uiIHMGroom->labelEtatPresence->setText("État : ...");
    uiIHMGroom->labelSonnette->setAutoFillBackground(true);
    uiIHMGroom->labelSonnette->clear();
    uiIHMGroom->boutonRadioAbsent->setChecked(true);
    uiIHMGroom->lineEditNom->setText("");
    uiIHMGroom->lineEditPrenom->setText("");
    uiIHMGroom->lineEditFonction->setText("");
}

/**
 * @brief Méthode qui permet d'envoyer l'affichage de l'occupant vers le système groom
 *
 */
void IHMGroom::envoyerAffichageOccupant()
{
    if(occupants.size() < 1)
        return;
    qDebug() << Q_FUNC_INFO << "Occupant courant" << indexOccupant << uiIHMGroom->listeOccupant->currentIndex();
    qDebug() << Q_FUNC_INFO << occupants[uiIHMGroom->listeOccupant->currentIndex()].nom << occupants[uiIHMGroom->listeOccupant->currentIndex()].prenom << occupants[uiIHMGroom->listeOccupant->currentIndex()].fonction;
    /**
      * Trame : $AFFICHAGE;NOM;PRENOM;FONCTION\r\n
      */
    QString trame = "$AFFICHAGE;" + occupants[uiIHMGroom->listeOccupant->currentIndex()].nom + ";" + occupants[uiIHMGroom->listeOccupant->currentIndex()].prenom + ";" + occupants[uiIHMGroom->listeOccupant->currentIndex()].fonction + "\r\n";
    communicationGroom->envoyer(trame);
}

/**
 * @brief Méthode qui permet d'envoyer l'état libre vers le système groom
 *
 */
void IHMGroom::envoyerEtatLibre()
{
    etatGroom = EtatGroom::Libre;
    etatSonnette = EtatSonnette::Active;
    QString trame = communicationGroom->fabriquerTrame(etatGroom, etatSonnette, etatPresence);
    communicationGroom->envoyer(trame);

    if(communicationGroom->estConnecte())
    {
        uiIHMGroom->boutonPoussoirSonnette->setEnabled(true);
        uiIHMGroom->boutonActiverDesactiverPresence->setEnabled(true);
        uiIHMGroom->boutonRadioEntrer->setEnabled(true);

        int Groom = 0;
        int Sonnette = 0;
        int Presence = 0;
        int ModeSonnette = 1;
        int ModePresence = 0;
        actualiserIndicateurs(Groom, Sonnette, Presence, ModeSonnette, ModePresence);
    }
}

/**
 * @brief Méthode qui permet d'envoyer l'état absent vers le système groom
 *
 */
void IHMGroom::envoyerEtatAbsent()
{
    etatGroom = EtatGroom::Absent;
    QString trame = communicationGroom->fabriquerTrame(etatGroom, etatSonnette, etatPresence);
    communicationGroom->envoyer(trame);

    if(communicationGroom->estConnecte())
    {
        uiIHMGroom->boutonPoussoirSonnette->setEnabled(false);
        uiIHMGroom->boutonActiverDesactiverPresence->setEnabled(false);
        uiIHMGroom->boutonRadioEntrer->setEnabled(false);

        if(etatSonnette == EtatSonnette::Active)
        {
            etatSonnette = EtatSonnette::Desactive;
            uiIHMGroom->labelSonnette->setPixmap(sonnetteOff.scaled(uiIHMGroom->labelSonnette->width(), uiIHMGroom->labelSonnette->height(), Qt::KeepAspectRatio));
        }

        trame = communicationGroom->fabriquerTrame(etatGroom, etatSonnette, etatPresence);
        communicationGroom->envoyer(trame);

        if(etatPresence == EtatPresence::Marche)
        {
            etatPresence = EtatPresence::Arret;
        }

        trame = communicationGroom->fabriquerTrame(etatGroom, etatSonnette, etatPresence);
        communicationGroom->envoyer(trame);

        int Groom = 3;
        int Sonnette = 0;
        int Presence = 0;
        int ModeSonnette = 0;
        int ModePresence = 0;
        actualiserIndicateurs(Groom, Sonnette, Presence, ModeSonnette, ModePresence);
    }
}

/**
 * @brief Méthode qui permet d'envoyer l'état occupé vers le système groom
 *
 */
void IHMGroom::envoyerEtatOccupe()
{
    etatGroom = EtatGroom::Occupe;
    QString trame = communicationGroom->fabriquerTrame(etatGroom, etatSonnette, etatPresence);
    communicationGroom->envoyer(trame);

    if(communicationGroom->estConnecte())
    {
        uiIHMGroom->boutonPoussoirSonnette->setEnabled(false);
        uiIHMGroom->boutonActiverDesactiverPresence->setEnabled(false);
        uiIHMGroom->boutonRadioEntrer->setEnabled(false);

        if(etatSonnette == EtatSonnette::Active)
        {
            etatSonnette = EtatSonnette::Desactive;
            uiIHMGroom->labelSonnette->setPixmap(sonnetteOff.scaled(uiIHMGroom->labelSonnette->width(), uiIHMGroom->labelSonnette->height(), Qt::KeepAspectRatio));
        }

        trame = communicationGroom->fabriquerTrame(etatGroom, etatSonnette, etatPresence);
        communicationGroom->envoyer(trame);

        if(etatPresence == EtatPresence::Marche)
        {
            etatPresence = EtatPresence::Arret;
        }

        trame = communicationGroom->fabriquerTrame(etatGroom, etatSonnette, etatPresence);
        communicationGroom->envoyer(trame);

        int Groom = 2;
        int Sonnette = 0;
        int Presence = 0;
        int ModeSonnette = 0;
        int ModePresence = 0;
        actualiserIndicateurs(Groom, Sonnette, Presence, ModeSonnette, ModePresence);
    }
}

/**
 * @brief Méthode qui permet d'envoyer l'état entrer vers le système groom
 *
 */
void IHMGroom::envoyerEtatEntrer()
{
    etatGroom = EtatGroom::Entrez;
    QString trame = communicationGroom->fabriquerTrame(etatGroom, etatSonnette, etatPresence);
    communicationGroom->envoyer(trame);

    if(communicationGroom->estConnecte())
    {
        uiIHMGroom->boutonPoussoirSonnette->setEnabled(false);
        uiIHMGroom->boutonActiverDesactiverPresence->setEnabled(false);

        if(etatSonnette == EtatSonnette::Active)
        {
            etatSonnette = EtatSonnette::Desactive;
            uiIHMGroom->labelSonnette->setPixmap(sonnetteOff.scaled(uiIHMGroom->labelSonnette->width(), uiIHMGroom->labelSonnette->height(), Qt::KeepAspectRatio));
        }

        trame = communicationGroom->fabriquerTrame(etatGroom, etatSonnette, etatPresence);
        communicationGroom->envoyer(trame);

        if(etatPresence == EtatPresence::Marche)
        {
            etatPresence = EtatPresence::Arret;
        }

        trame = communicationGroom->fabriquerTrame(etatGroom, etatSonnette, etatPresence);
        communicationGroom->envoyer(trame);

        int Groom = 0;
        int Sonnette = 0;
        int Presence = 0;
        int ModeSonnette = 0;
        int ModePresence = 0;
        actualiserIndicateurs(Groom, Sonnette, Presence, ModeSonnette, ModePresence);
    }
}

/**
 * @brief Méthode qui permet d'envoyer l'état de la sonnette vers le système groom (le mode)
 *
 */
void IHMGroom:: envoyerEtatSonnette()
{
    QString trame;
    if(etatSonnette == EtatSonnette::Active)
    {
        etatSonnette = EtatSonnette::Desactive;
        uiIHMGroom->labelSonnette->setPixmap(sonnetteOff.scaled(uiIHMGroom->labelSonnette->width(), uiIHMGroom->labelSonnette->height(), Qt::KeepAspectRatio));
    }
    else if (etatSonnette == EtatSonnette::Desactive)
    {
        etatSonnette = EtatSonnette::Active;
        uiIHMGroom->labelSonnette->setPixmap(sonnetteOn.scaled(uiIHMGroom->labelSonnette->width(), uiIHMGroom->labelSonnette->height(), Qt::KeepAspectRatio));
    }

    trame = communicationGroom->fabriquerTrame(etatGroom, etatSonnette, etatPresence);
    communicationGroom->envoyer(trame);
}

/**
 * @brief Méthode qui permet d'envoyer l'état du capteur de présence vers le système groom (le mode)
 *
 */
void IHMGroom::envoyerEtatPresence()
{
    QString trame;
    if(etatPresence == EtatPresence::Marche)
    {
        etatPresence = EtatPresence::Arret;
    }
    else if(etatPresence == EtatPresence::Arret)
    {
        etatPresence = EtatPresence::Marche;
    }

    trame = communicationGroom->fabriquerTrame(etatGroom, etatSonnette, etatPresence);
    communicationGroom->envoyer(trame);
}

/**
 * @brief Méthode qui permet d'envoyer un message de la part de l'occupant vers le système groom
 *
 */
void IHMGroom::envoyerMessageOccupant()
{
    QString trame = communicationGroom->fabriquerTrameMessageOccupant(uiIHMGroom->zoneTexteMessage->text());
    communicationGroom->envoyer(trame);

    if(communicationGroom->estConnecte())
    {
        uiIHMGroom->zoneTexteMessage->clear();

        QStringList listeChamps = trame.split(";");
        QString chaineChampMessage = listeChamps[1];
        QString nouvelleChaineChampMessage = chaineChampMessage.remove('\n');

        if(listeChamps[1] != "\r\n")
        {
            uiIHMGroom->labelAffichageMessageOccupant->setText("Message affiché : \"" + nouvelleChaineChampMessage + "\"");
        }
        else if(listeChamps[1] == "\r\n")
        {
            uiIHMGroom->labelAffichageMessageOccupant->setText("Aucun message affiché");
        }
    }
}

/**
 * @brief Charge les paramètres dans le fichier .ini de l'application Groom
 *
 * @fn IHMGroom::chargerParametres()
 */
void IHMGroom::chargerParametres()
{
    // Fichier de stockage des noms, prénoms et fonctions des occupants de la pièce
    QString nomFichier = QApplication::applicationDirPath() + "/parametres.ini";
    qDebug() << Q_FUNC_INFO << nomFichier;
    QSettings settings(nomFichier, QSettings::IniFormat);

    int nbOccupants = settings.value("NbOccupants", 0).toInt();
    qDebug() << Q_FUNC_INFO << "NbOccupants" << nbOccupants;
    indexOccupant = settings.value("Occupant", 0).toInt();
    qDebug() << Q_FUNC_INFO << "Occupant courant" << indexOccupant;

    uiIHMGroom->listeOccupant->clear();
    for(int i = 0; i < nbOccupants; i++)
    {
        QString nomOccupant = "Occupant" + QString::number(i+1);
        settings.beginGroup(nomOccupant);
        qDebug() << Q_FUNC_INFO << nomOccupant << settings.value("Nom").toString() << settings.value("Prenom").toString() << settings.value("Fonction").toString();
        Occupant occupant;
        occupant.nom = settings.value("Nom").toString();
        occupant.prenom = settings.value("Prenom").toString();
        occupant.fonction = settings.value("Fonction").toString();
        occupants.push_back(occupant);
        settings.endGroup();
        uiIHMGroom->listeOccupant->addItem(occupant.nom + " " + occupant.prenom + " - " + occupant.fonction);
    }
    uiIHMGroom->listeOccupant->setCurrentIndex(indexOccupant-1);
}

/**
 * @brief Sauvegarde les paramètres dans le fichier .ini de l'application Groom
 *
 * @fn IHMGroom::sauvegarderParametres()
 */
void IHMGroom::sauvegarderParametres()
{
    // Fichier de stockage des noms, prénoms et fonctions des occupants de la pièce
    QString nomFichier = QApplication::applicationDirPath() + "/parametres.ini";
    qDebug() << Q_FUNC_INFO << nomFichier;
    QSettings settings(nomFichier, QSettings::IniFormat);

    qDebug() << Q_FUNC_INFO << occupants.size();
    for(int i = 0; i < occupants.size(); i++)
    {
        QString nomOccupant = "Occupant" + QString::number(i+1);
        settings.beginGroup(nomOccupant);
        settings.setValue("Nom", occupants[i].nom);
        settings.setValue("Prenom", occupants[i].prenom);
        settings.setValue("Fonction", occupants[i].fonction);
        settings.endGroup();
    }
    settings.setValue("NbOccupants", occupants.size());
    settings.setValue("Occupant", indexOccupant);
}

/**
 * @brief Méthode qui permet de rechercher le périphérique groom voulu
 *
 * @fn IHMGroom::rechercherGroom()
 */
void IHMGroom::rechercherGroom()
{
    uiIHMGroom->listeBluetooth->clear();
    uiIHMGroom->listeBluetooth->setEnabled(false);
    uiIHMGroom->boutonRechercherBluetooth->setEnabled(false);
    uiIHMGroom->boutonConnecterBluetooth->setEnabled(false);
    uiIHMGroom->boutonDeconnecterBluetooth->setEnabled(false);
    uiIHMGroom->labelEtatConnexion->setText("État : déconnecté");

    communicationGroom->rechercher();
}

/**
 * @brief Méthode qui permet d'arrêter la recherche du module groom
 *
 * @fn IHMGroom::arreterRechercheGroom()
 */
void IHMGroom::arreterRechercheGroom()
{
    uiIHMGroom->listeBluetooth->setEnabled(true);
    uiIHMGroom->boutonRechercherBluetooth->setEnabled(true);
    if(uiIHMGroom->listeBluetooth->count() > 0 )
        uiIHMGroom->boutonConnecterBluetooth->setEnabled(true);
    uiIHMGroom->boutonDeconnecterBluetooth->setEnabled(false);
    uiIHMGroom->labelEtatConnexion->setText("État : déconnecté");
}

/**
 * @brief Recharge la liste d'occupants
 *
 * @fn IHMGroom::rechargerListeOccupants()
 */
void IHMGroom::rechargerListeOccupants()
{
    disconnect(uiIHMGroom->listeOccupant, SIGNAL(currentIndexChanged(int)), this, SLOT(selectionnerOccupant(int)));
    uiIHMGroom->listeOccupant->clear();
    for(int i = 0; i < occupants.size(); i++)
    {
        uiIHMGroom->listeOccupant->addItem(occupants.at(i).nom + " " + occupants.at(i).prenom + " - " + occupants.at(i).fonction);
        if (uiIHMGroom->lineEditNom->text() != '\0')
        {
            uiIHMGroom->listeOccupant->setCurrentIndex(i);
        }
    }

    connect(uiIHMGroom->listeOccupant, SIGNAL(currentIndexChanged(int)), this, SLOT(selectionnerOccupant(int)));
}

/**
 * @brief Méthode qui permet d'actualiser les indicateurs d'états dans l'IHM (labels)
 *
 * @fn IHMGroom::actualiserIndicateurs(int Groom, int Sonnette, int Presence, int ModeSonnette, int ModePresence)
 */
void IHMGroom::actualiserIndicateurs(int Groom, int Sonnette, int Presence, int ModeSonnette, int ModePresence)
{

    /**
      Acquittement pour les états de l'occupant
      */

    switch (Groom)
    {
        case EtatGroom::Libre:
            uiIHMGroom->labelEtatOccupant->setText("État : libre");
            break;
        case EtatGroom::Absent:
            uiIHMGroom->labelEtatOccupant->setText("État : absent");
            break;
        case EtatGroom::Occupe:
            uiIHMGroom->labelEtatOccupant->setText("État : occupé");
            break;
    }

    /**
      Acquittement si sonnette appuyé
      */

    switch (Sonnette)
    {
        case DetectionSonnette::PasSonne:
            break;
        case DetectionSonnette::ASonne:
            QSystemTrayIcon::MessageIcon icone = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information);
            afficherNotification("Groom", "Quelqu'un a sonné !", icone);
            break;
    }

    /**
      Acquittement si il y a une présence
      */

    switch (Presence)
    {
        case DetectionPresence::NonDetecte:
            break;
        case DetectionPresence::Detecte:
            QSystemTrayIcon::MessageIcon icone = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information);
            afficherNotification("Groom", "Présence détecté !", icone);
            break;
    }

    /**
      Acquittement pour le mode sonnette
      */

    switch (ModeSonnette)
    {
        case EtatSonnette::Desactive:
            uiIHMGroom->labelEtatSonnette->setText("État : désactivé");
            uiIHMGroom->labelSonnette->setPixmap(sonnetteOff.scaled(uiIHMGroom->labelSonnette->width(), uiIHMGroom->labelSonnette->height(), Qt::KeepAspectRatio));
            break;
        case EtatSonnette::Active:
            uiIHMGroom->labelEtatSonnette->setText("État : activé");
            uiIHMGroom->labelSonnette->setPixmap(sonnetteOn.scaled(uiIHMGroom->labelSonnette->width(), uiIHMGroom->labelSonnette->height(), Qt::KeepAspectRatio));
            break;
    }

    /**
      Acquittement pour le mode présence
      */

    switch (ModePresence)
    {
        case EtatPresence::Arret:
            uiIHMGroom->labelEtatPresence->setText("État : désactivé");
            break;
        case EtatPresence::Marche:
            uiIHMGroom->labelEtatPresence->setText("État : activé");
            break;
    }
}

/**
 * @brief Méthode qui permet de vider le formulaire (Occupant)
 *
 * @fn IHMGroom::viderFormulaire()
 */
void IHMGroom::viderFormulaire()
{
    uiIHMGroom->lineEditNom->clear();
    uiIHMGroom->lineEditPrenom->clear();
    uiIHMGroom->lineEditFonction->clear();
}

/**
 * @brief Méthode qui permet l'ouverture d'un fichier de type .ics
 *
 * @fn IHMGroom::ouvrirFichier()
 */
void IHMGroom::ouvrirFichier()
{
    QString fileName; // QCoreApplication::applicationDirPath()

    if (fileName.isNull())
        fileName = QFileDialog::getOpenFileName(this, tr("Ouvrir le fichier"), "", tr("Fichiers iCal (*.ics)"));

    if (!fileName.isEmpty())
    {
        afficherEtat(fileName, 5000);
        fileName.prepend("file://");
        calendrier->setHostURL(fileName);
        calendrier->charger();
    }
}

/**
 * @brief Méthode qui permet l'ouverture d'un URL de type .ics
 *
 * @fn IHMGroom::ouvrirURL()
 */
void IHMGroom::ouvrirURL()
{
    bool ok = true;
    QString url = QInputDialog::getText(this, tr("Ouvrir un calendrier avec une URL"), tr("URL :"), QLineEdit::Normal, "", &ok);
    //QString url = "https://calendar.google.com/calendar/ical/robertutilisateur84%40gmail.com/private-ab9f00fcf91e8c05d89d6e5c8a1234f7/basic.ics";
    if (ok && !url.isEmpty())
    {
        afficherEtat(url, 5000);
        calendrier->setHostURL(url);
        calendrier->charger();
    }
    uiIHMGroom->labelAffichageEvenements->clear();
}

/**
 * @brief Méthode qui permet l'affichage d'un état (Calendrier)
 *
 * @fn IHMGroom::afficherEtat(QString message)
 */
void IHMGroom::afficherEtat(QString message)
{
    barreEtat->showMessage(message);
}

/**
 * @brief Méthode qui permet l'affichage d'un état avec une durée (Calendrier)
 *
 * @fn IHMGroom::afficherEtat(QString message, int duree)
 */
void IHMGroom::afficherEtat(QString message, int duree)
{
    barreEtat->showMessage(message, duree);
}

/**
 * @brief Méthode qui permet d'effacer un état (barre d'état)
 *
 * @fn IHMGroom::effacerEtat()
 */
void IHMGroom::effacerEtat()
{
    barreEtat->clearMessage();
}

/**
 * @brief Méthode qui appelle les méthodes d'effacement
 *
 * @fn IHMGroom::effacer()
 */
void IHMGroom::effacer()
{
    //effacerEtat();
    effacerEvenement();
}

/**
 * @brief Méthode qui permet d'effacer les événements affichés
 *
 * @fn IHMGroom::effacerEvenement()
 */
void IHMGroom::effacerEvenement()
{
    uiIHMGroom->evenement->clear();
}

/**
 * @brief Méthode qui permet d'accéder et d'afficher les événements (Calendrier)
 *
 * @fn IHMGroom::selectionnerDate(QDate date)
 */
void IHMGroom::selectionnerDate(QDate date)
{
    qDebug() << __FUNCTION__ << date;
    effacerEvenement();
    QList<Evenement> listeEvenements = calendrier->getEvenements(date);
    foreach(Evenement evt, listeEvenements)
    {
        qDebug() << __FUNCTION__ << evt;
        QString dtStart = evt.getAttribut("dtstart");
        QString dtEnd = evt.getAttribut("dtend");
        bool ok;
        QDateTime debutEvenement = Evenement::convertirHorodatage(dtStart, ok);
        QDateTime finEvenement = Evenement::convertirHorodatage(dtEnd, ok);
        QDateTime dts = QDateTime(debutEvenement.date(), debutEvenement.time(), Qt::UTC);
        qDebug() << dts.toTimeZone(QTimeZone("Europe/Paris"));
        QDateTime dte = QDateTime(finEvenement.date(), finEvenement.time(), Qt::UTC);
        qDebug() << dte.toTimeZone(QTimeZone("Europe/Paris"));

        if(ok)
        {
            qDebug() << Q_FUNC_INFO << "summary" << evt.getAttribut("summary");

            qDebug() << Q_FUNC_INFO << "dtstart" << dtStart << dts.toTimeZone(QTimeZone("Europe/Paris")).toString("HH:mm");
            if(dts.toTimeZone(QTimeZone("Europe/Paris")).time().minute() == 0)
            {
                qDebug() << Q_FUNC_INFO << "dtstart" << dtStart << dts.toTimeZone(QTimeZone("Europe/Paris")).toString("h'h'");
                uiIHMGroom->evenement->append("L'événement \"" + evt.getAttribut("summary") + "\" est noté du " + date.toString("dd/MM/yyyy") + " de " + dts.toTimeZone(QTimeZone("Europe/Paris")).toString("h'h'"));
            }
            else
            {
                qDebug() << Q_FUNC_INFO << "dtstart" << dtStart << dts.toTimeZone(QTimeZone("Europe/Paris")).toString("h'h'mm");
                uiIHMGroom->evenement->append("L'événement \"" + evt.getAttribut("summary") + "\" est noté du " + date.toString("dd/MM/yyyy") + " de " + dts.toTimeZone(QTimeZone("Europe/Paris")).toString("h'h'mm"));
            }

            qDebug() << Q_FUNC_INFO << "dtend" << dtStart << dte.toTimeZone(QTimeZone("Europe/Paris")).toString("HH:mm");
            if(dte.toTimeZone(QTimeZone("Europe/Paris")).time().minute() == 0)
            {
                qDebug() << Q_FUNC_INFO << "dtend" << dtStart << dte.toTimeZone(QTimeZone("Europe/Paris")).toString("h'h'");
                uiIHMGroom->evenement->append(" à " + dte.toTimeZone(QTimeZone("Europe/Paris")).toString("h'h'") + ".");
            }
            else
            {
                qDebug() << Q_FUNC_INFO << "dtend" << dtStart << dte.toTimeZone(QTimeZone("Europe/Paris")).toString("h'h'mm");
                uiIHMGroom->evenement->append(" à " + dte.toTimeZone(QTimeZone("Europe/Paris")).toString("h'h'mm") + ".");
            }
        }
        else
        {
            qDebug() << Q_FUNC_INFO << "dtstart" << dtStart << "Erreur conversion !";
        }
    }
}
