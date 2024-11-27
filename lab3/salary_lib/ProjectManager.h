#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include "Manager.h"
#include "Project.h"

class ProjectManager : public Manager {
public:
    ProjectManager(int id, const std::string& first, const std::string& last, const std::string& pos, double rate, double bonus, const ContactInfo& contactInfo, const Address& address);

    void manageProject(Project& project);
};

#endif // PROJECTMANAGER_H
