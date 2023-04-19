// groceries_template.cpp: Stores Orders in a list.

#include <fstream>
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>
#include <vector>
#include <bits/stdc++.h>
#include "split.h"
using namespace std;

//////////////////
// Customer code /
//////////////////
struct Customer {
    int cust_id;
    string name;
    string street;
    string city;
    string state;
    string zip;
    string phone;
    string email;
    Customer(int id, const string& name, const string& street, const string& city,
             const string& state, const string& zip, const string& phone, const string& email)
        : name(name), street(street), city(city), state(state), zip(zip), phone(phone), email(email) 
    {
        cust_id = id;
    }
    string print_detail() const {
        stringstream stream;
        stream << "Customer ID #" << cust_id << ':' << endl << name << ", ph. " << phone << ", email: " << email << endl << street << endl << city << ", " << state << ' ' << zip;
        return stream.str();
    }
};
vector<Customer> customers;

void read_customers(const string& fname) {
    fstream customers_file(fname);
    string customer_data;
    int id;
    string name;
    string street;
    string city;
    string state;
    string zip;
    string phone;
    string email;

    getline(customers_file, customer_data);
    while (!customers_file.eof()) {
        auto splt_data = split(customer_data, ',');

        id = stoi(splt_data.at(0));
        name = splt_data.at(1);
        street = splt_data.at(2);
        city = splt_data.at(3);
        state = splt_data.at(4);
        zip = splt_data.at(5);
        phone = splt_data.at(6);
        email = splt_data.at(7);

        customers.emplace_back(id, name, street, city, state, zip, phone, email);
        getline(customers_file, customer_data);
    }
}

int find_cust_idx(int cust_id) {
    for (int i = 0; i < customers.size(); ++i)
        if (cust_id == customers.at(i).cust_id)
            return i;
    throw runtime_error("Customer not found");
}

//////////////
// Item code /
//////////////
struct Item {
    int item_id;
    string description;
    double price;
    Item(int id, const string& desc, double pric) : description(desc) {
        item_id = id;
        price = pric;
    }
};
vector<Item> items;

void read_items(const string& fname) {
    fstream item_file(fname);
    string item_data;

    getline(item_file, item_data);
    while (!item_file.eof()) {
        auto splt_data = split(item_data, ',');

        int id = stoi(splt_data.at(0));
        string desc = splt_data.at(1);
        double price = stod(splt_data.at(2));

        items.emplace_back(id, desc, price);
        getline(item_file, item_data);
    }
}

int find_item_idx(int item_id) {
    for (int i = 0; i < items.size(); ++i)
        if (item_id == items.at(i).item_id)
            return i;
    throw runtime_error("Item not found");
}

class LineItem {
        int item_id;
        int qty;
        friend class Order;
    public:
        LineItem(int id, int q) {
            item_id = id;
            qty = q;
        }
        double sub_total() const {
            int idx = find_item_idx(item_id);
            return items.at(idx).price * qty;
        }
        friend bool operator<(const LineItem& item1, const LineItem& item2) {
            return item1.item_id < item2.item_id;
        }
};

/////////////////
// Payment code /
/////////////////
class Payment {
    public:
        Payment() = default;
        virtual ~Payment() = default;
        virtual string print_detail() const = 0;
    private:
        double amount = 0.0;
        friend class Order;
};

class Credit : public Payment {
    public:
        Credit(string card_num, string exp_date) {
            card_number = card_num;
            expiration = exp_date;
        }
        ~Credit() = default;
        string print_detail() const override {
            return "Paid by Credit card " + card_number + ", exp. " + expiration;
        }
    private:
        string card_number;
        string expiration;
};

class Paypal : public Payment {
    public:
        Paypal(string id) {
            paypal_id = id;
        }
        ~Paypal() = default;
        string print_detail() const override {
            return "Paid by Paypal account " + paypal_id;
        }
    private:
        string paypal_id;
};

class WireTransfer : public Payment {
    public:
        WireTransfer(string bank, string account) {
            bank_id = bank;
            account_id = account;
        }
        ~WireTransfer() = default;
        string print_detail() const override {
            return "Paid by Wire transfer bank " + bank_id + ", account " + account_id;
        }
    private:
        string bank_id;
        string account_id;
};

///////////////
// Order code /
///////////////
class Order {
    int order_id;
    string order_date;
    int cust_id;
    vector<LineItem> line_items;
    Payment* payment;
public:
    Order(int id, const string& date, int c_id, const vector<LineItem>& items, Payment* p) 
    : order_date(date), line_items(items) {
        order_id = id;
        cust_id = c_id;
        payment = p;
        sort(line_items.begin(), line_items.end());
        double total = 0;
        for (LineItem item : line_items) {
            total += item.sub_total();
        }
        payment->amount = total;
    }
    ~Order() {
        delete payment;
    }
    double total() const {
        return payment->amount;
    }
    string print_order() const {
        stringstream stream;
        stream << setprecision(2) << fixed << "===========================" << endl << "Order #" << order_id << ", Date: " << order_date << endl << "Amount: $" << total() << ", " << payment->print_detail() << endl << endl << customers.at(find_cust_idx(cust_id)).print_detail() << endl << endl << "Order Detail:" << endl;
        for (LineItem item : line_items) {
            Item curr_item = items.at(find_item_idx(item.item_id));
            stream << setprecision(2) << fixed << "\t\tItem " << curr_item.item_id << ": \"" << curr_item.description << "\", " << item.qty << " @ " << curr_item.price << endl;
        }
        return stream.str();
    }
};
list<Order> orders;

void read_orders(const string& fname) {
    ifstream orderf(fname);
    string items;
    string payment;

    getline(orderf, items);
    while (!orderf.eof()) {
        auto splt_line = split(items, ',');

        int cust_id = stoi(splt_line.at(0));
        int order_id = stoi(splt_line.at(1));
        string date = splt_line.at(2);

        vector<LineItem> line_items;
        for (int i = 3; i < splt_line.size(); i++) {
            auto item = split(splt_line.at(i), '-');
            int id = stoi(item.at(0));
            int quantity = stoi(item.at(1));

            line_items.emplace_back(id, quantity);
        }
        sort(begin(line_items), end(line_items));

        getline(orderf, payment);
        auto splt_line_2 = split(payment, ',');
        int pay_method = stoi(splt_line_2.at(0));

        Payment* pmt;
        if (pay_method == 1) {
            pmt = new Credit(splt_line_2.at(1), splt_line_2.at(2));
        }
        else if (pay_method == 2) {
            pmt = new Paypal(splt_line_2.at(1));
        }
        else {
            pmt = new WireTransfer(splt_line_2.at(1), splt_line_2.at(2));
        }

        orders.emplace_back(order_id, date, cust_id, line_items, pmt);
        getline(orderf, items);
    }
}

int main() {
    read_customers("customers.txt");
    read_items("items.txt");
    read_orders("orders.txt");

    ofstream ofs("order_report.txt");
    for (const Order& order: orders)
        ofs << order.print_order() << endl;
}
