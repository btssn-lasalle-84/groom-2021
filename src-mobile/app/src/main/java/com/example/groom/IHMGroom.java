package com.example.groom;

import android.annotation.SuppressLint;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Build;
import android.os.Bundle;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.core.app.NotificationCompat;
import androidx.core.content.ContextCompat;
import androidx.webkit.WebSettingsCompat;
import androidx.webkit.WebViewFeature;

import android.os.CountDownTimer;
import android.os.Handler;
import android.os.Message;
import android.util.Base64;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;

import android.view.Menu;
import android.view.MenuItem;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.RadioButton;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;

/**
 * @file IHMGroom.java
 * @brief Déclaration de la classe IHMGroom
 * @author ROUGIER Alexander
 * $LastChangedRevision: 145 $
 * $LastChangedDate: 2021-06-13 10:30:15 +0200 (Sun, 13 Jun 2021) $
 */

/**
 * @class IHMGroom
 * @brief L'activité principale de l'application Groom
 */
public class IHMGroom extends AppCompatActivity
{
    // Constantes
    private static final String TAG = "_IHMGroom"; //!< TAG pour les logs
    private static final int ETAT_LIBRE = 0; //!< L'état Libre
    private static final int ETAT_ABSENT = 1;//!< L'état Absent
    private static final int ETAT_OCCUPE = 2;//!< L'état Occupe
    private static final int ETAT_ENTRER = 3;//!< L'état entrer
    private static final int SONNETTE_ON = 1;//!< Etat sonnette allumee
    private static final int SONNETTE_OFF = 0;//!< Etat sonnette eteinte
    private static final int DETECTEUR_ON = 1;//!< Etat detecteur allumee
    private static final int DETECTEUR_OFF = 0;//!< Etat detecteur eteinte
    private static final String messageSonnette = "On sonne à l'entrée !";

    // Attibuts
    private Occupant occupant;//!< constructeur occupant
    private String nomOccupant;//!< nom de l'occupant
    private String prenomOccupant;//!< prenom de l'occupant
    private String fonctionOccupant;//!< fonction de l'occupant
    private String nouvelUrl;//!< modification de l'URL pour l'agenda

    private int etatOccupant;//!< etat de l'occupant
    private int etatModeSonnette;//!< etat du mode de la sonnette
    private int etatModeDetecteur;//!< etat du mode de detection

    private CommunicationGroom communicationGroom = null;
    private int choixGroom = -1;

    private NotificationManager notificationManager = null;
    private int numeroNotification = 1;

    // connexionIdentification
    private TextView titreConnexion;//!< titre pour indiquer la zone de connexion
    private TextView texteNomOccupant;//!< texte montrant le nom de l'occupant
    private Spinner listeAppareilsBluetooth;//!< Affichage du peripherique actuellement et anciennement connecte
    private Button boutonConnexion;//!< bouton de connexion
    private RadioButton etatConnexion;//!< affichage de la connexion
    private Button boutonAjoutOccupant;//!<  bouton ajout d'un occupant
    private Button boutonEnvoieOccupant;//!< bouton d'envoie du nom prenom et fonction de l'occupant
    private EditText texteNomFonction;

    // zoneMessageOccupant
    private TextView titreMessageOccupant;//!< titre pour indiquer la zone du message occupant
    private EditText texteMessageOccupant;//!< zone ou entrer le message personnel
    private Button envoyerMessageOccupant;//!< bouton d'envoie du message

    // zoneEtat
    private TextView titreEtat;//!< titre pour indiquer la zone des etats de l'occupant du bureau
    private RadioButton boutonRadioLibre;//!< bouton pour choisir l'etat libre
    private RadioButton boutonRadioOccupe;//!< bouton pour choisir l'etat occupe
    private RadioButton boutonRadioAbsent;//!< boton pour choisir l'etat absent

    // zoneEntrerSonnette
    private Button boutonEntrer;//!< bouton pour choisir l'etat entrer
    private Button boutonSonnette;//!< bouton pour activer/desactiver le mode sonnette
    private ImageView imageClochette;//!< image pour afficher la notification d'un visiteur qui sonne
    private ImageView imagePresence;//!< image pour afficher la notification d'un visiteur qui est détecté

    //zonePresence
    private Button boutonPresence;//!< activation/desactivation du mode de presence
    private RadioButton boutonRadioActivationPresence;//!< affichage de l'etat du mode de presence
    private RadioButton boutonRadioEtatPresence;//!< affichage de la presence a l'entree

    private Button boutonAjoutUrl;
    private AlertDialog.Builder modificationUrl;
    private View layout;
    private Toast toast;
    private AlertDialog.Builder modificationOccupant; //!< Fenêtre de dialogue pour modifier un

