#include <iostream>
#include "../include/Table.h"
#include <vector>
#include <unordered_map>

Table::Table(int t_capacity) : capacity(t_capacity), open(false), customersList(), orderList() {}

int Table::getCapacity() const {
    return capacity;
}

void Table::addCustomer(Customer *customer) {
    customersList.push_back(customer);
}

void Table::removeCustomer(int id) {
    for (int i = 0; (unsigned) i < customersList.size(); ++i) {
        if (customersList[i]->getId() == id) {
            delete customersList[i];
            customersList.erase(customersList.begin() + i);
        }
    }
}

Customer *Table::getCustomer(int id) {
    for (auto &i : customersList) {
        if (i->getId() == id)
            return i;
    }
    return nullptr;
}

std::vector<Customer *> &Table::getCustomers() {
    return customersList;
}

std::vector<OrderPair> &Table::getOrders() {
    return orderList;
}

void Table::order(const std::vector<Dish> &menu) {
    std::vector<int> ordered;
    for (auto &k : customersList) {
        ordered = std::vector<int>(k->order(menu));
        for (int j : ordered) {
            orderList.emplace_back(k->getId(), menu[j]);
            std::cout << k->getName() << " ordered " << menu[j].getName() << std::endl;
        }
    }
}

void Table::openTable() {
    open = true;
}

void Table::closeTable() {
    open = false;
    for (int i = 0; (unsigned)i < customersList.size(); ++i) {
        removeCustomer(customersList[i]->getId());
    }

    orderList.clear();
}

bool Table::isOpen() {
    return open;
}

int Table::getBill() {
    int bill = 0;
    for (auto &i : orderList) {
        bill = bill + i.second.getPrice();
    }
    return bill;
}

void Table::removeOrder(int id) {
    std::vector<OrderPair> p = std::vector<OrderPair>();
    for (auto &j : orderList) {
        if (j.first != id) {
            p.push_back(j);
        }
    }
    orderList.clear();
    for (const auto &i : p) {
        orderList.push_back(i);
    }
    p.clear();
}

//Destructor
Table::~Table() {
    for (int i = 0; (unsigned) i < customersList.size(); ++i) {
        delete customersList[i];
    }
    customersList.clear();
    orderList.clear();
}

//Copy Constructor
Table::Table(const Table &t2) : capacity(t2.capacity), open(t2.open), customersList(), orderList(t2.orderList) {
    for (auto i : t2.customersList) {
        customersList.push_back(i->clone());
    }
}

//Copy assignment operator
Table &Table::operator=(const Table &other) {
    if (this == &other)
        return *this;
    if (!customersList.empty())
        for (auto &i : customersList) {
            delete i;
            i = nullptr;
        }
    capacity = other.capacity;
    open = other.open;
    orderList.clear();
    for (const auto &k : other.orderList) {
        orderList.emplace_back(k.first, k.second);
    }
    for (auto j : other.customersList) {
        customersList.push_back(j->clone());
    }
    return *this;
}

//Move constructor
Table::Table(Table &&other) : capacity(other.capacity), open(other.open), customersList(other.customersList), orderList(other.orderList) {
    other.customersList.clear();
    other.capacity = 0;
    other.open = false;
    orderList.clear();
}

//Move assignment operator
Table &Table::operator=(Table &&other) {
    if (this != &other) {
        this->capacity = other.capacity;
        this->open = other.open;
        orderList.clear();
        for (const auto &i : other.orderList) {
            orderList.push_back(i);
        }
        this->customersList = other.customersList;
        other.capacity = 0;
        other.open = false;
        other.orderList.clear();
        other.customersList.clear();
    }
    return *this;
}





