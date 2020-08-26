#include "../include/Dish.h"

Dish::Dish(int d_id, std::string d_name, int d_price, DishType d_type):id(d_id),name(d_name),price(d_price),type(d_type){}

int Dish::getId() const {
    return id;
}

std::string Dish::getName() const {
    return name;
}

int Dish::getPrice() const {
    return price;
}

DishType Dish::getType() const {
    return type;
}

std::string Dish::toString() {
    if (type==VEG)
        return name + " " + "VEG" + " "+std::to_string(price)+"NIS";
    if (type==SPC)
        return name+" " + "SPC" + " "+std::to_string(price)+"NIS";
    if (type==BVG)
        return name+" " + "BVG" + " "+std::to_string(price)+"NIS";
    else
        return name+" " + "ALC" + " "+std::to_string(price)+"NIS";
}