#include "Report.h"

Report::Report(const std::string& type, const std::string& content)
    : type(type), content(content) {}

void Report::updateReport(const std::string& type, const std::string& content) {
    this->type = type;
    this->content = content;
}

std::string Report::getType() const {
    return type;
}

std::string Report::getContent() const {
    return content;
}
