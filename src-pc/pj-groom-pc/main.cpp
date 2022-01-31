#include "IHMGroom.h"
#include <QApplication>

/**
 * @file main.cpp
 * @brief Programme principal GROOM-PC
 * @details Crée et affiche la fenêtre principale de l'application GROOM-PC
 * @author ymota <motayuri13@gmail.com>
 * @version 1.0
 * $LastChangedRevision: 81 $
 * $LastChangedDate: 2021-04-28 11:52:02 +0200 (mer. 28 avril 2021) $
 *
 * @param argc
 * @param argv[]
 * @return int
 *
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    IHMGroom w;
    w.show();
    return a.exec();
}
