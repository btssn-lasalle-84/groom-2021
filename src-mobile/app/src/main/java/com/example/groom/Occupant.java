package com.example.groom;

/**
 * @file Occupant.java
 * @brief Déclaration de la classe Occupant
 * @author ROUGIER Alexander
 * $LastChangedRevision: 145 $
 * $LastChangedDate: 2021-06-13 10:30:15 +0200 (Sun, 13 Jun 2021) $
 */

import android.util.Log;

/**
 * @class Occupant
 * @brief Informations sur l'occupant du bureau
 */
public class Occupant
{
    // Constantes
    private static final String TAG = "Occupant"; //!< TAG pour les logs

    // Attributs
    private String nom; //!< Le nom de l'occupant
    private String prenom; //!< Le prénom de l'occupant
    private String fonction; //!< La fonction de l'occupant

    /**
     * @brief Constructeur
     * @param nom Le nom de l'occupant
     */
    public Occupant(String nom)
    {
        Log.d(TAG, "Occupant(String nom) nom = " + nom);
        this.nom = nom;
        this.prenom = "";
        this.fonction = "";
    }

    /**
     * @brief Constructeur
     * @param nom Le nom de l'occupant
     * @param prenom Le prénom de l'occupant
     * @param fonction La fonction de l'occupant
     */
    public Occupant(String nom, String prenom, String fonction)
    {
        Log.d(TAG, "Occupant(String nom, String prenom, String fonction) nom = " + nom);
        this.nom = nom;
        this.prenom = prenom;
        this.fonction = fonction;
    }

    /**
     * @brief récupère le nom de l'occupant
     */
    public String getNom()
    {
        if(this.nom == null)
            return "";
        return this.nom;
    }

    /**
     * @brief défini le nom de l'occupant
     */
    public void setNom(String nom)
    {
        this.nom = nom;
    }

    /**
     * @brief récupère le prénom de l'occupant
     */
    public String getPrenom()
    {
        if(this.prenom == null)
            return "";
        return this.prenom;
    }

    /**
     * @brief défini le prénom de l'occupant
     */
    public void setPrenom(String prenom)
    {
        this.prenom = prenom;
    }

    /**
     * @brief récupère la fonction de l'occupant
     */
    public String getFonction()
    {
        if(this.fonction == null)
            return "";
        return this.fonction;
    }

    /**
     * @brief défini la fonction de l'occupant
     */
    public void setFonction(String fonction)
    {
        this.fonction = fonction;
    }
}
