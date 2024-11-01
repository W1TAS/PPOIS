#ifndef DELIVERYPERSON_H
#define DELIVERYPERSON_H

#include "Employee.h"
#include "Order.h"

class DeliveryPerson : public Employee {
private:
    std::string vehicleType;

public:
    DeliveryPerson(const std::string& name, int age, int employeeID, double salary, const std::string& vehicleType);
    std::string getVehicleType() const;
    void deliverOrder(Order& order);
    void trackOrder(const std::string& orderID);
    double calculateBonus() const override;
    void work() override;
};

#endif // DELIVERYPERSON_H
