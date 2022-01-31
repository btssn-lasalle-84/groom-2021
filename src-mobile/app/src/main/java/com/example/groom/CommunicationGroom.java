package com.example.groom;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Set;
import java.util.UUID;

/**
 * @file CommunicationGroom.java
 * @brief Déclaration de la classe CommunicationGroom
 * @author Thierry Vaira
 * $LastChangedRevision: 145 $
 * $LastChangedDate: 2021-06-13 10:30:15 +0200 (Sun, 13 Jun 2021) $
 */

public class CommunicationGroom
{
    // Constantes
    private static final String TAG = "_CommunicationGroom";//!< TAG pour les logs
    private static final String PERIPHERIQUE_GROOM = "groom-";//!< préfixe des périphériques bluetooth
    public static final int ERREUR_GROOM = -1;//!< erreur avec groom
    public static final int CONNEXION_GROOM = 0;//!< connexion du groom
    public static final int RECEPTION_GROOM = 1;//!< réception du groom
    public static final int DECONNEXION_GROOM = 2;//!< déconnexion du groom

    public static final int CHAMP_ETAT = 1;//!< le champ numéro 1 concernant l'état
    public static final int CHAMP_SONNETTE = 2;//!< le champ numéro 2 concernant la sonnette
    public static final int CHAMP_PRESENCE = 3;//!< le champ numéro 3 concernant la présence
    public static final int CHAMP_MODE_SONNETTE = 4;//!< le champ numéro 4 concernant le mode de la sonnette
    public static final int CHAMP_MODE_PRESENCE = 5;//!< le champ numéro 5 concernant le mode de la présence

    // Attributs
    private String nomPeripherique;//!< nom du périphérique
    private String adressePeripherique;//!< adresse du périphériques
    private Handler handler = null;//!< Constructeur handler de type Handle définie en null
    private BluetoothDevice peripheriqueGroom = null;//!< le périphérique bluetooth du groom
    private BluetoothSocket socket = null;//!< socket bluetooth
    private InputStream receiveStream = null;//!< données reçues
    private OutputStream sendStream = null;//!< données envoyés
    private Reception reception = null;//!< Constructeur reception de type Reception définie en null
    private ArrayList<String> listePeripheriques = null;//!< liste des périphériques

