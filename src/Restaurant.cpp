#include "../include/Restaurant.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

Restaurant::Restaurant() : open(false), tables(), menu(), actionsLog() {}

Restaurant::Restaurant(const std::string &configFilePath) : open(false), tables(), menu(), actionsLog() {
    const std::string &filePath = configFilePath;
    std::ifstream infile(filePath, ios::in);
    std::string line;
    int counter = 1;
    std::vector<std::string> dishes;
    while (std::getline(infile, line)) {
        if (line[line.size()-1] == '\r')
            line.erase(line.size() - 1);
        //get the dishes of the menu
        if (line[0] != '#' && !line.empty() && counter > 2) {
            dishes = split(line, ',');
            int dishID = menu.size();
            menu.emplace_back(dishID, dishes[0], std::atoi(dishes[2].c_str()), strToDish(dishes[1]));
            counter++;

        }
        //get the sizes of each table
        if (line[0] != '#' && !line.empty() && counter == 2) {
            std::vector<std::string> tableSizes;
            tableSizes = split(line, ',');
            for (int i = 0; (unsigned) i < tables.size(); ++i) {
                tables[i] = new Table(std::atoi(tableSizes[i].c_str()));
            }
            counter++;

        }
        //get the number of tables in the restaurant
        if (line[0] != '#' && !line.empty() && counter == 1) {
            tables.resize(std::atoi(line.c_str()));
            counter++;

        }

    }
}


DishType Restaurant::strToDish(std::string str) {
    if (str == "VEG")
        return VEG;
    if (str == "SPC")
        return SPC;
    if (str == "BVG")
        return BVG;
    else
        return ALC;
}

void Restaurant::start() {
    std::cout << "Restaurant is now open!" << std::endl;
    open = true;
    int customerID = 0;
    std::string str;
    getline(std::cin, str);
    //gets actions until action "closeall"
    while (str != "closeall") {
        std::vector<std::string> s = split(str, ' ');
        //prints the menu of the restaurant
        if (s[0] == "menu") {
            BaseAction *printmenu = new PrintMenu;
            printmenu->act(*this);
            actionsLog.push_back(printmenu);
        }
        //Opens a given table and assigns a list of customers to it.
        if (s[0] == "open") {
            std::vector<Customer *> customers;
            for (int i = 2; (unsigned) i < s.size(); ++i) {
                std::vector<std::string> tokens = split(s[i], ',');
                if (tokens[1] == "chp") {
                    customers.push_back(new CheapCustomer(tokens[0], customerID));
                    customerID++;
                }
                if (tokens[1] == "alc") {
                    customers.push_back(new AlchoholicCustomer(tokens[0], customerID));
                    customerID++;
                }
                if (tokens[1] == "veg") {
                    customers.push_back(new VegetarianCustomer(tokens[0], customerID));
                    customerID++;
                }
                if (tokens[1] == "spc") {
                    customers.push_back(new SpicyCustomer(tokens[0], customerID));
                    customerID++;
                }
            }
            BaseAction *openTable = new OpenTable(std::atoi(s[1].c_str()), customers);
            openTable->act(*this);
            actionsLog.push_back(openTable);
        }
        //takes an order from a given table
        if (s[0] == "order") {
            BaseAction *order = new Order(std::atoi(s[1].c_str()));
            order->act(*this);
            actionsLog.push_back(order);
        }
        //move a customer from one table to another
        if (s[0] == "move") {
            BaseAction *moveTable = new MoveCustomer(std::atoi(s[1].c_str()), std::atoi(s[2].c_str()),
                                                     std::atoi(s[3].c_str()));
            moveTable->act(*this);
            actionsLog.push_back(moveTable);
        }
        //closes a given table
        if (s[0] == "close") {
            BaseAction *closeTable = new Close(std::atoi(s[1].c_str()));
            closeTable->act(*this);
            actionsLog.push_back(closeTable);
        }
        //print a status report of a given table
        if (s[0] == "status") {
            BaseAction *status = new PrintTableStatus(std::atoi(s[1].c_str()));
            status->act(*this);
            actionsLog.push_back(status);
        }
        //prints all the actions that were performed by the user
        if (s[0] == "log") {
            BaseAction *log = new PrintActionsLog();
            log->act(*this);
            actionsLog.push_back(log);
        }
        //save all restaurant informaition
        if (s[0] == "backup") {
            BaseAction *backup = new BackupRestaurant();
            backup->act(*this);
            actionsLog.push_back(backup);
        }
        //restore the backed up restaurant status and over write the current restaurant status
        if (s[0] == "restore") {
            BaseAction *restore = new RestoreResturant();
            restore->act(*this);
            actionsLog.push_back(restore);
        }
        getline(std::cin, str);

    }
    BaseAction *closeAll = new CloseAll();
    closeAll->act(*this);
    open = false;
    delete closeAll;
}

