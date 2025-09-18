#include "../../includes/config/LocationConfig.hpp"

LocationConfig::LocationConfig() {
    filePath = "";
    locationPath = "";
    root = "";
    autoIndex = false;
    errorPages.clear();
    indexFiles.clear();
    tryFiles.clear();
    cgiIncludes.clear();
    fastcgiPass = "";
    fastcgiIndex = "";
    fastcgiParams.clear();
    maxBodySize = 0;
}

LocationConfig::LocationConfig(const LocationConfig &other) {
    *this = other;
}

LocationConfig &LocationConfig::operator=(const LocationConfig &other) {
    if (this != &other) {
        this->filePath = other.filePath;
        this->locationPath = other.locationPath;
        this->root = other.root;
        this->autoIndex = other.autoIndex;
        this->errorPages = other.errorPages;
        this->indexFiles = other.indexFiles;
        this->tryFiles = other.tryFiles;
        this->cgiIncludes = other.cgiIncludes;
        this->fastcgiPass = other.fastcgiPass;
        this->fastcgiIndex = other.fastcgiIndex;
        this->fastcgiParams = other.fastcgiParams;
        this->maxBodySize = other.maxBodySize;
    }
    return *this;
}

LocationConfig::~LocationConfig() {}

std::string LocationConfig::getFilePath() const {
    return filePath;
}

void LocationConfig::setFilePath(const std::string &filePath) {
    this->filePath = filePath;
    std::cout << "File Path: " << this->filePath << std::endl;
}

std::string LocationConfig::getLocationPath() const {
    return locationPath;
}

void LocationConfig::setLocationPath(const std::string &locationPath) {
    this->locationPath = locationPath;
    std::cout << "Location Path: " << this->locationPath << std::endl;
}

std::string LocationConfig::getRoot() const {
    return root;
}

void LocationConfig::setRoot(const std::string &root) {
    this->root = root;
    std::cout << "Root: " << this->root << std::endl;
}

bool LocationConfig::getAutoIndex() const {
    return autoIndex;
}

void LocationConfig::setAutoIndex(const std::string &autoIndex) {
    if (autoIndex == "on") {
        this->autoIndex = true;
        std::cout << "AutoIndex: " << autoIndex << std::endl;
    } else if (autoIndex == "off") {
        this->autoIndex = false;
        std::cout << "AutoIndex: " << autoIndex << std::endl;
    } else {
        std::cerr << "Error: Invalid value for autoindex" << std::endl;
    }
}

std::vector<std::pair<std::string, int> > LocationConfig::getErrorPages() const {
    return errorPages;
}

void LocationConfig::addErrorPage(const std::string &errorPage, int errorCode) {
    errorPages.push_back(std::make_pair(errorPage, errorCode));
    std::cout << "Added error page: " << errorPage << " with error code: " << errorCode << std::endl;
}

std::vector<std::string> LocationConfig::getIndexFiles() const {
    return indexFiles;
}

void LocationConfig::addIndexFile(const std::string &indexFile) {
    indexFiles.push_back(indexFile);
    std::cout << "Added index file: " << indexFile << std::endl;
}

std::vector<std::string> LocationConfig::getTryFiles() const {
    return tryFiles;
}

void LocationConfig::addTryFile(const std::string &tryFile) {
    tryFiles.push_back(tryFile);
    std::cout << "Added try file: " << tryFile << std::endl;
}

std::vector<std::string> LocationConfig::getCgiIncludes() const {
    return cgiIncludes;
}

void LocationConfig::addCgiInclude(const std::string &include) {
    cgiIncludes.push_back(include);
    std::cout << "Added CGI include: " << include << std::endl;
}

std::string LocationConfig::getFastcgiPass() const {
    return fastcgiPass;
}

void LocationConfig::setFastcgiPass(const std::string &fastcgiPass) {
    this->fastcgiPass = fastcgiPass;
    std::cout << "FastCGI Pass: " << this->fastcgiPass << std::endl;
}

std::string LocationConfig::getFastcgiIndex() const {
    return fastcgiIndex;
}

void LocationConfig::setFastcgiIndex(const std::string &fastcgiIndex) {
    this->fastcgiIndex = fastcgiIndex;
    std::cout << "FastCGI Index: " << this->fastcgiIndex << std::endl;
}

std::vector<std::pair<std::string, std::string> > LocationConfig::getFastcgiParams() const {
    return fastcgiParams;
}

void LocationConfig::addFastcgiParam(const std::string &param, const std::string &value) {
    fastcgiParams.push_back(std::make_pair(param, value));
    std::cout << "Added FastCGI param: " << param << " = " << value << std::endl;
}

size_t LocationConfig::getMaxBodySize() const {
    return maxBodySize;
}

void LocationConfig::setMaxBodySize(size_t maxBodySize) {
    this->maxBodySize = maxBodySize;
    std::cout << "Max Body Size: " << this->maxBodySize << std::endl;
}
