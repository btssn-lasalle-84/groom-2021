#ifndef OCCUPANT_H
#define OCCUPANT_H

/**
 * @file Occupant.h
 * @brief Déclaration de la structure Occupant
 * @version 1.1
 * @author ymota <motayuri13@gmail.com>
 * $LastChangedRevision: 132 $
 * $LastChangedDate: 2021-06-08 16:11:58 +0200 (mar. 08 juin 2021) $
 */

#include <QString>

/**
 * @brief Structure qui définit les informations d'un occupant du bureau
 */
struct Occupant
{
    QString nom;        //!< Membre définissant le nom de l'occupant
    QString prenom;     //!< Membre définissant le prénom de l'occupant
    QString fonction;   //!< Membre définissant la fonction de l'occupant

    /**
     * @brief Constructeur par défaut
     */
    Occupant();

    /**
     * @brief Constructeur d'initialisation
     * @param nom le nom de l'occupant
     * @param prenom le prénom de l'occupant
     * @param fonction la fonction de l'occupant
     */
    Occupant(QString nom, QString prenom, QString fonction);

    /**
     * @brief Destructeur
     */
    ~Occupant();

    /**
     * @brief Surcharge de l'opérateur == afin de comparer deux occupants
     * @param occupant
     * @return bool true si les deux éléments sont identiques, sinon false
     */
    bool operator==(const Occupant &occupant) const;
    /**
     * @brief Surcharge de l'opérateur != afin de comparer deux occupants
     * @param occupant
     * @return bool true si les deux éléments sont différents, sinon false
     */
    bool operator!=(const Occupant &occupant) const;
};

#endif // OCCUPANT_H