int Restaurant::getNumOfTables() const {
    return static_cast<int>(tables.size());
}

Table *Restaurant::getTable(int ind) {
    return tables[ind];
}

const std::vector<BaseAction *> &Restaurant::getActionsLog() const {
    return actionsLog;
}

std::vector<Dish> &Restaurant::getMenu() {
    return menu;
}

std::string Restaurant::printMenu() {
    std::string output;
    for (auto &i : menu) {
        output.operator+=(i.toString() + "\n");
    }
    return output;
}

//Destructor
Restaurant::~Restaurant() {
    for (auto &table : tables) {
        delete table;
    }
    tables.clear();
    menu.clear();
    for (auto &j : actionsLog) {
        delete j;
    }
    actionsLog.clear();

}

//Copy constructor
Restaurant::Restaurant(const Restaurant &other) : open(other.open), tables(), menu(other.menu), actionsLog() {
    for (auto &t : other.tables) {
        tables.push_back(new Table(*t));
    }
    for (auto &i : other.actionsLog) {
        actionsLog.push_back(i->clone());
    }
}

//Copy assignment operator
Restaurant &Restaurant::operator=(const Restaurant &other) {
    if (this == &other)
        return *this;
    if (!tables.empty())
        for (auto &table : tables) {
            delete table;
        }
    tables.clear();
    if (!actionsLog.empty())
        for (auto &i : actionsLog) {
            delete i;
        }
    actionsLog.clear();
    open = other.open;
    for (auto table : other.tables) {
        auto *newTable = new Table(*table);
        tables.push_back(newTable);
    }
    for (auto j : other.actionsLog) {
        actionsLog.push_back(j->clone());
    }
    menu.clear();
    for (const auto &k : other.menu) {
        menu.emplace_back(k);
    }
    return *this;
}

//Move Constructor
Restaurant::Restaurant(Restaurant &&other) : open(other.open), tables(other.tables), menu(other.menu),
                                             actionsLog(other.actionsLog) {
    other.actionsLog.clear();
    other.tables.clear();
    other.open = false;
}

// Move assignment operator
Restaurant &Restaurant::operator=(Restaurant &&other) {
    if (this != &other) {
        this->open = other.open;
        menu.clear();
        for (const auto &k : other.menu) {
            menu.emplace_back(k);
        }
        if (!tables.empty())
            for (auto &table : tables) {
                delete table;
            }
        tables.clear();
        if (!actionsLog.empty())
            for (auto &j : actionsLog) {
                delete j;
            }
        actionsLog.clear();
        for (auto &table : other.tables) {
            tables.push_back(new Table(*table));
        }
        for (auto &l : other.actionsLog) {
            actionsLog.push_back(l->clone());
        }
        other.open = false;
        other.menu.clear();
        other.tables.clear();
        other.actionsLog.clear();
    }
    return *this;
}

//split given string to vector of strings according to a given delimiter
std::vector<std::string> Restaurant::split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}




