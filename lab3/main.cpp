#include "salary_lib/Employee.h"
#include "salary_lib/Manager.h"
#include "salary_lib/Department.h"
#include "salary_lib/Company.h"
#include "salary_lib/PayrollSystem.h"
#include "salary_lib/Project.h"
#include "salary_lib/Task.h"
#include "salary_lib/ProjectManager.h"
#include "salary_lib/TimeSheet.h"
#include "salary_lib/Payment.h"
#include "salary_lib/LoginSystem.h"
#include "salary_lib/ContactInfo.h"
#include "salary_lib/BankAccount.h"
#include "salary_lib/Attendance.h"
#include "salary_lib/Role.h"
#include "salary_lib/Report.h"
#include "salary_lib/Address.h"
#include <iostream>

int main() {
    try {
        Employee emp1(1, "John", "Doe", "Developer", 50, ContactInfo("123-456-7890", "john.doe@example.com"), Address("Minsk", "Lenina", 10));
        Employee emp2(2, "Jane", "Smith", "Designer", 45, ContactInfo("098-765-4321", "jane.smith@example.com"), Address("Minsk", "Pushkina", 15));
        ProjectManager pm1(3, "Alice", "Brown", "Manager", 60, 500, ContactInfo("111-222-3333", "alice.brown@example.com"), Address("Minsk", "Gorkogo", 20));

        Department dept1("IT");
        dept1.addEmployee(&emp1);
        dept1.addEmployee(&emp2);
        dept1.addEmployee(&pm1);

        Company company("Tech Corp");
        company.addDepartment(dept1);

        emp1.updateHoursWorked(160);
        emp2.updateHoursWorked(160);
        pm1.updateHoursWorked(160);

        PayrollSystem payrollSystem(company);
        payrollSystem.initialize();
        payrollSystem.generateReport();

        // Пример использования TimeSheet
        TimeSheet timesheet(&emp1, "2024-11-25", 160);
        timesheet.fillTimeSheet(160);

        // Пример использования Payment
        Payment payment(&emp1, emp1.calculateSalary(), "2024-11-25");
        payment.processPayment();

        // Пример использования LoginSystem
        LoginSystem loginSystem;
        loginSystem.registerUser("admin", "password");

        // Попытка регистрации пользователя с уже существующим логином (ошибка)
        try {
            loginSystem.registerUser("admin", "password");
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }

        // Попытка входа с неверным паролем (ошибка)
        if (loginSystem.login("admin", "wrongpassword")) {
            std::cout << "Login successful" << std::endl;
        } else {
            std::cout << "Login failed" << std::endl;
        }

        // Управление проектом
        Project project1("New Website");
        Task task1("Design Landing Page", "2024-12-01");
        Task task2("Develop Backend", "2024-12-15");

        project1.addTask(&task1);
        project1.addTask(&task2);
        project1.addEmployee(&emp1);
        project1.addEmployee(&emp2);

        pm1.manageProject(project1);

        // Пример использования ContactInfo и Address
        std::cout << "Employee: " << emp1.getFirstName() << " " << emp1.getLastName() << std::endl;
        std::cout << "Phone: " << emp1.getContactInfo().getPhone() << ", Email: " << emp1.getContactInfo().getEmail() << std::endl;
        std::cout << "Address: " << emp1.getAddress().getCity() << ", " << emp1.getAddress().getStreet() << ", " << emp1.getAddress().getHouseNumber() << std::endl;

        // Пример использования BankAccount
        BankAccount bankAccount("123456789", "Tech Bank");
        std::cout << "Account Number: " << bankAccount.getAccountNumber() << ", Bank: " << bankAccount.getBank() << std::endl;

        // Пример использования Attendance
        Attendance attendance(&emp1, "2024-11-25", true);
        std::cout << "Employee: " << attendance.getEmployee()->getFirstName() << ", Date: " << attendance.getDate() << ", Present: " << (attendance.isPresent() ? "Yes" : "No") << std::endl;

        // Пример использования Role
        Role role("Admin", "Full Access");
        std::cout << "Role: " << role.getName() << ", Permissions: " << role.getPermissions() << std::endl;

        // Пример использования Report
        Report report("Monthly", "Report Content");
        std::cout << "Report Type: " << report.getType() << ", Content: " << report.getContent() << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