    /**
     * @brief Constructeur
     */
    public CommunicationGroom(BluetoothDevice groom, Handler handler)
    {
        Log.d(TAG, "CommunicationGroom() " + groom.getName());
        this.handler = handler; // permet de communiquer par message avec l'activité
        this.peripheriqueGroom = groom;
        this.nomPeripherique = groom.getName();
        this.adressePeripherique = groom.getAddress();
        this.handler = handler;

        // Création de la socket pour communiquer
        try
        {
            if(peripheriqueGroom != null)
            {
                Log.d(TAG, "Création nouvelle socket");
                socket = peripheriqueGroom.createRfcommSocketToServiceRecord(UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"));
                receiveStream = socket.getInputStream();
                sendStream = socket.getOutputStream();
            }
        }
        catch (IOException e)
        {
            Log.d(TAG, "Erreur création socket");
            e.printStackTrace();
            socket = null;
            // signale à l'activité l'erreur
            Message msg = Message.obtain();
            msg.what = ERREUR_GROOM;
            handler.sendMessage(msg);
        }

        // Création du thread Reception
        if(socket != null)
        {
            reception = new Reception(handler);
        }
    }

    /**
     * @brief Récupère le nom du périphérique
     */
    public String getNomPeripherique()
    {
        return nomPeripherique;
    }

    /**
     * @brief Récupère l'adresse du périphérique
     */
    public String getAdressePeripherique()
    {
        return adressePeripherique;
    }

    /**
     * @brief Retourne l'état de connexion de la socket
     */
    public boolean estConnecte()
    {
        if(socket == null)
            return false;
        return socket.isConnected();
    }

    /**
     * @brief Récupère la liste des grooms appairés
     */
    public static ArrayList<BluetoothDevice> getListeGrooms()
    {
        //Log.d(TAG, "getListeGrooms()");
        ArrayList<BluetoothDevice> grooms = new ArrayList<BluetoothDevice>();;
        Set<BluetoothDevice> devices;

        BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        devices = bluetoothAdapter.getBondedDevices();
        for (BluetoothDevice device : devices)
        {
            // un groom ?
            if(device.getName().contains(PERIPHERIQUE_GROOM))
            {
                grooms.add(device);
                //Log.d(TAG, " -> " + device.getName() + " [" + device.getAddress() + "]");
            }
        }

        if(grooms.size() < 1)
            Log.d(TAG, "getListeGrooms() aucun groom détecté !");

        return grooms;
    }

    /**
     * @brief Permet d'envoyer une trame au module bluetooth du groom
     */
    public void envoyer(String trame)
    {
        // Vérifications
        if(peripheriqueGroom == null || socket == null)
            return;

        new Thread()
        {
            @Override public void run()
            {
                try
                {
                    if(socket.isConnected())
                    {
                        sendStream.write(trame.getBytes());
                        sendStream.flush();
                    }

                    Log.d(TAG, "Envoyer trame : " + trame);
                }
                catch (IOException e)
                {
                    Log.d(TAG, "Erreur écriture socket");
                    e.printStackTrace();
                    // signale à l'activité l'erreur
                    Message msg = Message.obtain();
                    msg.what = ERREUR_GROOM;
                    handler.sendMessage(msg);
                }
            }
        }.start();
    }

    /**
     * @brief Permet la connexion au module bluetooth
     */
    public void connecter()
    {
        // Vérifications
        if(peripheriqueGroom == null || socket == null)
            return;

        new Thread()
        {
            @Override public void run()
            {
                // Connexion de la socket
                try
                {
                    socket.connect();

                    // signale à l'activité la connexion
                    Message msg = Message.obtain();
                    msg.what = CONNEXION_GROOM;
                    handler.sendMessage(msg);

                    Log.d(TAG, "Connexion socket");
                }
                catch (IOException e)
                {
                    Log.d(TAG, "Erreur connexion socket");
                    e.printStackTrace();
                    // signale à l'activité l'erreur
                    Message msg = Message.obtain();
                    msg.what = ERREUR_GROOM;
                    handler.sendMessage(msg);
                }

                // Démarrage de la reception
                if(socket.isConnected())
                {
                    reception.start();
                }
            }
        }.start();
    }

    /**
     * @brief Permet la déconnexion du module bluetooth
     */
    public void deconnecter()
    {
        // Vérifications
        if(peripheriqueGroom == null || socket == null)
            return;

        // Exécute la déconnexion dans un Thread pour éviter un blocage
        new Thread()
        {
            @Override public void run()
            {
                // Déconnexion de la socket
                try
                {
                    socket.close();

                    // signale à l'activité la déconnexion
                    Message msg = Message.obtain();
                    msg.what = DECONNEXION_GROOM;
                    handler.sendMessage(msg);

                    Log.d(TAG, "Déconnexion socket");
                }
                catch (IOException e)
                {
                    Log.d(TAG, "Erreur déconnexion socket");
                    e.printStackTrace();
                    // signale à l'activité l'erreur
                    Message msg = Message.obtain();
                    msg.what = ERREUR_GROOM;
                    handler.sendMessage(msg);
                }

                // Arrêt de la reception
                if(!socket.isConnected())
                {
                    reception.arreter();
                }
            }
        }.start();
    }

    /**
     * @brief Permet la réception de trames du module bluetooth
     */
    private class Reception extends Thread
    {
        Handler handlerUI;
        private boolean fini;

        Reception(Handler h)
        {
            handlerUI = h;
            fini = false;
        }

        @Override
        public void run()
        {
            Log.d(TAG, "Démarre la réception");
            BufferedReader reception = new BufferedReader(new InputStreamReader(receiveStream));

            // Attendre la reception
            while (!fini)
            {
                try
                {
                    String trame = "";
                    if (reception.ready())
                    {
                        trame = reception.readLine(); // ATTENTION : il attend et enlève un \r\n
                    }
                    if (trame.length() > 0)
                    {
                        Log.d(TAG, "Réception trame : " + trame);
                        Message msg = Message.obtain();
                        msg.what = RECEPTION_GROOM;
                        msg.obj = trame;
                        handlerUI.sendMessage(msg);
                    }
                }
                catch (IOException e)
                {
                    Log.d(TAG, "Erreur lecture socket");
                    e.printStackTrace();
                    // signale à l'activité l'erreur
                    Message msg = Message.obtain();
                    msg.what = ERREUR_GROOM;
                    handler.sendMessage(msg);
                }
                try
                {
                    Thread.sleep(250);
                }
                catch (InterruptedException e)
                {
                    e.printStackTrace();
                }
            }
            //Log.d(TAG, "Réception arrêtée");
        }

        public void arreter()
        {
            if (!fini)
            {
                fini = true;
            }
        }
    }
}
