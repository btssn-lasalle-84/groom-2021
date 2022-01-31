#include "Calendrier.h"
#include <QDebug>

/**
 * @file Calendrier.cpp
 * @brief Déclaration de la classe Calendrier
 * @author Thierry VAIRA
 * @version 1.1
 * $LastChangedRevision$
 * $LastChangedDate$
 */

Calendrier::Calendrier(QObject *parent) : QObject(parent), hostURL(""), nom(""), contenuCalendrier(nullptr), networkReply(nullptr), requeteTimeoutMS(2000)
{
    qDebug() << __FUNCTION__;
    synchronisationTimer.setSingleShot(true);
    synchronisationTimer.setInterval(1000);
    requeteTimeoutTimer.setSingleShot(true);

    connect(&synchronisationTimer, SIGNAL(timeout()), this, SLOT(recharger()));
}

Calendrier::~Calendrier()
{
    synchronisationTimer.stop();
    qDebug() << __FUNCTION__;
}

bool Calendrier::setHostURL(const QString hostURL)
{
    QUrl url(hostURL);
    return setHostURL(url);
}

bool Calendrier::setHostURL(const QUrl hostURL)
{
    bool ret = false;

    if (hostURL.isValid() != false)
    {
      this->hostURL = hostURL;
      ret = true;
    }

    return ret;
}

QString Calendrier::getHostURL(void) const
{
    return this->hostURL.toString();
}

QString Calendrier::getNom(void) const
{
    return this->nom;
}

void Calendrier::setNom(QString nom)
{
    this->nom = nom;
}

int Calendrier::getRequeteTimeoutMS(void) const
{
    return this->requeteTimeoutMS;
}

void Calendrier::setRequeteTimeoutMS(const int requeteTimeoutMS)
{
    if (requeteTimeoutMS > 0)
    {
        this->requeteTimeoutMS = requeteTimeoutMS;
    }
}

QList<Evenement> Calendrier::getEvenements(QDate date) const
{
    qDebug() << __FUNCTION__ << date;
    QList<Evenement> listeEvenements;
    bool ok;
    foreach(Evenement evt, evenements)
    {
        QString dtstart = evt.getAttribut("dtstart");
        QDateTime dtStart = Evenement::convertirHorodatage(dtstart, ok);
        QString dtend = evt.getAttribut("dtend");
        QDateTime dtEnd = Evenement::convertirHorodatage(dtend, ok);
        if (dtStart.isValid() && dtEnd.isValid())
        {
            if(date >= dtStart.date() && date <= dtEnd.date())
            {
                listeEvenements.append(evt);
            }
        }
    }
    qDebug() << __FUNCTION__ << date << "nb evenements : " << listeEvenements.size();

    return listeEvenements;
}

void Calendrier::charger()
{
    qDebug() << __FUNCTION__ << nom << hostURL << "local ?" << hostURL.isLocalFile();
    emit effacement();
    if(hostURL.isLocalFile())
    {
        QString filename = hostURL.toString().remove("file:///");
        qDebug() << __FUNCTION__ << nom << hostURL << "relative ?" << QUrl(filename).isRelative();
        if (QUrl(filename).isRelative())
        {
            filename = filename.prepend("/");
        }
        filename = QUrl(filename).toString(QUrl::NormalizePathSegments);
        qDebug() << __FUNCTION__ << nom << filename;

        QFile file(filename);

        if (file.exists())
        {
          if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
          {
              qDebug() << __FUNCTION__ << nom << filename << file.errorString();
              emit erreur("Erreur : impossible de lire le fichier !");
              return;
          }
        }
        else
        {
            qDebug() << __FUNCTION__ << nom << filename << "Erreur : fichier introuvable !";
            emit erreur("Erreur : fichier introuvable !");
            return;
        }

        if(contenuCalendrier)
        {
            delete contenuCalendrier;
        }
        contenuCalendrier = new QTextStream(&file);

        evenements.clear();
        bool trouve = false;

        do
        {
            QString line = contenuCalendrier->readLine();
            //qDebug() << __FUNCTION__ << nom << line;
            if(!line.isNull())
            {
                if (line.startsWith("X-WR-CALNAME"))
                {
                    qDebug() << __FUNCTION__ << nom << line;
                    QString nomCalendrier = line.trimmed();
                    nomCalendrier.remove(";LANGUAGE=fr", Qt::CaseInsensitive);
                    nomCalendrier.remove("X-WR-CALNAME:", Qt::CaseInsensitive);
                    qDebug() << __FUNCTION__ << nom << nomCalendrier;
                    this->setNom(nomCalendrier);
                    emit message("Calendrier : " + nomCalendrier);
                    trouve = true;
                }
            }
        }
        while (!contenuCalendrier->atEnd() && !trouve);
        contenuCalendrier->seek(0);

        traiterCalendrier();
    }
    else
    {
        telecharger();
    }
}

void Calendrier::telecharger()
{
    qDebug() << __FUNCTION__ << nom;

    if (networkReply != nullptr)
    {
        networkReply->abort();
        networkReply->disconnect();
    }

    QNetworkRequest requete;
    requete.setUrl(hostURL);
    requete.setRawHeader("User-Agent", "CalendarClient_ICS");
    requete.setRawHeader("Depth", "0");
    requete.setRawHeader("Prefer", "return-minimal");
    requete.setRawHeader("Content-Type", "text/xml; charset=utf-8");
    requete.setRawHeader("Content-Length", "0");

    QSslConfiguration configuration = requete.sslConfiguration();
    configuration.setPeerVerifyMode(QSslSocket::VerifyNone);
    requete.setSslConfiguration(configuration);

    networkReply = networkManager.get(requete);

    if (networkReply != nullptr)
    {
      connect(networkReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleHTTPError()));
      connect(networkReply, SIGNAL(finished()), this, SLOT(traiterRequete()));

      requeteTimeoutTimer.start(requeteTimeoutMS);
    }
    else
    {
        qDebug() << __FUNCTION__ << nom << "Erreur : réponse invalide !";
        emit erreur("Erreur : réponse invalide !");
    }
}

