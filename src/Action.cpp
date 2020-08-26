#include "../include/Action.h"
#include "../include/Restaurant.h"

BaseAction::BaseAction(): errorMsg(), status(PENDING) {}

ActionStatus BaseAction::getStatus() const {
    return status;
}

void BaseAction::error(std::string errorMsg) {
    status = ERROR;
    this->errorMsg = std::move(errorMsg);
}

void BaseAction::complete() {
    status = COMPLETED;
}

std::string BaseAction::getErrorMsg() const {
    return errorMsg;
}

BaseAction::~BaseAction() {
    errorMsg.clear();
}

OpenTable::OpenTable(int id, std::vector<Customer *> &customersList) : tableId(id), customers(customersList) {}

void OpenTable::act(Restaurant &restaurant) {
    if (tableId > restaurant.getNumOfTables()) {
        BaseAction::error("Error: Table does not exist or is already open");
        std::cout<<getErrorMsg()<< std::endl;
    }
    else {
        Table *t = restaurant.getTable(tableId);
        if (t->isOpen()) {
            BaseAction::error("Error: Table does not exist or is already open");
            std::cout << getErrorMsg()<< std::endl;
        }
        else {
            if (customers.size() > (unsigned)t->getCapacity())
                BaseAction::error("number of customers is too big for the table");
            else {
                for (auto customer : customers) {
                    if (customer->getType() == "veg")
                        t->addCustomer(new VegetarianCustomer(customer->getName(), customer->getId()));
                    if (customer->getType() == "spc")
                        t->addCustomer(new SpicyCustomer(customer->getName(), customer->getId()));
                    if (customer->getType() == "alc")
                        t->addCustomer(new AlchoholicCustomer(customer->getName(), customer->getId()));
                    if (customer->getType() == "chp")
                        t->addCustomer(new CheapCustomer(customer->getName(), customer->getId()));
                }

                t->openTable();
                complete();
            }
        }
    }
}


std::string OpenTable::toString() const {
    std::string str = "open " + std::to_string(tableId);
    if (getStatus() == COMPLETED) {
        for (auto customer : customers) {
            str.operator+=(" " + customer->getName() + "," + customer->getType());
        }
        return str + " Completed ";
    }
    if (getStatus() == ERROR) {
        for (auto customer : customers) {
            str += " " + (customer->getName()) + "," + (customer->getType());
        }
        return str + " " + getErrorMsg() ;
    }
    else {
        for (auto customer : customers) {
            str += " " + (customer->getName()) + "," + (customer->getType()) + " ";
        }
        return str + " Pending ";
    }
}

OpenTable *OpenTable::clone() const {
    return new OpenTable(*this);
}

//Destructor
OpenTable::~OpenTable() {
    for (auto &customer : customers) {
        delete customer;
    }
}

//Copy Constructor
OpenTable::OpenTable(const OpenTable &other) : tableId(other.tableId) ,customers(){
    if (other.getStatus()==COMPLETED)
        complete();
    else
        error(other.getErrorMsg());
    for (auto customer : other.customers) {
        customers.push_back(customer->clone());
    }
}

//Move constructor
OpenTable::OpenTable(OpenTable &&other) : tableId(other.tableId),customers(other.customers) {
    other.customers.clear();
}

Order::Order(int id) : tableId(id) {}

void Order::act(Restaurant &restaurant) {
    Table *t = restaurant.getTable(tableId);
    if (tableId > restaurant.getNumOfTables() || !(t->isOpen())) {
        BaseAction::error("Error: Table does not exist or is not open");
        std::cout << getErrorMsg()<< std::endl;
    }
        else {
        restaurant.getTable(tableId)->order(restaurant.getMenu());
        complete();
    }
}

std::string Order::toString() const {
    if (getStatus() == COMPLETED)
        return "order " + std::to_string(tableId) + " Completed ";
    if (getStatus() == ERROR)
        return "order " + std::to_string(tableId) + "Error: " + getErrorMsg() ;
    else
        return "order " + std::to_string(tableId) + " Pending ";
}

Order *Order::clone() const {
    return new Order(*this);
}

Order::~Order() = default;

MoveCustomer::MoveCustomer(int src, int dst, int customerId) : srcTable(src), dstTable(dst), id(customerId) {}

void MoveCustomer::act(Restaurant &restaurant) {
    if (srcTable > restaurant.getNumOfTables() || dstTable > restaurant.getNumOfTables()) {
        BaseAction::error("Error: Cannot move customer");
        std::cout << getErrorMsg()<< std::endl;
    }
        Table *src = restaurant.getTable(srcTable);
    Table *dst = restaurant.getTable(dstTable);
    if (!src->isOpen() || !dst->isOpen() || src->getCustomer(id) == nullptr || (unsigned)dst->getCapacity() == dst->getCustomers().size()) {
        BaseAction::error("Error: Cannot move customer");
        std::cout << getErrorMsg()<< std::endl;
    }
    else {
        dst->addCustomer(src->getCustomer(id)->clone());
        src->removeCustomer(id);
        std::vector<OrderPair> &p = restaurant.getTable(srcTable)->getOrders();
        for (auto &i : p) {
            if (i.first == id) {
                dst->getOrders().push_back(i);
                src->removeOrder(id);
            }
        }
        if (src->getCustomers().empty())
            src->closeTable();
        complete();
    }
}

