#ifndef TASK_H
#define TASK_H

#include <string>

class Task {
public:
    Task(const std::string& description, const std::string& deadline);
    void setDescription(const std::string& description);
    void setDeadline(const std::string& deadline);
    void setCompleted(bool completed);

    std::string getDescription() const;
    std::string getDeadline() const;
    bool isCompleted() const;

private:
    std::string description;
    std::string deadline;
    bool completed;
};

#endif // TASK_H
