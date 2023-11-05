/**
\file extractor.h
\author Eva Ray, Benoit Delay
\date 04.11.2023


Ce fichier contient l'implémentation de la classe Extractor, qui permet
l'implémentation d'une mine et de ses fonctions de ventes.
*/
#include "extractor.h"
#include "costs.h"
#include <pcosynchro/pcothread.h>
#include <map>


WindowInterface* Extractor::interface = nullptr;

Extractor::Extractor(int uniqueId, int fund, ItemType resourceExtracted)
    : Seller(fund, uniqueId), resourceExtracted(resourceExtracted), nbExtracted(0)
{
    assert(resourceExtracted == ItemType::Copper ||
           resourceExtracted == ItemType::Sand ||
           resourceExtracted == ItemType::Petrol);
    interface->consoleAppendText(uniqueId, QString("Mine Created"));
    interface->updateFund(uniqueId, fund);
}

std::map<ItemType, int> Extractor::getItemsForSale() {
    return stocks;
}

int Extractor::trade(ItemType it, int qty) {
    // TODO
    mutex.lock();
    if(getItemsForSale()[it] < qty or
        qty <= 0 or
        getItemsForSale().find(it) == getItemsForSale().end()){
        mutex.unlock();
        return 0;
    }
    int price = getMaterialCost() * qty;
    money += price;
    stocks[it] -= qty;
    mutex.unlock();
    return price;
}

void Extractor::run() {
    interface->consoleAppendText(uniqueId, "[START] Mine routine");

    while (!PcoThread::thisThread()->stopRequested() /* TODO terminaison*/) {
        /* TODO concurrence */

        int minerCost = getEmployeeSalary(getEmployeeThatProduces(resourceExtracted));
        mutex.lock();
        if (money < minerCost) {
            /* Pas assez d'argent */
            /* Attend des jours meilleurs */
            mutex.unlock();
            PcoThread::usleep(1000U);     
            continue;
        }

        /* On peut payer un mineur */
        money -= minerCost;
        /* Temps aléatoire borné qui simule le mineur qui mine */
        PcoThread::usleep((rand() % 100 + 1) * 10000);
        /* Statistiques */
        nbExtracted++;
        /* Incrément des stocks */
        stocks[resourceExtracted] += 1;

        /* Message dans l'interface graphique */
        interface->consoleAppendText(uniqueId, QString("1 ") % getItemName(resourceExtracted) %
                                     " has been mined");
        /* Update de l'interface graphique */
        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);
        mutex.unlock();
    }
    interface->consoleAppendText(uniqueId, "[STOP] Mine routine");
}

int Extractor::getMaterialCost() {
    return getCostPerUnit(resourceExtracted);
}

ItemType Extractor::getResourceMined() {
    return resourceExtracted;
}

int Extractor::getAmountPaidToMiners() {
    return nbExtracted * getEmployeeSalary(getEmployeeThatProduces(resourceExtracted));
}

void Extractor::setInterface(WindowInterface *windowInterface) {
    interface = windowInterface;
}

SandExtractor::SandExtractor(int uniqueId, int fund): Extractor::Extractor(uniqueId, fund, ItemType::Sand) {}

CopperExtractor::CopperExtractor(int uniqueId, int fund): Extractor::Extractor(uniqueId, fund, ItemType::Copper) {}

PetrolExtractor::PetrolExtractor(int uniqueId, int fund): Extractor::Extractor(uniqueId, fund, ItemType::Petrol) {}
