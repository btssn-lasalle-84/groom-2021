#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/**
 * @file IHMGroom.h
 * @brief Déclaration de la classe IHMGroom
 * @version 1.1
 * @author ymota <motayuri13@gmail.com>
 * $LastChangedRevision: 146 $
 * $LastChangedDate: 2021-06-14 18:11:59 +0200 (lun. 14 juin 2021) $
 */

#include <QtWidgets>
#include <QVector>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSettings>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QStringList>
#include "CommunicationGroom.h"
#include "Occupant.h"
#include "Calendrier.h"
#include "Evenement.h"

/**
 * @def TEST_NOTIFICATION
 * @brief Pour tester le système de notification
 */
//#define TEST_NOTIFICATION

namespace Ui
{
    class IHMGroom;
}

/**
 * @class IHMGroom
 * @brief Déclaration de la classe IHMGroom
 * @details Cette classe s'occupe de l'affichage de l'IHM du Groom
*/
class IHMGroom : public QMainWindow
{
    Q_OBJECT

public:
    IHMGroom(QWidget *parent = nullptr);                                                    //!< Constructeur de la classe IHMGroom
    ~IHMGroom();                                                                            //!< Destructeur de la classe IHMGroom

protected:
    /**
     * @brief
     * Méthode redéfinie qui est appelée automatiquement lors d'une demande de fermeture
     * @fn IHMGroom::closeEvent(QCloseEvent *event)
     * @param event L'évènement de fermeture reçu
     */
    void closeEvent(QCloseEvent *event);

public slots:
    /**
     * @brief Affichage des notifications
     * @fn afficherNotification()
     */
    void afficherNotification(QString titre, QString message, QSystemTrayIcon::MessageIcon icone, int duree = 1000);
    /**
     * @brief Acquitter les notifications
     * @fn acquitterNotification()
     */
    void acquitterNotification();

    #ifdef TEST_NOTIFICATION
    /**
     * @brief Tester les notifications
     * @fn testerNotification()
     */
    void testerNotification();
    #endif
    void recupererRequeteCalendrier();
    void traiterRequete();
    void selectionnerOccupant(int index);
    void ajouterOccupant();
    void modifierOccupant();
    void supprimerOccupant();
    void detecterGroom(QString nomPeripherique, QString adressePeripherique);
    void connecterGroom();
    void deconnecterGroom();
    void afficherConnexionGroom(QString nomPeripherique, QString adressePeripherique);
    void afficherDeconnexionGroom(QString nomPeripherique, QString adressePeripherique);
    void envoyerAffichageOccupant();
    void envoyerEtatLibre();
    void envoyerEtatAbsent();
    void envoyerEtatOccupe();
    void envoyerEtatEntrer();
    void envoyerEtatSonnette();
    void envoyerEtatPresence();
    void envoyerMessageOccupant();
    void rechercherGroom();
    void arreterRechercheGroom();
    void actualiserIndicateurs(int Groom, int Sonnette, int Presence, int ModeSonnette, int ModePresence);
    void viderFormulaire();
    void ouvrirFichier();
    void ouvrirURL();
    void afficherEtat(QString message);
    void afficherEtat(QString message, int duree);
    void effacerEtat();
    void effacer();
    void effacerEvenement();
    void selectionnerDate(QDate date);

private:
    Ui::IHMGroom *uiIHMGroom;                       //!< Association vers l'interface utilisateur
    QVector<Occupant> occupants;                    //!< Conteneur pour les objets Occupant
    int indexOccupant;                              //!< Index de l'occupant courant
    CommunicationGroom *communicationGroom;         //!< Pointeur vers l'objet communicationGroom de la classe CommunicationGroom
    EtatGroom etatGroom;                            //!< Stockage état groom
    EtatSonnette etatSonnette;                      //!< Stockage état sonnette
    EtatPresence etatPresence;                      //!< Stockage état présence
    QPixmap sonnetteOff;                            //!< Icône sonnette en mode off
    QPixmap sonnetteOn;                             //!< Icône sonnette en mode onn

    QSystemTrayIcon *iconeSysteme;                  //!< L'icône de l'application pour la barre système
    QMenu *menuIconeSysteme;                        //!< Le menu de l'application
    QAction *actionMinimiser;                       //!< L'action minimiser l'application
    QAction *actionMaximiser;                       //!< L'action maximiser l'application
    QAction *actionRestaurer;                       //!< L'action restaurer l'application
    QAction *actionQuitter;                         //!< L'action quitter l'application
    bool etatInitialIconeSysteme;                   //!< Booléen indiquant si c'est la première demande Quitter
    QNetworkAccessManager networkManager;           //!< Objet pour la classe QNetworkAccessManager
    QNetworkRequest requete;                        //!< Objet pour la classe QNetworkRequest
    QNetworkReply* networkReply;                    //!< Pointeur vers la classe QNetworkReply
    QStatusBar *barreEtat;                          //!< Pointeur vers la classe QStatusBar
    Calendrier *calendrier;                         //!< Pointeur vers la classe Calendrier

    /**
     * @brief Affichage dans barre des taches
     * @fn initialiserIconeSysteme()
     */
    void initialiserIconeSysteme();
    /**
     * @brief Charge les paramètres de l'application
     * @fn chargerParametres()
     */
    void chargerParametres();
    /**
     * @brief Sauvegarde les paramètres de l'application
     * @fn sauvegarderParametres()
     */
    void sauvegarderParametres();
    /**
     * @brief Initialise l'IHM GROOM
     * @fn initialiserIHM()
     */
    void initialiserIHM();
    /**
     * @brief Recharge la liste d'occupants
     * @fn rechargerListeOccupants()
     */
    void rechargerListeOccupants();
};

#endif // MAINWINDOW_H
