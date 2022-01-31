#ifndef CALENDRIER_H
#define CALENDRIER_H

#include <QObject>
#include <QTimer>
#include <QList>
#include <QDate>
#include <QUrl>
#include <QTextStream>
#include <QtNetwork>
#include "Evenement.h"

/**
 * @file Calendrier.h
 * @brief Déclaration de la classe Calendrier
 * @author Thierry VAIRA
 * @version 1.1
 * $LastChangedRevision$
 * $LastChangedDate$
 */

/**
 * @enum EtatCalendrier
 * @brief Définit les différents états pour la gestion du calendrier
 */
typedef enum
{
    E_STATE_IDLE,
    E_STATE_BUSY,
    E_STATE_ERROR
} EtatCalendrier;

/**
 * @class Calendrier
 * @brief Déclaration de la classe Calendrier
 * @details Cette classe permet de gérer un calendrier au format iCal
 */
class Calendrier : public QObject
{
    Q_OBJECT
private:
    QUrl                  hostURL;
    QString               nom;
    QTimer                synchronisationTimer;
    QTextStream*          contenuCalendrier;
    QNetworkAccessManager networkManager;
    QNetworkReply*        networkReply;
    QTimer                requeteTimeoutTimer;
    int                   requeteTimeoutMS;
    EtatCalendrier        etatCalendrier;
    QList<Evenement>      evenements;

    void traiterCalendrier();
    void traiterEvenement();
    QString encoderBase64(QString string);
    QString asciiToUtf8(QString str);

public:
    explicit Calendrier(QObject *parent = nullptr);
    ~Calendrier();

    void charger();
    void telecharger();

    bool setHostURL(const QString hostURL);
    bool setHostURL(const QUrl hostURL);
    QString getHostURL(void) const;
    QString getNom(void) const;
    void setNom(QString nom);
    int getRequeteTimeoutMS(void) const;
    void setRequeteTimeoutMS(const int requestTimeoutMS);
    QList<Evenement> getEvenements(QDate date) const;

signals:
    void forcerSynchronisation();
    void effacement();
    void erreur(QString erreur, QString couleur="red");
    void message(QString message, QString couleur="green");
    void nouvelEvenement(QString evt, QString couleur="blue");

public slots:
    void recharger();
    void traiterRequete();
    void handleHTTPError();
};

#endif // CALENDRIER_H