std::string MoveCustomer::toString() const {
    if (getStatus() == COMPLETED)
        return "move " + std::to_string(srcTable) + " " + std::to_string(dstTable) + " " + std::to_string(id) + " Completed ";
    if (getStatus() == ERROR)
        return "move " + std::to_string(srcTable) + " " + std::to_string(dstTable) + " " + std::to_string(id) + "Error: " + getErrorMsg() ;
    else
        return "move " + std::to_string(srcTable) + " " + std::to_string(dstTable) + " " + std::to_string(id) + " Pending ";
}

MoveCustomer *MoveCustomer::clone() const {
    return new MoveCustomer(*this);
}

MoveCustomer::~MoveCustomer() = default;

Close::Close(int id) : tableId(id) {}

void Close::act(Restaurant &restaurant) {
    if (tableId > restaurant.getNumOfTables() || !restaurant.getTable(tableId)->isOpen()) {
        BaseAction::error("Error: Table does not exist or is not open");
        std::cout << getErrorMsg()<< std::endl;
    }
    else {
        int bill = restaurant.getTable(tableId)->getBill();
        std::cout << "Table " << tableId << " was closed. Bill " << bill << "NIS" << std::endl;
        restaurant.getTable(tableId)->closeTable();
        complete();
    }
}

std::string Close::toString() const {
    if (getStatus() == COMPLETED)
        return "close " + std::to_string(tableId) + " Completed";
    if (getStatus() == ERROR)
        return "close " + std::to_string(tableId) + " Error: " + getErrorMsg() ;
    else
        return "close " + std::to_string(tableId) + " Pending";
}

Close *Close::clone() const {
    return new Close(*this);
}

Close::~Close() = default;

CloseAll::CloseAll() {}

void CloseAll::act(Restaurant &restaurant) {
    for (int i = 0; i < restaurant.getNumOfTables(); ++i) {
        if (restaurant.getTable(i)->isOpen()) {
            Close c = Close(i);
            c.act(restaurant);
        }
    }
    restaurant.getMenu().clear();
    complete();
}

std::string CloseAll::toString() const {
    return "close all completed";
}

CloseAll *CloseAll::clone() const {
    return new CloseAll(*this);
}

CloseAll::~CloseAll() = default;

PrintMenu::PrintMenu() {}

void PrintMenu::act(Restaurant &restaurant) {
    std::cout<<restaurant.printMenu();
    complete();
}

std::string PrintMenu::toString() const {
    return "Print menu completed";
}

PrintMenu *PrintMenu::clone() const {
    return new PrintMenu(*this);
}

PrintMenu::~PrintMenu() = default;

PrintTableStatus::PrintTableStatus(int id) : tableId(id) {}

void PrintTableStatus::act(Restaurant &restaurant) {
    if (!(restaurant.getTable(tableId)->isOpen()))
        std::cout << "Table " << tableId << " status: " << "closed" << std::endl;
    else {
        std::cout << "Table " << tableId << " status: " << "open" << std::endl;
        std::vector<Customer *> &c = restaurant.getTable(tableId)->getCustomers();
        std::cout << "Customers:" << std::endl;
        for (auto &i : c) {
            std::cout << i->toString() << std::endl;
        }
        std::cout << "Orders:" << std::endl;
        std::vector<OrderPair> p = restaurant.getTable(tableId)->getOrders();
        for (auto &i : p) {
            std::cout << i.second.getName() << " " << i.second.getPrice() << "NIS " << i.first << std::endl;
        }
        std::cout << "Current Bill: " << restaurant.getTable(tableId)->getBill() << "NIS "<<std::endl;
    }
    complete();
}

std::string PrintTableStatus::toString() const {
    if (getStatus() == COMPLETED)
        return "Status " + std::to_string(tableId) + " " + "Completed ";
    if (getStatus() == ERROR)
        return "Status " + std::to_string(tableId) + " " + getErrorMsg() ;
    else
        return "Status " + std::to_string(tableId) + " " + "Pending ";
}

PrintTableStatus *PrintTableStatus::clone() const {
    return new PrintTableStatus(*this);
}

PrintTableStatus::~PrintTableStatus() = default;

PrintActionsLog::PrintActionsLog() = default;

void PrintActionsLog::act(Restaurant &restaurant) {
    std::vector<BaseAction *> bActions = restaurant.getActionsLog();
    for (auto &bAction : bActions) {
        std::cout<<bAction->toString()<<std::endl;
    }
    complete();
}

std::string PrintActionsLog::toString() const {
    return "Print actions log completed";
}

PrintActionsLog *PrintActionsLog::clone() const {
    return new PrintActionsLog(*this);
}

PrintActionsLog::~PrintActionsLog() = default;

BackupRestaurant::BackupRestaurant() {}

void BackupRestaurant::act(Restaurant &restaurant) {
    if (backup!=nullptr)
        backup->operator=(restaurant);
    else
        backup = new Restaurant(restaurant);
    complete();
}

std::string BackupRestaurant::toString() const {
    return "backup Completed";
}

BackupRestaurant *BackupRestaurant::clone() const {
    return new BackupRestaurant(*this);
}

BackupRestaurant::~BackupRestaurant() = default;

RestoreResturant::RestoreResturant() {}

void RestoreResturant::act(Restaurant &restaurant) {
    if (backup == nullptr) {
        BaseAction::error("Error: No available backup");
        std::cout << getErrorMsg()<< std::endl;
    }
    else {
        restaurant.operator=(*backup);
        complete();
    }
}

std::string RestoreResturant::toString() const {
    return "restore Completed";
}

RestoreResturant *RestoreResturant::clone() const {
    return new RestoreResturant(*this);
}

RestoreResturant::~RestoreResturant() = default;
