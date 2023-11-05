/**
\file factory.cpp
\author Eva Ray, Benoit Delay
\date 04.11.2023


Ce fichier contient l'implémentation de la classe Factory, qui permet
l'implémentation d'une usine et de ses fonctions de ventes et d'achats.
*/
#include "factory.h"
#include "extractor.h"
#include "costs.h"
#include "wholesale.h"
#include <pcosynchro/pcothread.h>
#include <iostream>
#include <pcosynchro/pcologger.h>

WindowInterface* Factory::interface = nullptr;


Factory::Factory(int uniqueId, int fund, ItemType builtItem, std::vector<ItemType> resourcesNeeded)
    : Seller(fund, uniqueId), resourcesNeeded(resourcesNeeded), itemBuilt(builtItem), nbBuild(0)
{
    assert(builtItem == ItemType::Chip ||
           builtItem == ItemType::Plastic ||
           builtItem == ItemType::Robot);

    interface->updateFund(uniqueId, fund);
    interface->consoleAppendText(uniqueId, "Factory created");
}

void Factory::setWholesalers(std::vector<Wholesale *> wholesalers) {
    Factory::wholesalers = wholesalers;

    for(Seller* seller: wholesalers){
        interface->setLink(uniqueId, seller->getUniqueId());
    }
}

ItemType Factory::getItemBuilt() {
    return itemBuilt;
}

int Factory::getMaterialCost() {
    return getCostPerUnit(itemBuilt);
}

bool Factory::verifyResources() {
    for (auto item : resourcesNeeded) {
        if (stocks[item] == 0) {
            return false;
        }
    }

    return true;
}

void Factory::buildItem() {

    // TODO
    int builderCost = getEmployeeSalary(getEmployeeThatProduces(itemBuilt));

    mutex.lock();
    //mutexBuying.lock();
    if(money < builderCost){
        interface->consoleAppendText(uniqueId,"I don't have enough money to pay the employee");
        mutex.unlock();
        //mutexBuying.unlock();
        return;
    }

    money -= builderCost;
    for(ItemType ressource : resourcesNeeded){
        stocks[ressource]--;
    }

    //Temps simulant l'assemblage d'un objet.
    PcoThread::usleep((rand() % 100) * 100000);

    // TODO
    stocks[itemBuilt]++;
    mutex.unlock();
    //mutexBuying.unlock();
    interface->consoleAppendText(uniqueId, "I built a new item");

}

void Factory::orderResources() {

    // TODO - Itérer sur les resourcesNeeded et les wholesalers disponibles

    for(ItemType resource : resourcesNeeded){
        // On achète en priorité la ressource qu'on n'a plus en stock
        if(stocks[resource]){
            continue;
        }
        int price = getCostPerUnit(resource);

        for(auto wholesale : wholesalers){

            interface->consoleAppendText(uniqueId, QString("I would like to buy 1 " + getItemName(resource)));

            mutex.lock();
            //mutexBuying.lock();
            if(money < price){
                interface->consoleAppendText(uniqueId, QString("I dont have enough money to buy this item"));
                mutex.unlock();
                //mutexBuying.unlock();
                continue;
            }

            if(wholesale->trade(resource,1) == 0){
                mutex.unlock();
                //mutexBuying.unlock();
                interface->consoleAppendText(uniqueId, QString("The seller doesn't have enough stock"));
                continue;
            }

            money -= price;
            stocks[resource]++;

            // On a réussi à acheter la ressource
            mutex.unlock();
            //mutexBuying.unlock();
            interface->consoleAppendText(uniqueId, QString("Success"));
            break;
        }
    }    

    //Temps de pause pour éviter trop de demande
    PcoThread::usleep(10 * 100000);

}

void Factory::run() {

    if (wholesalers.empty()) {
        std::cerr << "You have to give to factories wholesalers to sales their resources" << std::endl;
        return;
    }
    interface->consoleAppendText(uniqueId, "[START] Factory routine");

    while (!PcoThread::thisThread()->stopRequested() /* TODO terminaison*/) {
        interface->consoleAppendText(uniqueId, "Run");

        if (verifyResources()) {
            buildItem();
        } else {
            orderResources();
        }
        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);
    }
    interface->consoleAppendText(uniqueId, "[STOP] Factory routine");
}

std::map<ItemType, int> Factory::getItemsForSale() {
    return std::map<ItemType, int>({{itemBuilt, stocks[itemBuilt]}});
}

int Factory::trade(ItemType it, int qty) {
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

int Factory::getAmountPaidToWorkers() {
    return Factory::nbBuild * getEmployeeSalary(getEmployeeThatProduces(itemBuilt));
}

void Factory::setInterface(WindowInterface *windowInterface) {
    interface = windowInterface;
}

PlasticFactory::PlasticFactory(int uniqueId, int fund) :
    Factory::Factory(uniqueId, fund, ItemType::Plastic, {ItemType::Petrol}) {}

ChipFactory::ChipFactory(int uniqueId, int fund) :
    Factory::Factory(uniqueId, fund, ItemType::Chip, {ItemType::Sand, ItemType::Copper}) {}

RobotFactory::RobotFactory(int uniqueId, int fund) :
    Factory::Factory(uniqueId, fund, ItemType::Robot, {ItemType::Chip, ItemType::Plastic}) {}
