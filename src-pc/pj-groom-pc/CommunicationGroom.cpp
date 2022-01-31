#include "CommunicationGroom.h"
#include <QDebug>

/**
 * @file CommunicationGroom.cpp
 * @brief Définition de la classe CommunicationGroom
 * @author ymota <motayuri13@gmail.com>
 * @version 1.1
 * $LastChangedRevision: 146 $
 * $LastChangedDate: 2021-06-14 18:11:59 +0200 (lun. 14 juin 2021) $
 */

/**
 * @brief Déclaration du constructeur
 *
 * @fn CommunicationGroom::CommunicationGroom(QObject *parent) : QObject(parent), socketBluetooth(nullptr), peripheriqueGroomTrouve(false), discoveryAgent(nullptr)
 */
CommunicationGroom::CommunicationGroom(QObject *parent) : QObject(parent), socketBluetooth(nullptr), peripheriqueGroomTrouve(false), discoveryAgent(nullptr)
{
    qDebug() << Q_FUNC_INFO;

    initialiserBluetooth();
}

/**
 * @brief Déclaration du destructeur
 *
 * @fn CommunicationGroom::~CommunicationGroom()
 */
CommunicationGroom::~CommunicationGroom()
{
    qDebug() << Q_FUNC_INFO;

    deconnecter();
}

/**
 * @brief Méthode inititialisation du bluetooth
 *
 * @fn void CommunicationGroom::initialiserBluetooth()
 */
void CommunicationGroom::initialiserBluetooth()
{
    // vérifier la présence du Bluetooth
    if(peripheriqueBluetooth.isValid())
    {
        // activer le bluetooth
        peripheriqueBluetooth.powerOn();

        // récupérer le nom du périphérique local
        nomPeripheriqueBluetooth = peripheriqueBluetooth.name();
        qDebug() << Q_FUNC_INFO << nomPeripheriqueBluetooth;

        discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
        connect(discoveryAgent, SIGNAL(finished()), this, SLOT(terminerRecherche()));
        connect(discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)), this, SLOT(deviceDiscovered(QBluetoothDeviceInfo)));
    }
    else qDebug() << Q_FUNC_INFO << "Pas de bluetooh !";
}

/**
 * @brief Méthode de recherche bluetooth
 *
 * @fn void CommunicationGroom::rechercher()
 */
void CommunicationGroom::rechercher()
{
    // vérifier la présence du Bluetooth
    if(!peripheriqueBluetooth.isValid())
        return;

    if(discoveryAgent == nullptr)
        return;

    qDebug() << Q_FUNC_INFO;

    // Démarrer la recherche de périphériques Bluetooth    
    peripheriqueGroomTrouve = false;
    discoveryAgent->start();
}

/**
 * @brief Slot de connexion bluetooth
 *
 * @fn void CommunicationGroom::connecter()
 */
void CommunicationGroom::connecter()
{
    if(groom.name().contains(ENTETE_NOM_PERIPHERIQUE))
        connecter(groom);
}

/**
 * @brief Slot de connexion à un module "groom-"
 *
 * @fn void CommunicationGroom::connecter(const QBluetoothDeviceInfo device)
 */
