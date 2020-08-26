#include <utility>
#include <iostream>
#include "../include/Customer.h"

Customer::Customer(std::string c_name, int c_id) : name(std::move(c_name)), id(c_id) {}

std::string Customer::getName() const {
    return name;
}

int Customer::getId() const {
    return id;
}

Customer::~Customer() = default;

VegetarianCustomer::VegetarianCustomer(std::string name, int id) : Customer(name, id) {}

std::vector<int> VegetarianCustomer::order(const std::vector<Dish> &menu) {
    std::vector<int> o;
    bool veg = false;
    // gets the VEG dish with lowest id
    for (int i = 0; (unsigned) i < menu.size() && !veg; ++i) {
        if (menu[i].getType() == VEG) {
            veg = true;
            o.push_back(menu[i].getId());
        }
    }
    if (!veg)
        return o;
    int price = 0;
    int cIndex = 0;
    // gets the most expansive beverage id
    for (int j = 0; (unsigned) j < menu.size(); ++j) {
        if (price < menu[j].getPrice() && menu[j].getType() == BVG) {
            price = menu[j].getPrice();
            cIndex = j;
        }
    }
    o.push_back(menu[cIndex].getId());
    if (o.size() == 1)
        o.clear();
    return o;
}

std::string VegetarianCustomer::toString() const {
    return std::to_string(VegetarianCustomer::getId()) + " " + VegetarianCustomer::getName();
}

VegetarianCustomer *VegetarianCustomer::clone() const {
    return new VegetarianCustomer(*this);
}

std::string VegetarianCustomer::getType() const {
    return "veg";
}

VegetarianCustomer::~VegetarianCustomer() = default;


CheapCustomer::CheapCustomer(std::string name, int id) : Customer(name, id), isOrdered(false) {}

std::vector<int> CheapCustomer::order(const std::vector<Dish> &menu) {
    std::vector<int> o;
    if (!isOrdered) {
        int price = menu[0].getPrice();
        int cIndex = 0;
        for (int i = 1; (unsigned) i < menu.size(); ++i) {
            if (price > menu[i].getPrice()) {
                price = menu[i].getPrice();
                cIndex = i;
            }
        }
        o.push_back(menu[cIndex].getId());
        isOrdered = true;
    }
    return o;

}

std::string CheapCustomer::toString() const {
    return std::to_string(CheapCustomer::getId()) + " " + CheapCustomer::getName();
}

CheapCustomer *CheapCustomer::clone() const {
    CheapCustomer *cl = new CheapCustomer(this->getName(), this->getId());
    cl->isOrdered = this->isOrdered;
    return cl;
}

std::string CheapCustomer::getType() const {
    return "chp";
}

CheapCustomer::~CheapCustomer() = default;


SpicyCustomer::SpicyCustomer(std::string name, int id) : Customer(name, id), isSpicy(false) {}

std::vector<int> SpicyCustomer::order(const std::vector<Dish> &menu) {
    std::vector<int> o;
    int price;
    int cIndex = -1;
    //if customer's first order
    if (!isSpicy) {
        price = 0;
        for (int j = 0; (unsigned) j < menu.size(); ++j) {
            if (price < menu[j].getPrice() && menu[j].getType() == SPC) {
                price = menu[j].getPrice();
                cIndex = j;
            }
        }
        if (cIndex!=-1) {
            isSpicy = true;
            o.push_back(menu[cIndex].getId());
        }
        return o;
    }
    //if customer already ordered spicy dish
    else {
        for (int i = 0; (unsigned) i < menu.size(); ++i) {
            if (menu[i].getType() == BVG) {
                price = menu[i].getPrice();
                cIndex = i;
            }
        }
        for (int j = 0; (unsigned) j < menu.size(); ++j) {
            if (price > menu[j].getPrice() && menu[j].getType() == BVG) {
                price = menu[j].getPrice();
                cIndex = j;
            }
        }
        if (cIndex != -1)
            o.push_back(menu[cIndex].getId());
        return o;
    }
}

std::string SpicyCustomer::toString() const {
    return std::to_string(SpicyCustomer::getId()) + " " + SpicyCustomer::getName();
}

SpicyCustomer *SpicyCustomer::clone() const {
    SpicyCustomer *cl = new SpicyCustomer(this->getName(), this->getId());
    cl->isSpicy = this->isSpicy;
    return cl;
}

std::string SpicyCustomer::getType() const {
    return "spc";
}

SpicyCustomer::~SpicyCustomer() = default;


AlchoholicCustomer::AlchoholicCustomer(std::string name, int id) : Customer(name, id), lastPaidPrice(0),
                                                                   lastAlcID(-1) {}

std::vector<int> AlchoholicCustomer::order(const std::vector<Dish> &menu) {
    std::vector<int> o;
    //if customer's first order
    if (lastAlcID == -1){
        for (const auto &i : menu) {
            if (i.getType()==ALC) {
                lastAlcID = i.getId();
                lastPaidPrice = i.getPrice();
            }
        }
        //finds the cheapest alcoholic beverage
        for (const auto &j : menu) {
            if (j.getType()==ALC && (j.getPrice()<lastPaidPrice || (j.getPrice() == lastPaidPrice && j.getId()<lastAlcID))){
                lastPaidPrice = j.getPrice();
                lastAlcID = j.getId();
            }
        }
        if (lastAlcID!=-1)
            o.push_back(lastAlcID);
    }
    //already order his first alcoholic beverages
    else{
        int tempID = lastAlcID;
        int tempPrice = lastPaidPrice;
        for (const auto &i : menu) {
            if(i.getType()==ALC && lastAlcID!=i.getId() && lastPaidPrice < i.getPrice()) {
                if(tempID==lastAlcID) {
                    tempID = i.getId();
                    tempPrice = i.getPrice();
                }
                else{
                    if(tempPrice > i.getPrice()){
                        tempPrice = i.getPrice();
                        tempID = i.getId();
                    }
                }
            }
            //find the cheapest alcoholic beverages that hasn't order yet by the customer
            if (i.getType()== ALC && lastAlcID < i.getId() && lastPaidPrice == i.getPrice()) {
                if (tempID == lastAlcID || tempPrice>lastPaidPrice) {
                    tempID = i.getId();
                    tempPrice = i.getPrice();
                }
                else {
                    if (tempID > i.getId()) {
                        tempID = i.getId();
                        tempPrice = i.getPrice();
                    }
                }
            }
        }
        if(tempID!=lastAlcID) {
            lastPaidPrice = tempPrice;
            lastAlcID = tempID;
            o.push_back(lastAlcID);
        }
    }
    return o;
}

std::string AlchoholicCustomer::toString() const {
    return std::to_string(AlchoholicCustomer::getId()) + " " + AlchoholicCustomer::getName();
}

AlchoholicCustomer *AlchoholicCustomer::clone() const {
    AlchoholicCustomer *cl = new AlchoholicCustomer(this->getName(), this->getId());
    cl->lastAlcID = this->lastAlcID;
    cl->lastPaidPrice = this->lastPaidPrice;
    return cl;
}

std::string AlchoholicCustomer::getType() const {
    return "alc";
}


AlchoholicCustomer::~AlchoholicCustomer() = default;