    // Stockage local des informations sur l'occupant
    SharedPreferences sharedPreferences; //!<  système de persistance de données (clé/valeur) pour l’application
    public static final String PREFERENCES = "groom";//!< clé pour le titre du stockage
    public static final String PREFERENCES_NOM_OCCUPANT = "NomOccupant"; //!< clé pour le nom de l'occupant
    public static final String PREFERENCES_PRENOM_OCCUPANT = "PrenomOccupant";//!< clé pour le prénom de l'occupant
    public static final String PREFERENCES_FONCTION = "FonctionOccupant";//!< clé pour la fonction de l'occupant

    /**
     * @brief Méthode appelée à la création de l'activité
     */
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Log.d(TAG, "onCreate()");
        //Log.d(TAG, "version API = " + Build.VERSION.SDK_INT);

        setTitle(getString(R.string.app_name) + " v" + BuildConfig.VERSION_NAME);

        // Pour le stockage local des informations sur l'occupant
        sharedPreferences = getBaseContext().getSharedPreferences(PREFERENCES, MODE_PRIVATE);

        initialiserSystemeNotification();

        recupererInformationsOccupant();

        initialiserWidgets();

        //etatOccupant = ETAT_ABSENT;
        etatModeDetecteur = DETECTEUR_OFF;
        etatModeSonnette = SONNETTE_OFF;

        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        nouvelUrl =  "<iframe src=\"https://calendar.google.com/calendar/embed?src=robertutilisateur84%40gmail.com&ctz=Europe%2FParis\" style=\"border: 0\" width=\"760\" height=\"500\" frameborder=\"0\" scrolling=\"no\"></iframe>";
        chargerCalendrier();
    }

    /**
     * @brief Méthode appelée au démarrage après le onCreate() ou un restart après un onStop()
     */
    @Override
    protected void onStart()
    {
        super.onStart();
        Log.d(TAG, "onStart()");
    }

    /**
     * @brief Méthode appelée après onStart() ou après onPause()
     */
    @Override
    protected void onResume()
    {
        super.onResume();
        Log.d(TAG, "onResume()");
    }

    /**
     * @brief Méthode appelée après qu'une boîte de dialogue s'est affichée (on reprend sur un
     * onResume()) ou avant onStop() (activité plus visible)
     */
    @Override
    protected void onPause()
    {
        super.onPause();
        Log.d(TAG, "onPause()");
    }

    /**
     * @brief Méthode appelée lorsque l'activité n'est plus visible
     */
    @Override
    protected void onStop()
    {
        super.onStop();
        Log.d(TAG, "onStop()");
    }

    /**
     * @brief Méthode appelée à la destruction de l'application (après onStop() et détruite par
     * le système Android)
     */
    @Override
    protected void onDestroy()
    {
        super.onDestroy();
        Log.d(TAG, "onDestroy()");
        if(communicationGroom != null)
            communicationGroom.deconnecter();
    }

    /**
     * @brief Récupère et crée l'objet occupant avec les informations stockées localement
     */
    private void recupererInformationsOccupant()
    {
        // On récupère l'élément si il existe
        if (sharedPreferences.contains(PREFERENCES_NOM_OCCUPANT))
        {
            nomOccupant = sharedPreferences.getString(PREFERENCES_NOM_OCCUPANT, "COPIN"); // null ou une valeur par défaut
        }

        if (sharedPreferences.contains(PREFERENCES_PRENOM_OCCUPANT))
        {
            prenomOccupant = sharedPreferences.getString(PREFERENCES_PRENOM_OCCUPANT, "Olivier"); // null ou une valeur par défaut
        }

        if (sharedPreferences.contains(PREFERENCES_FONCTION))
        {
            fonctionOccupant = sharedPreferences.getString(PREFERENCES_FONCTION, "DDFPT"); // null ou une valeur par défaut
        }

        // Crée l'objet occupant avec les informations récupérées localement
        occupant = new Occupant(nomOccupant, prenomOccupant, fonctionOccupant);
    }

    /**
     * @brief Initialise les widgets de l'IHM
     */
    private void initialiserWidgets()
    {
        titreConnexion = (TextView) findViewById(R.id.titreConnexion);
        texteNomOccupant = (TextView) findViewById(R.id.texteNomOccupant);
        listeAppareilsBluetooth = (Spinner) findViewById(R.id.listeAppareilsBluetooth);

        boutonConnexion = (Button) findViewById(R.id.boutonConnexion);
        boutonConnexion.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                Log.d(TAG, "onClick() boutonConnexion : " + boutonConnexion.getText().toString());
                if(boutonConnexion.getText().toString().equals("Connecter"))
                {
                    if(communicationGroom != null)
                    {
                        communicationGroom.connecter();
                    }
                }
                else
                {
                    if(communicationGroom != null)
                    {
                        communicationGroom.deconnecter();
                    }
                }
            }
        });
        etatConnexion = (RadioButton) findViewById(R.id.etatConnexion);

        boutonAjoutOccupant = (Button) findViewById(R.id.boutonAjoutOccupant);
        boutonAjoutOccupant.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                Log.d(TAG, "onClick() boutonAjoutOccupant");
                modifierOccupant();
            }
        });
        boutonEnvoieOccupant = (Button) findViewById(R.id.boutonEnvoieOccupant);
        boutonEnvoieOccupant.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                if(communicationGroom != null)
                    communicationGroom.envoyer("$AFFICHAGE;" + occupant.getNom() + ";" + occupant.getPrenom() + ";" + occupant.getFonction() + "\r\n");
                Log.d(TAG, "onClick() boutonEnvoieOccupant");
            }
        });
        texteNomFonction = (EditText) findViewById(R.id.texteNomFonction);
        if(!occupant.getNom().isEmpty())
            texteNomFonction.setText(occupant.getNom() + " " + occupant.getPrenom() + " - " + occupant.getFonction());
        texteNomFonction.setEnabled(false);

        // zoneMessageOccupant
        titreMessageOccupant = (TextView) findViewById(R.id.titreMessageOccupant);
        texteMessageOccupant = (EditText) findViewById(R.id.texteMessageOccupant);
        envoyerMessageOccupant = (Button) findViewById(R.id.envoyerMessageOccupant);
        envoyerMessageOccupant.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                if(communicationGroom != null)
                    communicationGroom.envoyer("$MSGPERSO;" + texteMessageOccupant.getText() + "\r\n");
                Log.d(TAG, "onClick() envoyerMessageOccupant");
            }
        });

        // zoneEtat
        titreEtat = (TextView) findViewById(R.id.titreEtat);
        boutonRadioLibre = (RadioButton) findViewById(R.id.boutonRadioLibre);
        boutonRadioLibre.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                Log.d(TAG, "onClick() boutonRadioLibre");
                etatOccupant = ETAT_LIBRE;
                if(communicationGroom != null)
                    communicationGroom.envoyer("$CMD;" + etatOccupant + ";" + etatModeSonnette + ";" + etatModeDetecteur + "\r\n");
                boutonSonnette.setEnabled(true);
                boutonPresence.setEnabled(true);
            }
        });
        boutonRadioOccupe = (RadioButton) findViewById(R.id.boutonRadioOccupe);
        boutonRadioOccupe.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                Log.d(TAG, "onClick() boutonRadioOccupe");
                etatOccupant = ETAT_OCCUPE;
                etatModeSonnette = SONNETTE_OFF;
                boutonSonnette.setEnabled(false);
                boutonPresence.setEnabled(true);
                boutonSonnette.setText("Activer Sonnette");
                if(communicationGroom != null)
                    communicationGroom.envoyer("$CMD;" + etatOccupant + ";" + etatModeSonnette + ";" + etatModeDetecteur + "\r\n");
            }
        });
        boutonRadioAbsent = (RadioButton) findViewById(R.id.boutonRadioAbsent);
        boutonRadioAbsent.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                Log.d(TAG, "onClick() boutonRadioAbsent");
                etatOccupant = ETAT_ABSENT;
                etatModeDetecteur = DETECTEUR_OFF;
                etatModeSonnette = SONNETTE_OFF;
                boutonPresence.setText("Activer");
                affichageDesactiverModePresence();
                boutonPresence.setEnabled(false);
                boutonSonnette.setEnabled(false);
                boutonSonnette.setText("Activer Sonnette");
                if(communicationGroom != null)
                    communicationGroom.envoyer("$CMD;" + etatOccupant + ";" + etatModeSonnette + ";" + etatModeDetecteur + "\r\n");
            }
        });

        // zoneEntrerSonnette
        boutonEntrer = (Button) findViewById(R.id.boutonEntrer);
        boutonEntrer.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                Log.d(TAG, "onClick() boutonEntrer");
                etatOccupant = ETAT_ENTRER;
                etatModeSonnette = SONNETTE_OFF;
                if(communicationGroom != null)
                    communicationGroom.envoyer("$CMD;" + etatOccupant + ";" + etatModeSonnette + ";" + etatModeDetecteur + "\r\n");
                boutonSonnette.setEnabled(false);
                boutonSonnette.setText("Activer Sonnette");
            }
        });
        boutonSonnette = (Button) findViewById(R.id.boutonSonnette);
        boutonSonnette.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                if(boutonSonnette.getText().toString().equals("Activer Sonnette"))
                {
                    Log.d(TAG, "onClick() boutonSonnette");
                    etatOccupant = ETAT_LIBRE;
                    etatModeSonnette = SONNETTE_ON;
                    boutonSonnette.setText("Désactiver Sonnette");
                    if(communicationGroom != null)
                        communicationGroom.envoyer("$CMD;" + etatOccupant + ";" + etatModeSonnette + ";" + etatModeDetecteur + "\r\n");
                }
                else
                {
                    Log.d(TAG, "onClick() boutonSonnette");
                    etatOccupant = ETAT_LIBRE;
                    etatModeSonnette = SONNETTE_OFF;
                    boutonSonnette.setText("Activer Sonnette");
                    if(communicationGroom != null)
                        communicationGroom.envoyer("$CMD;" + etatOccupant + ";" + etatModeSonnette + ";" + etatModeDetecteur + "\r\n");
                }
            }
        });

        // Donne le focus
        boutonSonnette.setFocusableInTouchMode(true);
        boutonSonnette.requestFocus();

        boutonPresence= (Button) findViewById(R.id.boutonPresence);
        boutonPresence.setOnClickListener(new View.OnClickListener(){
            public void onClick(View v)
            {
                Log.d(TAG, "onClick() boutonPresence :" + boutonPresence.getText().toString());
                if(boutonPresence.getText().toString().equals("Desactiver"))
                {
                    etatModeDetecteur = DETECTEUR_OFF;
                    boutonPresence.setText("Activer");
                    affichageDesactiverModePresence();
                    if(communicationGroom != null)
                        communicationGroom.envoyer("$CMD;" + etatOccupant + ";" + etatModeSonnette + ";" + etatModeDetecteur + "\r\n");
                }
                else
                {
                    etatModeDetecteur = DETECTEUR_ON;
                    boutonPresence.setText("Désactiver");
                    affichageActiverModePresence();
                    if(communicationGroom != null)
                        communicationGroom.envoyer("$CMD;" + etatOccupant + ";" + etatModeSonnette + ";" + etatModeDetecteur + "\r\n");
                }
            }
        });

        boutonRadioActivationPresence = (RadioButton) findViewById(R.id.boutonRadioActivationPresence);
        boutonRadioEtatPresence = (RadioButton) findViewById(R.id.boutonRadioEtatPresence);

        boutonAjoutUrl = (Button) findViewById(R.id.boutonAjoutUrl);
        boutonAjoutUrl.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                Log.d(TAG, "onClick() boutonAjoutUrl");
                modifierUrl();
            }
        });

        afficherDeconnexionGroom();

        initialiserListeGroom();
    }

    /**
     * @brief Initialise la liste des périphériques Groom
     */
    private void initialiserListeGroom()
    {
        ArrayList<String> listePeripheriques = new ArrayList<String>();
        for (BluetoothDevice device : CommunicationGroom.getListeGrooms())
        {
            listePeripheriques.add(device.getName());
        }
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(getApplicationContext(), android.R.layout.simple_spinner_item, listePeripheriques);

        listeAppareilsBluetooth.setAdapter(adapter);
        listeAppareilsBluetooth.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener()
        {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id)
            {
                Log.d(TAG, "onItemSelected() position : " + position + " -> " + CommunicationGroom.getListeGrooms().get(position).getName() + " [" + CommunicationGroom.getListeGrooms().get(position).getAddress() + "]");
                if(choixGroom != position)
                {
                    choixGroom = position;
                    communicationGroom = new CommunicationGroom(CommunicationGroom.getListeGrooms().get(position), handler);
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent)
            {

            }
        });
    }

    /**
     * @brief Gère la boîte de dialogue permettant de modifier les information de l'occupant
     */
    private void modifierOccupant()
    {
        Log.d(TAG, "modifierOccupant()");
        modificationOccupant = new AlertDialog.Builder(this);

        modificationOccupant.setTitle("GROOM");
        modificationOccupant.setMessage("Modifier les informations de l'occupannt : ");
        //dialogueModificationOccupant.setView(R.layout.dialogue_modification_occupant);
        // ou :
        LayoutInflater factory = LayoutInflater.from(this);
        final View ajoutJoueurView = factory.inflate(R.layout.dialogue_modification_occupant, null);
        modificationOccupant.setView(ajoutJoueurView);

        modificationOccupant.setPositiveButton("Modifier", new DialogInterface.OnClickListener()
        {
            @Override
            public void onClick(DialogInterface dialog, int which)
            {
                // Récupère les informations saisies
                EditText editNomOccupant =
                        (EditText) ((AlertDialog) dialog).findViewById(R.id.nomOccupant);
                EditText editPrenomOccupant =
                        (EditText) ((AlertDialog) dialog).findViewById(R.id.prenomOccupant);
                EditText editFonctionOccupant =
                        (EditText) ((AlertDialog) dialog).findViewById(R.id.fonctionOccupant);
                Log.v(TAG,
                        "Modification occupant : " + "Nom = " + editNomOccupant.getText().toString() + " - " + "Prénom = " + editPrenomOccupant.getText().toString() + " - " + "Fonction = " + editFonctionOccupant.getText().toString());

                // Modifie et affiche les nouvelles informations de l'occupant
                occupant.setNom(editNomOccupant.getText().toString());
                occupant.setPrenom(editPrenomOccupant.getText().toString());
                occupant.setFonction(editFonctionOccupant.getText().toString());
                texteNomFonction.setText(occupant.getNom() + " " + occupant.getPrenom() + " - " + occupant.getFonction());

                // On sauvegarde localement les nouvelles informations d l'occupant
                sharedPreferences.edit().putString(PREFERENCES_NOM_OCCUPANT, occupant.getNom()).apply(); // ou .commit()
                sharedPreferences.edit().putString(PREFERENCES_PRENOM_OCCUPANT, occupant.getPrenom()).apply();
                sharedPreferences.edit().putString(PREFERENCES_FONCTION, occupant.getFonction()).apply();
            }
        });
        modificationOccupant.setNegativeButton("Annuler", new DialogInterface.OnClickListener()
        {
            @Override
            public void onClick(DialogInterface dialog, int which)
            {
            }
        });

        AlertDialog dialogueModificationOccupant = modificationOccupant.create();
        dialogueModificationOccupant.show();

        // Affiche les informations actuelles de l'occupant
        EditText nom =
                (EditText) ((AlertDialog) dialogueModificationOccupant).findViewById(R.id.nomOccupant);
        nom.setText(occupant.getNom());
        EditText prenom =
                (EditText) ((AlertDialog) dialogueModificationOccupant).findViewById(R.id.prenomOccupant);
        prenom.setText(occupant.getPrenom());
        EditText fonction =
                (EditText) ((AlertDialog) dialogueModificationOccupant).findViewById(R.id.fonctionOccupant);
        fonction.setText(occupant.getFonction());

    }

    private void modifierUrl()
    {
        Log.d(TAG, "modifierUrl()");
        modificationUrl = new AlertDialog.Builder(this);

        modificationUrl.setTitle("GROOM");
        modificationUrl.setMessage("Modifier l'agenda");
        //dialogueModificationUrl.setView(R.layout.dialogue_modification_calendrier);
        // ou :
        LayoutInflater factory = LayoutInflater.from(this);
        final View ajoutModificationUrlView = factory.inflate(R.layout.dialogue_modification_calendrier, null);
        modificationUrl.setView(ajoutModificationUrlView);

        modificationUrl.setPositiveButton("Modifier", new DialogInterface.OnClickListener()
        {
            @Override
            public void onClick(DialogInterface dialog, int which)
            {
                // Récupère les informations saisies
                EditText editEntreeUrl =
                        (EditText) ((AlertDialog) dialog).findViewById(R.id.entreeUrl);

                editEntreeUrl.getText().toString();

                nouvelUrl = editEntreeUrl.getText().toString();

                WebView myWebView = (WebView) findViewById(R.id.calendrierGroom);
                WebSettings webSettings = myWebView.getSettings();
                webSettings.setJavaScriptEnabled(true);

                String unencodedHtml = nouvelUrl;
                String encodedHtml = Base64.encodeToString(unencodedHtml.getBytes(), Base64.NO_PADDING);
                myWebView.loadData(encodedHtml, "text/html", "base64");
                if(WebViewFeature.isFeatureSupported(WebViewFeature.FORCE_DARK))
                {
                    WebSettingsCompat.setForceDark(myWebView.getSettings(), WebSettingsCompat.FORCE_DARK_ON);
                }
            }
        });
        modificationUrl.setNegativeButton("Annuler", new DialogInterface.OnClickListener()
        {
            @Override
            public void onClick(DialogInterface dialog, int which)
            {
            }
        });

        AlertDialog dialogueModificationUrl = modificationUrl.create();
        dialogueModificationUrl.show();

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu)
    {
        Log.d(TAG, "onCreateOptionsMenu()");
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item)
    {
        int id = item.getItemId();
        Log.d(TAG, "onOptionsItemSelected() " + "id = " + id);

        //if (id == R.id.action_settings)
        //{
        //    return true;
        //}

        return super.onOptionsItemSelected(item);
    }

    /**
     * @brief Gère les messages avec le thread CommunicationGroom
     */
    final private Handler handler = new Handler()
    {
        @SuppressLint("HandlerLeak")
        public void handleMessage(Message msg)
        {
            super.handleMessage(msg);

            switch (msg.what)
            {
                case CommunicationGroom.ERREUR_GROOM:
                    Log.d(TAG, "ERREUR_GROOM");
                    afficherErreur();
                    break;
                case CommunicationGroom.CONNEXION_GROOM:
                    Log.d(TAG, "CONNEXION_GROOM");
                    afficherConnexionGroom();
                    initialiserEtatGroom();
                    break;
                case CommunicationGroom.DECONNEXION_GROOM:
                    Log.d(TAG, "DECONNEXION_GROOM");
                    afficherDeconnexionGroom();
                    break;
                case CommunicationGroom.RECEPTION_GROOM:
                    String trame = msg.obj.toString();
                    Log.d(TAG, "RECEPTION_GROOM -> " + trame);

                    String champs[] = trame.split(";");
                    // Debug
                    for(int i=0;i<champs.length;++i)
                    {
                        Log.d(TAG, " champ " + i + " -> " + champs[i]);
                    }

                    int etat = Integer.parseInt(champs[CommunicationGroom.CHAMP_ETAT]);

                    switch (etat)
                    {
                        case IHMGroom.ETAT_LIBRE:
                            Log.d(TAG, " ETAT_LIBRE");
                            boutonRadioLibre.setChecked(true);
                            boutonSonnette.setEnabled(true);
                            afficherEtatLibre();
                            break;
                        case IHMGroom.ETAT_ABSENT:
                            Log.d(TAG, " ETAT_ABSENT");
                            boutonRadioAbsent.setChecked(true);
                            afficherEtatAbsent();
                            break;
                        case IHMGroom.ETAT_OCCUPE:
                            Log.d(TAG, " ETAT_OCCUPE");
                            boutonRadioOccupe.setChecked(true);
                            afficherEtatOccupe();
                            break;
                        case IHMGroom.ETAT_ENTRER:
                            Log.d(TAG, "ETAT_ENTRER");
                            afficherEtatEntrer();
                            break;
                    }

                    if(champs[CommunicationGroom.CHAMP_SONNETTE].equals("1"))
                    {
                        Log.d(TAG, " Appui sonnette !");
                        afficherCloche();
                        //afficherNotification();
                    }
                    else
                    {
                        Log.d(TAG, " Pas d'appui sonnette !");
                    }

                    if(champs[CommunicationGroom.CHAMP_PRESENCE].equals("1"))
                    {
                        Log.d(TAG, " Présence personne détectée !");
                        activerPresence();
                    }
                    else
                    {
                        Log.d(TAG, " Pas de présence personne détectée !");
                        desactiverPresence();
                    }

                    if(champs[CommunicationGroom.CHAMP_MODE_PRESENCE].equals("1"))
                    {
                        Log.d(TAG, " Détecteur_de présence activé !");
                        boutonPresence.setText("Desactiver");
                        affichageActiverModePresence();
                        //mettre l'affichage du mode presence active une fois le capteur de présence fonctionnel
                    }
                    else
                    {
                        Log.d(TAG, " Détecteur_de présence désactivé !");
                        boutonPresence.setText("Activer");
                        affichageDesactiverModePresence();
                        //mettre l'affichage du mode presence desactive une fois le capteur de présence fonctionnel
                    }

                    if(champs[CommunicationGroom.CHAMP_MODE_SONNETTE].equals("1"))
                    {
                        Log.d(TAG, " Sonnette activée !");
                        boutonSonnette.setText("Désactiver Sonnette");
                    }
                    else
                    {
                        Log.d(TAG, " Sonnette désactivée !");
                        boutonSonnette.setText("Activer Sonnette");
                    }

                    break;
            }
        }




    };

    /**
     * @brief affiche une erreur avec l'ESP 32
     */
    private void afficherErreur()
    {
        Context context = getApplicationContext();
        CharSequence text = "Erreur communication !\nVérifier l'appairage et le branchement du groom.";
        int duration = Toast.LENGTH_LONG;
        Toast toast = Toast.makeText(context, text, duration);
        toast.show();
        toast.setGravity(Gravity.LEFT,600,-600);
    }

    /**
     * @brief affiche l'état de l'ESP 32, ici il est libre
     */
    private void afficherEtatLibre()
    {
        Context context = getApplicationContext();
        CharSequence text = "Libre !";
        int duration = Toast.LENGTH_LONG;
        Toast toast = Toast.makeText(context, text, duration);
        toast.show();
        toast.setGravity(Gravity.LEFT,300,-150);
    }

    /**
     * @brief affiche l'état de l'ESP 32, ici il est occupe
     */
    private void afficherEtatOccupe()
    {
        Context context = getApplicationContext();
        CharSequence text = "Occupé !";
        int duration = Toast.LENGTH_LONG;
        Toast toast = Toast.makeText(context, text, duration);
        toast.show();
        toast.setGravity(Gravity.LEFT,300,-70);
    }

    /**
     * @brief affiche l'état de l'ESP 32, ici il est absent
     */
    private void afficherEtatAbsent()
    {
        Context context = getApplicationContext();
        CharSequence text = "Absent !";
        int duration = Toast.LENGTH_SHORT;
        Toast toast = Toast.makeText(context, text, duration);
        toast.show();
        toast.setGravity(Gravity.LEFT,300,0);
    }

    /**
     * @brief affiche l'état de l'ESP 32, ici il est entrer
     */
    private void afficherEtatEntrer()
    {
        Context context = getApplicationContext();
        CharSequence text = "Entrer !";
        int duration = Toast.LENGTH_SHORT;
        Toast toast = Toast.makeText(context, text, duration);
        toast.show();
    }

    /**
     * @brief initialiser la notification
     */
    private void initialiserSystemeNotification()
    {
        // On crée un "gestionnaire de notification"
        notificationManager = (NotificationManager)getSystemService(Context.NOTIFICATION_SERVICE);
    }

    /**
     * @brief affiche la notification dans la barre de notification
     */
    private void afficherNotification()
    {
        CharSequence name = getString(R.string.app_name);
        String description = "Gestion GROOM";
        int importance = NotificationManager.IMPORTANCE_DEFAULT;
        NotificationChannel channel = new NotificationChannel("GROOM", name, importance);
        channel.setDescription(description);
        notificationManager.createNotificationChannel(channel);

        // On définit le titre de la notification
        String titreNotification = getString(R.string.app_name);
        Log.d(TAG, "afficherNotification() titre = " + titreNotification + " - message = " + messageSonnette + " - numeroNotification = " + numeroNotification);

        // On crée la notification
        NotificationCompat.Builder notification = new NotificationCompat.Builder(this, "GROOM")
                .setSmallIcon(R.mipmap.ic_launcher)
                .setContentTitle(titreNotification)
                .setContentText(messageSonnette)
                .setPriority(NotificationCompat.PRIORITY_DEFAULT);

        PendingIntent pendingIntent = PendingIntent.getActivity(this, 0, new Intent(), PendingIntent.FLAG_UPDATE_CURRENT);
        notification.setContentIntent(pendingIntent);
        notification.setAutoCancel(true);

        // On affiche la notification
        notificationManager.notify(numeroNotification++, notification.build());
    }



    /**
     * @brief Récupère l'état affiché sur le module à la connexion
     */
    private void initialiserEtatGroom()
    {
        if(communicationGroom != null)
            communicationGroom.envoyer("$ETAT\r\n");
    }

    /**
     * @brief lorsque la présence est active, le bouton radio etat présence change en vert et informe qu'il y a une personne détecté
     */
    private void activerPresence()
    {
        boutonRadioEtatPresence.setText("Il y a quelqu'un à l'entrée !");
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP)
        {
            boutonRadioEtatPresence.setButtonTintList(ContextCompat.getColorStateList(this, R.color.vert));
        }
        LayoutInflater inflater = this.getLayoutInflater();

        layout = inflater.inflate(R.layout.toast_presence, findViewById(R.id.layout_toast_presence));
        // TextView in layout_custom_toast.
        imagePresence = (ImageView) findViewById(R.id.imagePresence);
        // Create Toast
        toast = new Toast(getApplicationContext());
        toast.setDuration(Toast.LENGTH_LONG);
        toast.setGravity(Gravity.TOP,-130,280);
        toast.setView(layout);
        toast.show();

    }

    /**
     * @brief lorsque la présence est désactive, le bouton radio etat presence change en rouge et informe qu'il y a personne de détecté
     */
    private void desactiverPresence()
    {
        boutonRadioEtatPresence.setText("Il n'y a personne à l'entrée");
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP)
        {
            boutonRadioEtatPresence.setButtonTintList(ContextCompat.getColorStateList(this, R.color.rouge));
        }
    }

    /**
     * @brief Changement fait lors de la désactivation du mode présence
     */
    private void affichageDesactiverModePresence()
    {
        //Log.d(TAG, "affichageDesactiverModePresence()");
        boutonRadioActivationPresence.setText("Désactivé");
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP)
        {
            boutonRadioActivationPresence.setButtonTintList(ContextCompat.getColorStateList(this, R.color.rouge));
        }
    }

    /**
     * @brief Changement fait lors de l'activation du mode présence
     */
    private void affichageActiverModePresence()
    {
        //Log.d(TAG, "affichageActiverModePresence()");
        boutonRadioActivationPresence.setText("Activé");
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP)
        {
            boutonRadioActivationPresence.setButtonTintList(ContextCompat.getColorStateList(this, R.color.vert));
        }
    }

    /**
     * @brief Affichage de la cloche sur l'IHM lors de l'appui du bouton sonnette sur le module
     */
    private void afficherCloche()
    {
        LayoutInflater inflater = this.getLayoutInflater();

        layout = inflater.inflate(R.layout.toast_image, findViewById(R.id.layout_image_clochette));
        // TextView in layout_custom_toast.
        imageClochette = (ImageView) findViewById(R.id.imageClochette);
        // Create Toast
        toast = new Toast(getApplicationContext());
        toast.setDuration(Toast.LENGTH_LONG);
        toast.setGravity(Gravity.RIGHT,100,-30);
        toast.setView(layout);
        toast.show();
    }

    /**
     * @brief affichage de la présence lorsqu'il n'y a personne
     */
    private void afficherPresenceDesactiveGroom()
    {
        boutonPresence.setText("Activer");
    }

    /**
     * @brief affichage de la présence lorsqu'il y a quelqu'un
     */
    private void afficherPresenceActiveGroom()
    {
        boutonPresence.setText("Désactiver");
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP)
        {
            boutonRadioActivationPresence.setButtonTintList(ContextCompat.getColorStateList(this, R.color.vert));
        }
    }

    /**
     * @brief Actions à réaliser lorsque l'application est déconnectée
     */
    private void afficherDeconnexionGroom()
    {
        Log.d(TAG, "afficherDeconnexionGroom()");

        if(CommunicationGroom.getListeGrooms().size() > 0)
        {
            choixGroom = (int)listeAppareilsBluetooth.getSelectedItemId();
            communicationGroom = new CommunicationGroom(CommunicationGroom.getListeGrooms().get((int)listeAppareilsBluetooth.getSelectedItemId()), handler);
            boutonConnexion.setEnabled(true);
            listeAppareilsBluetooth.setEnabled(true);
        }
        else
        {
            boutonConnexion.setEnabled(false);
            listeAppareilsBluetooth.setEnabled(false);
        }

        boutonConnexion.setText("Connecter");
        etatConnexion.setText("Déconnecté");
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP)
        {
            etatConnexion.setButtonTintList(ContextCompat.getColorStateList(this, R.color.rouge));
        }
        boutonRadioLibre.setEnabled(false);
        boutonRadioAbsent.setEnabled(false);
        boutonRadioOccupe.setEnabled(false);
        boutonEntrer.setEnabled(false);
        boutonSonnette.setEnabled(false);
        boutonAjoutOccupant.setEnabled(true);
        envoyerMessageOccupant.setEnabled(false);
        texteMessageOccupant.setEnabled(false);
        boutonEnvoieOccupant.setEnabled(true);
        boutonEnvoieOccupant.setEnabled(false);
        boutonPresence.setEnabled(false);
        afficherPresenceDesactiveGroom();
        boutonSonnette.setText("Activer Sonnette");
        boutonRadioAbsent.setChecked(false);
        boutonRadioOccupe.setChecked(false);
        boutonRadioLibre.setChecked(false);
    }

    /**
     * @brief Actions à réaliser lorsque l'application est connectée
     */
    private void afficherConnexionGroom()
    {
        Log.d(TAG, "afficherConnexionGroom()");

        listeAppareilsBluetooth.setEnabled(false);
        boutonConnexion.setEnabled(true);
        boutonConnexion.setText("Déconnecter");
        etatConnexion.setText("Connecté");
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP)
        {
            etatConnexion.setButtonTintList(ContextCompat.getColorStateList(this, R.color.vert));
        }
        boutonRadioLibre.setEnabled(true);
        boutonRadioAbsent.setEnabled(true);
        boutonRadioOccupe.setEnabled(true);
        boutonEntrer.setEnabled(true);
        boutonSonnette.setEnabled(false);
        boutonAjoutOccupant.setEnabled(true);
        envoyerMessageOccupant.setEnabled(true);
        texteMessageOccupant.setEnabled(true);
        boutonEnvoieOccupant.setEnabled(true);
        boutonEnvoieOccupant.setEnabled(true);
        boutonPresence.setEnabled(true);
        boutonSonnette.setText("Activer Sonnette");
    }

    /**
     * @brief Affichage du calendrier google via une WebView
     */
    private void chargerCalendrier()
    {
        WebView myWebView = (WebView) findViewById(R.id.calendrierGroom);
        WebSettings webSettings = myWebView.getSettings();
        webSettings.setJavaScriptEnabled(true);

        String unencodedHtml = "<iframe src=\"https://calendar.google.com/calendar/embed?src=robertutilisateur84%40gmail.com&ctz=Europe%2FParis\" style=\"border: 0\" width=\"760\" height=\"495\" frameborder=\"0\" scrolling=\"no\"></iframe>";;
        String encodedHtml = Base64.encodeToString(unencodedHtml.getBytes(), Base64.NO_PADDING);
        myWebView.loadData(encodedHtml, "text/html", "base64");
        if(WebViewFeature.isFeatureSupported(WebViewFeature.FORCE_DARK))
        {
            WebSettingsCompat.setForceDark(myWebView.getSettings(), WebSettingsCompat.FORCE_DARK_ON);
        }

        new CountDownTimer(300000, 1000)
        {
            public void onTick(long millisUntilFinished)
            {
            }
            public void onFinish()
            {
                String encodedHtml = Base64.encodeToString(nouvelUrl.getBytes(), Base64.NO_PADDING);
                myWebView.loadData(encodedHtml, "text/html", "base64");
                if(WebViewFeature.isFeatureSupported(WebViewFeature.FORCE_DARK))
                {
                    WebSettingsCompat.setForceDark(myWebView.getSettings(), WebSettingsCompat.FORCE_DARK_ON);
                }
                this.start();
            }
        }.start();
    }
}
