#ifndef EVENEMENT_H
#define EVENEMENT_H

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>

/**
 * @file Evenement.h
 * @brief Déclaration de la classe Evenement
 * @author Thierry VAIRA
 * @version 1.1
 * $LastChangedRevision$
 * $LastChangedDate$
 */

/*
    BEGIN:VEVENT
    CATEGORIES:Emploi du Temps
    DTSTAMP:20191111T151814Z
    UID:Cours-243-1-20191111T151814Z-Index-Education
    DTSTART:20190905T132500Z
    DTEND:20190905T141500Z
    SUMMARY:ACCOMPAGNEMT. PERSO. - TS2 SNIR
    LOCATION:-B. 20
    DESCRIPTION:Matière : ACCOMPAGNEMT. PERSO. \n Professeur : VAIRA T. \n Classe : TS2 SNIR \n Salle : -B. 20 \n
    END:VEVENT
 */

/**
 * @class Evenement
 * @brief Déclaration de la classe Evenement
 * @details Cette classe permet de gérer un événement d'un calendrier au format iCal
*/
class Evenement : public QObject
{
    Q_OBJECT
private:
    QString calendrier;
    QString uid;
    QString dtstart;
    QString dtend;
    QString categories;
    QString summary;
    QString location;
    QString description;
    QString url;
    QMap<QString,QString> attributs;
    QList<QString> nomsAttributs;

public:
    explicit Evenement(QObject *parent = nullptr);
    Evenement(const Evenement &evt);
    Evenement& operator=(const Evenement &evt);
    QList<QString> getNomsAttributs() const;
    QString getAttribut(QString nomAttribut) const;
    void setAttribut(QString nomAttribut, QString valeur);
    QString toString() const;

    static QDateTime convertirHorodatage(QString horodatage, bool &ok);

signals:

public slots:
};

QDebug operator<<(QDebug dbg, const Evenement &e);

#endif // EVENEMENT_H
