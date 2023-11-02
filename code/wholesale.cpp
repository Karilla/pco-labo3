#include "wholesale.h"
#include "factory.h"
#include "costs.h"
#include <iostream>
#include <pcosynchro/pcothread.h>
#include <pcosynchro/pcologger.h>

WindowInterface* Wholesale::interface = nullptr;

Wholesale::Wholesale(int uniqueId, int fund)
    : Seller(fund, uniqueId)
{
    interface->updateFund(uniqueId, fund);
    interface->consoleAppendText(uniqueId, "Wholesaler Created");

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
    logger() << "Lock Id " << this->uniqueId  << std::endl;
    mutex.lock();
    if(this->getFund() < price){
       interface->consoleAppendText(uniqueId, QString("I cant buy these item i dont have enough money"));
       logger() << "Unlock Id " << this->uniqueId  << std::endl;
       mutex.unlock();
       return;
    }
     if(seller->trade(item, qty) != 0){
        this->stocks[item] += qty;
         this->money -= price;
     }
     mutex.unlock();
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
  interface->consoleAppendText(uniqueId, "Une factory veut acheter des trucs");
    // TODO
  mutex.lock();
    if(this->getItemsForSale()[it] < qty or
       qty < 0 or
       this->getItemsForSale().find(it) == this->getItemsForSale().end()){
        mutex.unlock();
        return 0;
    }
    int price = getCostPerUnit(it) * qty;
    this->money += price;
    this->getItemsForSale()[it] -= qty;
    mutex.unlock();
    return price;
}

void Wholesale::setInterface(WindowInterface *windowInterface) {
    interface = windowInterface;
}
