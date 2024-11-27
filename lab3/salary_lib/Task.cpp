#include "Task.h"

Task::Task(const std::string& description, const std::string& deadline)
    : description(description), deadline(deadline), completed(false) {}

void Task::setDescription(const std::string& description) {
    this->description = description;
}

void Task::setDeadline(const std::string& deadline) {
    this->deadline = deadline;
}

void Task::setCompleted(bool completed) {
    this->completed = completed;
}

std::string Task::getDescription() const {
    return description;
}

std::string Task::getDeadline() const {
    return deadline;
}

bool Task::isCompleted() const {
    return completed;
}
