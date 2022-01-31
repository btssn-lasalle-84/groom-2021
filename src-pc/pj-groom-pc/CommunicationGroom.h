#ifndef COMMUNICATIONGROOM_H
#define COMMUNICATIONGROOM_H

#include <QObject>
#include <QString>
#include <QBluetoothLocalDevice>
#include <QBluetoothDeviceInfo>
#include <QBluetoothSocket>
#include <QBluetoothDeviceDiscoveryAgent>

/**
 * @file CommunicationGroom.h
 * @brief Déclaration de la classe CommunicationGroom
 * @author ymota <motayuri13@gmail.com>
 * @version 1.1
 * $LastChangedRevision: 146 $
 * $LastChangedDate: 2021-06-14 18:11:59 +0200 (lun. 14 juin 2021) $
 */

#define ENTETE_TRAME            "$"         //!< Valeur d'entête de la trame
#define FIN_TRAME               "\r\n"      //!< Valeur de fin de la trame
#define ENTETE_NOM_PERIPHERIQUE "groom-69"  //!< Valeur de l'entête du nom du périphérique "groom-1"
#define TYPE_TRAME              1           //!< Valeur du type de trame
#define TRAME_ALIVE             "A"         //!< Valeur de la trame alive
#define TRAME_ETAT              "ETAT"      //!< Valeur état de la connexion bluetooth (trame alive)

/**
 * @enum EtatGroom
 * @brief Définit les différents états du groom
 */
enum EtatGroom
{
    Libre,  //!< Le bureau de loccupant est libre d'accès
    Absent, //!< L'occupant est absent
    Occupe, //!< L'occupant est occupé
    Entrez  //!< Ordre envoyé par l'occupant au groom
};

/**
 * @enum DetectionSonnette
 * @brief Définit la détection d'un appui sur la sonnette
 */
enum DetectionSonnette
{
    PasSonne,
    ASonne
};

/**
 * @enum DetectionPresence
 * @brief Définit la détection d'unen présence
 */
enum DetectionPresence
{
    NonDetecte,
    Detecte
};

/**
 * @enum EtatSonnette
 * @brief Définit les différents états du mode Sonnette
 */
enum EtatSonnette
{
    Desactive,
    Active
};

/**
 * @enum EtatPresence
 * @brief Définit les différents états du mode Détecteur de présence
 */
enum EtatPresence
{
    Arret,
    Marche
};

/**
 * @enum Emplacements

 * @brief Définit les différents emplacement des données de la trame
 */
enum Emplacements
{
    Entete,
    Etat,
    Sonnette,
    Presence,
    ModeSonnette,
    ModePresence
};

class CommunicationGroom : public QObject
{
    Q_OBJECT
public:
    CommunicationGroom(QObject *parent = nullptr);
    ~CommunicationGroom();

    void rechercher();
    bool estConnecte();
    void envoyer(QString trame);
    QString fabriquerTrame(EtatGroom etatGroom, EtatSonnette etatSonnette, EtatPresence etatPresence);
    QString fabriquerTrameMessageOccupant(QString messageOccupant);
    QStringList traiterTrameReception(QString trame);
private:
    QBluetoothSocket *socketBluetooth;              //!< La socket de communication Bluetooth (client)
    QBluetoothLocalDevice peripheriqueBluetooth;    //!< L'interface Bluetooth du dongle PC
    QBluetoothServiceInfo serviceInfoBluetooth;     //!< Informations sur le service bluetooth
    QBluetoothDeviceInfo groom;                     //!< Le module Bluetooth GROOM
    QString nomPeripheriqueBluetooth;               //!< Nom du périphérique local
    QString nomPeripheriqueGroom;                   //!< Nom du périphérique distant
    QString adressePeripheriqueGroom;               //!< Adresse MAC du périphérique distant
    bool peripheriqueGroomTrouve;                   //!< Booléen si periphérique trouvé ou non
    QBluetoothDeviceDiscoveryAgent *discoveryAgent; //!< Pointeur vers la classe QBluetoothDeviceDiscoveryAgent de détection de périphériques
    QString trameReception;                         //!< Chaine de caractères de la trame de reception

    void initialiserBluetooth();

public slots:
    void terminerRecherche();
    void deviceDiscovered(const QBluetoothDeviceInfo &device);
    void connecter();
    void connecter(const QBluetoothDeviceInfo device);
    void deconnecter();
    void socketConnected();
    void socketDisconnected();
    void recevoir();

signals:
    void groomDetecte(QString nomPeripherique, QString adressePeripherique);
    void connexionGroom(QString nomPeripherique, QString adressePeripherique);
    void deconnexionGroom(QString nomPeripherique, QString adressePeripherique);
    void rechercheTerminee();
    void actualiserEtats(int Groom, int Sonnette, int Presence, int ModeSonnette, int ModePresence);
};

#endif // COMMUNICATIONGROOM_H
