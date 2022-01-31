#include "Occupant.h"
#include <QDebug>

/**
 * @file Occupant.cpp
 * @brief Définition de la structure Occupant
 * @version 1.1
 * @author ymota <motayuri13@gmail.com>
 * $LastChangedRevision: 132 $
 * $LastChangedDate: 2021-06-08 16:11:58 +0200 (mar. 08 juin 2021) $
 */

/**
 * @brief Constructeur par défaut de la classe Occupant
 *
 * @fn Occupant::Occupant()
 */
Occupant::Occupant()
{
    qDebug() << Q_FUNC_INFO;
}

/**
 * @brief Constructeur de la classe Occupant
 *
 * @fn Occupant::Occupant(QString nom, QString prenom, QString fonction) : nom(nom), prenom(prenom), fonction(fonction)
 */
Occupant::Occupant(QString nom, QString prenom, QString fonction) : nom(nom), prenom(prenom), fonction(fonction)
{
    qDebug() << Q_FUNC_INFO;
}

/**
 * @brief Destructeur de la classe Occupant
 *
 * @fn Occupant::~Occupant()
 */
Occupant::~Occupant()
{
    //qDebug() << Q_FUNC_INFO;
}

/**
 * @brief Surcharge de l'opérateur "=="
 *
 * @fn Occupant::operator==(const Occupant &occupant) const
 */
bool Occupant::operator==(const Occupant &occupant) const
{
    if(this->nom != occupant.nom)
        return false;
    if(this->prenom != occupant.prenom)
        return false;
    if(this->fonction != occupant.fonction)
        return false;
    else
        return true;
}

/**
 * @brief Surcharge de l'opérateur "!="
 *
 * @fn Occupant::operator!=(const Occupant &occupant) const
 */
bool Occupant::operator!=(const Occupant &occupant) const
{
    return !(*this == occupant);
}
