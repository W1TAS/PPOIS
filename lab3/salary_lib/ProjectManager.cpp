#include "ProjectManager.h"

ProjectManager::ProjectManager(int id, const std::string& first, const std::string& last, const std::string& pos, double rate, double bonus, const ContactInfo& contactInfo, const Address& address)
    : Manager(id, first, last, pos, rate, bonus, contactInfo, address) {}

void ProjectManager::manageProject(Project& project) {
    project.showProjectDetails();
}
