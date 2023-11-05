/**
\file wholesale.h
\author Eva Ray, Benoit Delay
\date 04.11.2023


Ce fichier contient l'implémentation de la classe Wholesale, qui permet
l'implémentation d'un grossiste et de ses fonctions de ventes et d'achats.
*/
#include "wholesale.h"
#include "factory.h"
#include "costs.h"
#include <iostream>
#include <pcosynchro/pcothread.h>

WindowInterface* Wholesale::interface = nullptr;

Wholesale::Wholesale(int uniqueId, int fund)
    : Seller(fund, uniqueId)
{
    interface->updateFund(uniqueId, fund);
    interface->consoleAppendText(uniqueId, "Wholesaler Created");

}

int Wholesale::getSellerId(){
    return sellerId;
}

void Wholesale::setSellers(std::vector<Seller*> sellers) {
    this->sellers = sellers;

    for(Seller* seller: sellers){
        interface->setLink(uniqueId, seller->getUniqueId());
    }
}

void Wholesale::buyResources() {
    auto seller = Seller::chooseRandomSeller(sellers);
    auto map = seller->getItemsForSale();
    auto item = Seller::chooseRandomItem(map);

    if (item == ItemType::Nothing) {
        /* Nothing to buy... */
        return;
    }

    int qty = rand() % 5 + 1;
    int price = qty * getCostPerUnit(item);

    interface->consoleAppendText(uniqueId, QString("I would like to buy %1 of ").arg(qty) %
                                 getItemName(item) % QString(" which would cost me %1").arg(price));
    /* TODO */

    //mutex.lock();
    //mutexBuying.lock();

    if(money < price){
       //mutex.unlock();
       //mutexBuying.unlock();
       interface->consoleAppendText(uniqueId, QString("I cant buy these items I dont have enough money"));
       return;
    }

     if(seller->trade(item, qty) == 0){
        //mutex.unlock();
        //mutexBuying.unlock();
        interface->consoleAppendText(uniqueId, QString("The seller doesn't have enough stock"));
        return;
     }

     mutex.lock();
     stocks[item] += qty;
     money -= price;
     mutex.unlock();

     //mutexBuying.unlock();
     interface->consoleAppendText(uniqueId, QString("I cant buy these items I dont have enough money"));
}

void Wholesale::run() {

    if (sellers.empty()) {
        std::cerr << "You have to give factories and mines to a wholeseler before launching is routine" << std::endl;
        return;
    }

    interface->consoleAppendText(uniqueId, "[START] Wholesaler routine");
    while (!PcoThread::thisThread()->stopRequested() /* TODO terminaison*/) {
        buyResources();
        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);
        //Temps de pause pour espacer les demandes de ressources
        PcoThread::usleep((rand() % 10 + 1) * 100000);
    }
    interface->consoleAppendText(uniqueId, "[STOP] Wholesaler routine");


}

std::map<ItemType, int> Wholesale::getItemsForSale() {
    return stocks;
}

int Wholesale::trade(ItemType it, int qty) {
    // TODO
    mutex.lock();
    //mutexTrading.lock();
    if(getItemsForSale()[it] < qty or
        qty <= 0 or
        getItemsForSale().find(it) == getItemsForSale().end()){
        mutex.unlock();
        //mutexTrading.unlock();
        return 0;
    }
    int price = getCostPerUnit(it) * qty;
    money += price;
    stocks[it] -= qty;
    mutex.unlock();
    //mutexTrading.unlock();
    return price;
}

void Wholesale::setInterface(WindowInterface *windowInterface) {
    interface = windowInterface;
}