void CommunicationGroom::connecter(const QBluetoothDeviceInfo device)
{
    // vérifier la présence du Bluetooth
    if(!peripheriqueBluetooth.isValid())
        return;

    if(socketBluetooth == nullptr)
    {
        socketBluetooth = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
        connect(socketBluetooth, SIGNAL(connected()), this, SLOT(socketConnected()));
        connect(socketBluetooth, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
        connect(socketBluetooth, SIGNAL(readyRead()), this, SLOT(recevoir()));
    }

    qDebug() << Q_FUNC_INFO << device.name() << device.address().toString();
    QBluetoothAddress adresse = QBluetoothAddress(device.address());
    QBluetoothUuid uuid = QBluetoothUuid(QBluetoothUuid::SerialPort);
    socketBluetooth->connectToService(adresse, uuid);
    socketBluetooth->open(QIODevice::ReadWrite);
}

/**
 * @brief Slot de déconnexion bluetooth
 *
 * @fn void CommunicationGroom::deconnecter()
 */
void CommunicationGroom::deconnecter()
{
    // vérifier la présence du Bluetooth
    if(!peripheriqueBluetooth.isValid())
        return;
    if(socketBluetooth == nullptr)
        return;    
    if(socketBluetooth->isOpen())
    {
        qDebug() << Q_FUNC_INFO << nomPeripheriqueGroom << adressePeripheriqueGroom;
        socketBluetooth->close();
    }
}

/**
 * @brief Méthode pour valider la connexion ou non
 *
 * @fn bool CommunicationGroom::estConnecte()
 */
bool CommunicationGroom::estConnecte()
{
    // vérifier la présence du Bluetooth
    if(!peripheriqueBluetooth.isValid())
        return false;
    if(socketBluetooth == nullptr)
        return false;
    qDebug() << Q_FUNC_INFO << socketBluetooth->isOpen();
    return socketBluetooth->isOpen();
}

/**
 * @brief Slot de connexion au socket
 *
 * @fn void CommunicationGroom::socketConnected()
 */
void CommunicationGroom::socketConnected()
{    
    nomPeripheriqueGroom = socketBluetooth->peerName();
    adressePeripheriqueGroom = socketBluetooth->peerAddress().toString();
    qDebug() << Q_FUNC_INFO << nomPeripheriqueGroom << adressePeripheriqueGroom;
    emit connexionGroom(nomPeripheriqueGroom, adressePeripheriqueGroom);
}

/**
 * @brief Slot de déconnexion au socket
 *
 * @fn void CommunicationGroom::socketDisconnected()
 */
void CommunicationGroom::socketDisconnected()
{
    qDebug() << Q_FUNC_INFO << nomPeripheriqueGroom << adressePeripheriqueGroom;
    emit deconnexionGroom(nomPeripheriqueGroom, adressePeripheriqueGroom);
    nomPeripheriqueGroom.clear();
    adressePeripheriqueGroom.clear();
}

/**
 * @brief Slot de fin de recherche bluetooth
 *
 * @fn void CommunicationGroom::terminerRecherche()
 */
void CommunicationGroom::terminerRecherche()
{
    qDebug() << Q_FUNC_INFO;
    emit rechercheTerminee();
}

/**
 * @brief Slot d'affichage de périphériques
 *
 * @fn void CommunicationGroom::deviceDiscovered(const QBluetoothDeviceInfo &device)
 */
void CommunicationGroom::deviceDiscovered(const QBluetoothDeviceInfo &device)
{
    qDebug() << Q_FUNC_INFO << device.name() << device.address().toString();
    if(device.name().contains(ENTETE_NOM_PERIPHERIQUE))
    {
        qDebug() << Q_FUNC_INFO << "Module Groom trouvé !";
        discoveryAgent->stop();
        groom = device;
        peripheriqueGroomTrouve = true;
        emit groomDetecte(device.name(), device.address().toString());        
        terminerRecherche();
    }
}

/**
 * @brief Slot de reception de trame
 *
 * @fn void CommunicationGroom::recevoir()
 */
void CommunicationGroom::recevoir()
{
    /**
     * Trame envoyée par le Groom lorsque celui-ci a détecté un changement d’état (ETAT, SONNETTE et/ou PRESENCE).
     * Format : $GROOM;ETAT;SONNETTE;PRESENCE\r\n
     * Le champ ETAT peut prendre 3 valeurs différentes :
     *  Libre    0
     *  Absent   1
     *  Occupé	 2
     * Le champ SONNETTE aura la valeur 1 si le mode Sonnette est activé sinon 0.
     * Le champ PRESENCE aura la valeur 1 si la détection de présence est activée sinon 0.
     */

    QByteArray donnees;

    donnees = socketBluetooth->readAll();
    //qDebug() << Q_FUNC_INFO << donnees;

    trameReception += QString(donnees.data());
    //qDebug() << Q_FUNC_INFO << trameReception;

    if(trameReception.contains(ENTETE_TRAME) && trameReception.contains(FIN_TRAME))
    {
        QStringList trames = trameReception.split(FIN_TRAME, QString::SkipEmptyParts);
        qDebug() << Q_FUNC_INFO << trames;

        for(int i = 0; i < trames.count(); ++i)
        {
            if(trames[i].contains(ENTETE_TRAME))
            {
                qDebug() << Q_FUNC_INFO << trames[i];

                QStringList listeDonnees = traiterTrameReception(trames[i]);

                if (listeDonnees[TYPE_TRAME] != TRAME_ETAT)
                {
                    emit actualiserEtats(listeDonnees[Emplacements::Etat].toInt(), listeDonnees[Emplacements::Sonnette].toInt(), listeDonnees[Emplacements::Presence].toInt(), listeDonnees[Emplacements::ModeSonnette].toInt(), listeDonnees[Emplacements::ModePresence].toInt());
                }
            }
        }

        trameReception.clear();
    }
}

/**
 * @brief Méthode d'envoi de trame
 *
 * @fn void CommunicationGroom::envoyer(QString trame)
 */
void CommunicationGroom::envoyer(QString trame)
{
    // vérifier la présence du Bluetooth
    if(!peripheriqueBluetooth.isValid())
        return;
    if(!estConnecte())
        return;
    qDebug() << Q_FUNC_INFO << trame;
    socketBluetooth->write(trame.toLocal8Bit());
}

/**
 * @brief Méthode de fabrication de la trame "CMD"
 *
 * @fn QString CommunicationGroom::fabriquerTrame(EtatGroom etatGroom, EtatSonnette etatSonnette, EtatPresence etatPresence)
 */
QString CommunicationGroom::fabriquerTrame(EtatGroom etatGroom, EtatSonnette etatSonnette, EtatPresence etatPresence)
{
    /**
      * Trame : $CMD;ETAT;SONNETTE;PRESENCE\r\n
        Le champ ETAT peut prendre les valeurs différentes :
            Libre   0
            Absent  1
            Occupé  2
            Entrez  3
        Le champ SONNETTE aura la valeur 1 dans le cas où la personne active le mode Sonette sinon 0.
        Le champ PRESENCE aura la valeur 1 dans le cas où la personne active le détecteur de présence sinon 0.
      */
    QString trame = "$CMD;" + QString::number(etatGroom) + ";" + QString::number(etatSonnette) + ";" + QString::number(etatPresence) + "\r\n";
    qDebug() << Q_FUNC_INFO << trame;

    return trame;
}

/**
 * @brief Méthode de fabrication de la trame "MSGPERSO"
 *
 * @fn QString CommunicationGroom::fabriquerTrameMessageOccupant(QString messageOccupant)
 */
QString CommunicationGroom::fabriquerTrameMessageOccupant(QString messageOccupant)
{
    /**
      * Format :  $MSGPERSO;messageperso\r\n
        Cette trame sera envoyé que lorsque l’utilisateur du bureau voudra afficher un message personnalisé court.
        Exemple : $MSGPERSO;Je reviens dans 10 minutes\r\n
        Le message "Je reviens dans 10 minutes" sera affiché sur l’écran du groom.
      */
    QString trame = "$MSGPERSO;" + messageOccupant + "\r\n";
    qDebug() << Q_FUNC_INFO << trame;

    return trame;
}

/**
 * @brief Méthode de traitement de trame de l'état de l'occupant
 *
 * @fn QStringList CommunicationGroom::traiterTrameReception(QString trame)
 */
QStringList CommunicationGroom::traiterTrameReception(QString trame)
{

    /**
      * Format :    $GROOM;ETAT;SONNETTE;PRESENCE;MODE_SONNETTE;MODE_PRESENCE\r\n
        Le champ ETAT peut prendre 3 valeurs différentes :
            Libre        0
            Absent       1
            Occupé       2
        Le champ SONNETTE précise si un appel a été réalisé.
        Le champ PRESENCE aura la valeur 1 dans le cas où une personne a été détectée devant le groom.
        Le champ MODE_SONNETTE précise l’état d’activation 1 (activé) ou 0 (désactivé) de la sonnette.
        Le champ MODE_PRESENCE précise l’état d’activation 1 (activé) ou 0 (désactivé) du détecteur de présence.
        Cette trame est envoyée par le Groom lorsque celui-ci a détecté un changement d’état (ETAT, SONNETTE et/ou PRESENCE) ou de mode (MODE_SONNETTE/MODE_PRESENCE).
            Exemple :   $GROOM;0;1;1;1;1\r\n
      */
    qDebug() << Q_FUNC_INFO << trame;
    QStringList champs = trame.split(";");
    qDebug() << Q_FUNC_INFO << champs;

    return champs;
}
