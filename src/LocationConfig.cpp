#include "../includes/LocationConfig.hpp"

LocationConfig::LocationConfig() {
    filePath = "";
    locationPath = "";
    autoIndex = false;
    tryFiles.clear();
    cgiIncludes.clear();
    fastcgiPass = "";
    fastcgiIndex = "";
    fastcgiParams.clear();
    cgiEnabled = false;
    cgi.clear();
}

LocationConfig::LocationConfig(const LocationConfig &other) {
    *this = other;
}

LocationConfig &LocationConfig::operator=(const LocationConfig &other) {
    if (this != &other) {
        this->filePath = other.filePath;
        this->locationPath = other.locationPath;
        this->autoIndex = other.autoIndex;
        this->tryFiles = other.tryFiles;
        this->cgiIncludes = other.cgiIncludes;
        this->fastcgiPass = other.fastcgiPass;
        this->fastcgiIndex = other.fastcgiIndex;
        this->fastcgiParams = other.fastcgiParams;
        this->cgiEnabled = other.cgiEnabled;
        this->cgi = other.cgi;
    }
    return *this;
}

LocationConfig::~LocationConfig() {}

std::string LocationConfig::getFilePath() const {
    return filePath;
}

void LocationConfig::setFilePath(const std::string &filePath) {
    this->filePath = filePath;
}

std::string LocationConfig::getLocationPath() const {
    return locationPath;
}

void LocationConfig::setLocationPath(const std::string &locationPath) {
    this->locationPath = locationPath;
}

bool LocationConfig::getAutoIndex() const {
    return autoIndex;
}

void LocationConfig::setAutoIndex(const std::string &autoIndex) {
    if (autoIndex == "on")
        this->autoIndex = true;
    else if (autoIndex == "off")
        this->autoIndex = false;
    else
        std::cerr << "Error: Invalid value for autoindex" << std::endl;
}

std::vector<std::string> LocationConfig::getTryFiles() const {
    return tryFiles;
}

void LocationConfig::addTryFile(const std::string &tryFile) {
    tryFiles.push_back(tryFile);
}

std::vector<std::string> LocationConfig::getCgiIncludes() const {
    return cgiIncludes;
}

void LocationConfig::addCgiInclude(const std::string &include) {
    cgiIncludes.push_back(include);
}

std::string LocationConfig::getFastcgiPass() const {
    return fastcgiPass;
}

void LocationConfig::setFastcgiPass(const std::string &fastcgiPass) {
    this->fastcgiPass = fastcgiPass;
}

std::string LocationConfig::getFastcgiIndex() const {
    return fastcgiIndex;
}

void LocationConfig::setFastcgiIndex(const std::string &fastcgiIndex) {
    this->fastcgiIndex = fastcgiIndex;
}

std::vector<std::pair<std::string, std::string> > LocationConfig::getFastcgiParams() const {
    return fastcgiParams;
}

void LocationConfig::addFastcgiParam(const std::string &param, const std::string &value) {
    fastcgiParams.push_back(std::make_pair(param, value));
}

bool LocationConfig::getCgiEnabled() const {
    return cgiEnabled;
}

void LocationConfig::setCgiEnabled(bool enabled) {
    this->cgiEnabled = enabled;
}

std::vector<std::pair<std::string, std::string> > LocationConfig::getCgi() const {
    return cgi;
}

void LocationConfig::addCgi(const std::string &extension, const std::string &interpreter) {
    cgi.push_back(std::make_pair(extension, interpreter));
}
