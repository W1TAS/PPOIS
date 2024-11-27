#ifndef REPORT_H
#define REPORT_H

#include <string>

class Report {
public:
    Report(const std::string& type, const std::string& content);
    void updateReport(const std::string& type, const std::string& content);

    std::string getType() const;
    std::string getContent() const;

private:
    std::string type;
    std::string content;
};

#endif // REPORT_H