void Calendrier::recharger()
{
    this->charger();
}

void Calendrier::traiterRequete()
{
    requeteTimeoutTimer.stop();

    if (etatCalendrier == E_STATE_ERROR)
    {
        qDebug() << __FUNCTION__ << nom << "Erreur !";
    }

    qDebug() << __FUNCTION__ << nom << "Requête terminée";

    if (networkReply != nullptr)
    {
        QBuffer buffer(this);
        buffer.setData(networkReply->readAll());

        buffer.open(QIODevice::ReadOnly);
        bool trouve = false;
        do
        {
            QString ligne = buffer.readLine();
            if (!ligne.isNull())
            {
                if (ligne.startsWith("X-WR-CALNAME"))
                {
                    qDebug() << __FUNCTION__ << nom << ligne;
                    QString nomCalendrier = ligne.trimmed();
                    nomCalendrier.remove(";LANGUAGE=fr", Qt::CaseInsensitive);
                    nomCalendrier.remove("X-WR-CALNAME:", Qt::CaseInsensitive);
                    qDebug() << __FUNCTION__ << nom << nomCalendrier;
                    this->setNom(nomCalendrier);
                    emit message("Calendrier : " + nomCalendrier);
                    trouve = true;
                }
            }
        }
        while (!buffer.atEnd() && !trouve);
        buffer.seek(0);
        buffer.close();

        if (contenuCalendrier)
        {
            delete contenuCalendrier;
        }
        contenuCalendrier = new QTextStream(buffer.data());

        evenements.clear();

        traiterCalendrier();

        synchronisationTimer.start();
    }
    else
    {
        qDebug() << __FUNCTION__ << nom << "Erreur : réponse invalide !";
        emit erreur("Erreur : réponse invalide !");
    }
}

void Calendrier::traiterCalendrier()
{
    QString ligne = contenuCalendrier->readLine();

    while(ligne.isNull() == false)
    {
        if(ligne.contains("BEGIN:VEVENT") != false)
        {
            traiterEvenement();
        }
        ligne = contenuCalendrier->readLine();
    }
    emit message(QString::fromUtf8("Nb évènement(s) : ") + QString::number(evenements.size()));
    qDebug() << __FUNCTION__ << nom << "nb evenements : " << evenements.size();
}

void Calendrier::traiterEvenement()
{
    Evenement evenement(this);
    QString ligne;
    QDateTime utcTime;
    bool ok;

    qDebug() << __FUNCTION__ << nom;
    evenement.setAttribut("utilisateur du calendrier", nom);

    while (!(ligne = contenuCalendrier->readLine()).contains(QByteArray("END:VEVENT")))
    {
        ligne.remove(";LANGUAGE=fr", Qt::CaseInsensitive);
        const int delimiteur = ligne.indexOf(QLatin1Char(':'));
        const QString key = ligne.mid(0, delimiteur);
        QString value = (ligne.mid(delimiteur + 1, -1).replace("\\n", "\n")); //.toLatin1();
        QString testEncodingString = asciiToUtf8(value);
        if (testEncodingString.contains("�") == false)
        {
          value = testEncodingString;
        }

        if (key.startsWith(QLatin1String("DTSTART")))
        {
            utcTime = Evenement::convertirHorodatage(value, ok);
            if (utcTime.isValid())
                evenement.setAttribut("dtstart", value);
        }
        else if (key.startsWith(QLatin1String("DTEND")))
        {
            utcTime = Evenement::convertirHorodatage(value, ok);
            if (utcTime.isValid())
                evenement.setAttribut("dtend", value);
        }
        /*else if (key == QLatin1String("RRULE"))
        {
            //event.setRRULE(value);
        }
        else if (key == QLatin1String("EXDATE"))
        {
            //event.setExdates(value);
        }*/
        else if (key == QLatin1String("SUMMARY"))
        {
            evenement.setAttribut("summary", value);
        }
        else if (key == QLatin1String("LOCATION"))
        {
            evenement.setAttribut("location", value);
        }
        else if (key == QLatin1String("UID"))
        {
            evenement.setAttribut("uid", value);
        }
        else if (key == QLatin1String("CATEGORIES"))
        {
            evenement.setAttribut("categories", value);
        }
        else if (key == QLatin1String("DESCRIPTION"))
        {
            evenement.setAttribut("description", value);
        }
    }

    evenements.append(evenement);
    emit nouvelEvenement(QString::fromUtf8("Évènement : ") + evenement.toString());
    //qDebug() << __FUNCTION__ << nom << evenement;
}

void Calendrier::handleHTTPError()
{
    etatCalendrier = E_STATE_ERROR;
    //emit syncStateChanged(etatCalendrier);
    requeteTimeoutTimer.stop();
    if (networkReply != nullptr)
    {
        qDebug() << __FUNCTION__ << nom << "Erreur : requête HTTP !" << networkReply->errorString();
        emit erreur(networkReply->errorString());
    }
    else
    {
        qDebug() << __FUNCTION__ << nom << "Erreur : réponse invalide !";
        emit erreur("Erreur : réponse invalide !");
    }
}

QString Calendrier::encoderBase64(QString string)
{
  QByteArray ba;
  ba.append(string);
  return ba.toBase64();
}

QString Calendrier::asciiToUtf8(QString str)
{
 return QString::fromUtf8(str.toLocal8Bit().data());
}
