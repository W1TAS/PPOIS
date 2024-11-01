#include <iostream>
#include "pizzeria_lib/Person.h"
#include "pizzeria_lib/Customer.h"
#include "pizzeria_lib/Pizza.h"
#include "pizzeria_lib/Order.h"
#include "pizzeria_lib/Manager.h"
#include "pizzeria_lib/Chef.h"
#include "pizzeria_lib/DeliveryPerson.h"
#include "pizzeria_lib/OnlineCustomer.h"
#include "pizzeria_lib/InStoreCustomer.h"
#include "pizzeria_lib/CreditCardPayment.h"
#include "pizzeria_lib/CashPayment.h"
#include "pizzeria_lib/BankTransferPayment.h"
#include "pizzeria_lib/LoyaltyProgram.h"
#include <windows.h>

int main() {
    SetConsoleOutputCP(CP_UTF8);

    // Создание объекта InStoreCustomer
    InStoreCustomer inStoreCustomer("pupa", 30, 1001, 500, "12345-67890");

    // Создание заказа для InStoreCustomer
    Order order1(1, &inStoreCustomer);
    inStoreCustomer.placeOrder(order1);

    // Добавление пиццы в заказ
    Pizza pizza1("Large", "Thin Crust", 16.0);
    pizza1.addTopping("Ketcup");
    pizza1.addTopping("Cheese");
    pizza1.addTopping("Peperoni");
    Pizza pizza2("Medium", "Thick Crust", 12.0);
    pizza2.addTopping("Cheese");
    pizza2.addTopping("Cheese2");
    pizza2.addTopping("Cheese3");
    pizza2.addTopping("Cheese4");
    order1.addPizza(pizza1);
    order1.addPizza(pizza2);

    // Вывод общей стоимости заказа
    std::cout << "Общая стоимость до скидки: " << order1.getTotalPrice() << std::endl;

    // Использование бонусных очков
    inStoreCustomer.useLoyaltyPoints(20, order1); // Пример: используем 20 бонусных очков

    // Создание объекта LoyaltyProgram и использование его
    LoyaltyProgram loyaltyProgram(1, 0.1); // Программа с 10% скидкой
    inStoreCustomer.useLoyaltyProgram(loyaltyProgram, order1);

    // Оплата заказа
    CashPayment CashPayment(1, order1.getTotalPrice(), 500);
    inStoreCustomer.makePayment(CashPayment);
    std::cout <<"Баллы: "<< inStoreCustomer.getLoyaltyPoints() << std::endl;

    // Создание объекта OnlineCustomer
    OnlineCustomer onlineCustomer("lupa", 25, 1002, 300, "lupa@example.com", "password123");

    // Создание заказа для OnlineCustomer
    Order order2(2, &onlineCustomer);
    onlineCustomer.placeOrder(order2);

    // Добавление пиццы в заказ
    Pizza pizza3("Small", "Thin Crust", 10.0);
    order2.addPizza(pizza3);


    // Использование бонусных очков
    onlineCustomer.useLoyaltyPoints(50, order2); // Пример: используем 50 бонусных очков


    // Использование программы лояльности
    onlineCustomer.useLoyaltyProgram(loyaltyProgram, order2);


    // Оплата заказа
    CreditCardPayment CreditCardPayment(2, order2.getTotalPrice(), "1234123412341234", "12/26", 1000);
    onlineCustomer.makePayment(CreditCardPayment);
    std::cout <<"Баллы: "<< onlineCustomer.getLoyaltyPoints() << std::endl;

    // Демонстрация работы классов Manager, Chef и DeliveryPerson
    Manager manager("Karen Milton", 40, 3001, 5000.0);
    Chef chef("Виктор Баринов", 35, 3002, 3000.0, "Italian Cuisine");
    DeliveryPerson deliveryPerson("Тимур", 28, 3003, 2000.0, "Bike");

    // Добавление сотрудников к менеджеру
    manager.addEmployee(&chef);
    manager.addEmployee(&deliveryPerson);

    // Работа шеф-повара и доставка заказа
    std::cout << "Работа шеф-повара:" << std::endl;
    chef.work();
    chef.prepareOrder(order1);

    std::cout << "Доставка заказа:" << std::endl;
    deliveryPerson.work();
    deliveryPerson.deliverOrder(order1);

    // Работа менеджера
    std::cout << "Работа менеджера:" << std::endl;
    manager.work();

    return 0;
}
