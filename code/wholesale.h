/**
\file wholesale.h
\author Eva Ray, Benoit Delay
\date 04.11.2023


Ce fichier contient la définition de la classe Wholesale, qui permet
l'implémentation d'un grossiste et de ses fonctions de ventes et d'achats.
*/
#ifndef WHOLESALE_H
#define WHOLESALE_H
#include "seller.h"
#include <vector>
#include "windowinterface.h"
#include <pcosynchro/pcomutex.h>

/**
 * @brief La classe permet l'implémentation d'un grossiste et de ces fonctions
 *        de ventes et d'achats.
 */
class Wholesale : public Seller
{
private:
    // Vecteur de vendeurs (mines, usines) auxquels le grossiste peut acheter des ressources
    std::vector<Seller*> sellers;

    static WindowInterface* interface;

    // Mutex pour protéger les ressources partagées
    PcoMutex mutex;
    PcoMutex mutexBuying;
    PcoMutex mutexTrading;

    // UniqueId of the seller the wholesale is trying to buy from
    int sellerId;

    /**
     * @brief Fonction permettant d'acheter des ressources à des usines ou des mines
     */
    void buyResources();
public:


    /**
     * @brief Constructeur de grossiste
     * @param Fond du grossiste à sa création
     */
    Wholesale(int uniqueId, int fund);

    int getSellerId();

    /**
     * @brief Routine d'exécution du grossiste
     */
    void run();

    std::map<ItemType, int> getItemsForSale() override;
    int trade(ItemType it, int qty) override;



    /**
     * @brief Fonction permettant de lier des vendeurs
     * @param Vecteurs
     */
    void setSellers(std::vector<Seller*> sellers);

    static void setInterface(WindowInterface* windowInterface);
};

#endif // WHOLESALE_H
