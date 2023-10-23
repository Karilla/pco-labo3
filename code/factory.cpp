#include "factory.h"
#include "extractor.h"
#include "costs.h"
#include "wholesale.h"
#include <pcosynchro/pcothread.h>
#include <iostream>

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
    if(!verifyResources()){
        return;
    }

    EmployeeType employee = getEmployeeThatProduces(this->itemBuilt);
    int salary = getEmployeeSalary(employee);

    if(money < salary){
        interface->consoleAppendText(uniqueId,"Factory don't have enough money to pay the employee");
        return;
    }

    this->money -= salary;
    for(auto ressource : this->resourcesNeeded){
        this->stocks[ressource]--;
    }

    //Temps simulant l'assemblage d'un objet.
    PcoThread::usleep((rand() % 100) * 100000);

    // TODO
    this->stocks[this->itemBuilt]++;

    interface->consoleAppendText(uniqueId, "Factory have build a new object");
}

void Factory::orderResources() {

    // TODO - Itérer sur les resourcesNeeded et les wholesalers disponibles
    for(auto resource : this->resourcesNeeded){
        for(auto seller : this->wholesalers){
            if(seller->getItemsForSale()[resource] != 0){
                interface->consoleAppendText(uniqueId,"I would like to buy 5 " + getItemName(resource) );
                this ->money -= seller->trade(resource,5);
                this->stocks[resource] += 5;
            }
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
    if(this->getItemsForSale()[it] < qty or
       qty < 0 or
       this->getItemsForSale().find(it) == this->getItemsForSale().end()){
        return 0;
    }
    int price = getCostPerUnit(it) * qty;
    this->money += price;
    this->getItemsForSale()[it] -= qty;
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
