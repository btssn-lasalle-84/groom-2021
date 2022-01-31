#include "Evenement.h"
#include <QDateTime>
#include <QDebug>

/**
 * @file Evenement.h
 * @brief Déclaration de la classe Evenement
 * @author Thierry VAIRA
 * @version 1.1
 * $LastChangedRevision$
 * $LastChangedDate$
 */

Evenement::Evenement(QObject *parent) : QObject(parent)
{
    nomsAttributs.push_back("calendrier");
    //nomsAttributs.push_back("uid");
    nomsAttributs.push_back("dtstart");
    nomsAttributs.push_back("dtend");
    nomsAttributs.push_back("categories");
    nomsAttributs.push_back("summary");
    nomsAttributs.push_back("location");
    //nomsAttributs.push_back("description");
    //nomsAttributs.push_back("url");
    foreach (const QString &key, nomsAttributs)
    {
        attributs[key] = "";
    }
}

Evenement::Evenement(const Evenement &evt) : QObject(nullptr)
{
    nomsAttributs.push_back("calendrier");
    //nomsAttributs.push_back("uid");
    nomsAttributs.push_back("dtstart");
    nomsAttributs.push_back("dtend");
    nomsAttributs.push_back("categories");
    nomsAttributs.push_back("summary");
    nomsAttributs.push_back("location");
    //nomsAttributs.push_back("description");
    //nomsAttributs.push_back("url");
    foreach (const QString &key, nomsAttributs)
    {
        attributs[key] = evt.attributs[key];
    }
}

Evenement& Evenement::operator=(const Evenement &evt)
{
    if(this != &evt)
    {
        foreach (const QString &key, nomsAttributs)
        {
            attributs[key] = evt.attributs[key];
        }
    }
    return *this;
}

QList<QString> Evenement::getNomsAttributs() const
{
    return nomsAttributs;
}

QString Evenement::getAttribut(QString nomAttribut) const
{
    return attributs[nomAttribut];
}

void Evenement::setAttribut(QString nomAttribut, QString valeur)
{
    attributs[nomAttribut] = valeur;
}

QString Evenement::toString() const
{
    QString s = "Evenement(";
    foreach (const QString &key, getNomsAttributs())
    {
        s += " " + key + " : " + getAttribut(key) + " ";
    }
    s += ")";

    return s;
}

QDateTime Evenement::convertirHorodatage(QString horodatage, bool &ok)
{
    //20200325T123000Z
    QDateTime utcTime = QDateTime::fromString(horodatage, "yyyyMMdd'T'hhmmss'Z'");
    if (utcTime.isValid())
    {
        ok = true;
        return utcTime;
    }
    utcTime = QDateTime::fromString(horodatage, "yyyyMMdd'T'hhmmss");
    if (utcTime.isValid())
    {
        ok = true;
        return utcTime;
    }
    utcTime = QDateTime::fromString(horodatage, "yyyyMMddhhmmss");
    if (utcTime.isValid())
    {
        ok = true;
        return utcTime;
    }
    utcTime = QDateTime::fromString(horodatage, "yyyyMMdd");
    if (utcTime.isValid())
    {
        ok = true;
        return utcTime;
    }

    utcTime = QDateTime();
    ok = false;

    return utcTime;
}

QDebug operator<<(QDebug dbg, const Evenement &e)
{
    dbg.nospace() << "Evenement(";
    foreach (const QString &key, e.getNomsAttributs())
    {
        dbg.nospace() << " " << key << " : " << e.getAttribut(key) << " ";
    }
    dbg.nospace() << ")";

    return dbg.space();
}
