#ifndef LOCATION_CONFIG_HPP
#define LOCATION_CONFIG_HPP

#include <iostream>
#include <string>
#include <vector>

class LocationConfig {
    private:
        std::string filePath;
        std::string locationPath;
        std::string root;
        bool autoIndex;
        std::vector<std::string> indexFiles;
        std::vector<std::pair<std::string, int> > errorPages;
        std::vector<std::string> tryFiles;
        std::vector<std::string> cgiIncludes;
        std::string fastcgiPass;
        std::string fastcgiIndex;
        std::vector<std::pair<std::string, std::string> > fastcgiParams;
        size_t maxBodySize;

    public:
        LocationConfig();
        LocationConfig(const LocationConfig &other);
        LocationConfig &operator=(const LocationConfig &other);
        ~LocationConfig();

        std::string getFilePath() const;
        void setFilePath(const std::string &filePath);
        std::string getLocationPath() const;
        void setLocationPath(const std::string &locationPath);
        std::string getRoot() const;
        void setRoot(const std::string &root);
        bool getAutoIndex() const;
        void setAutoIndex(const std::string &autoIndex);
        std::vector<std::pair<std::string, int> > getErrorPages() const;
        void addErrorPage(const std::string &errorPage, int errorCode);
        std::vector<std::string> getIndexFiles() const;
        void addIndexFile(const std::string &indexFile);
        std::vector<std::string> getTryFiles() const;
        void addTryFile(const std::string &tryFile);
        std::vector<std::string> getCgiIncludes() const;
        void addCgiInclude(const std::string &include);
        std::string getFastcgiPass() const;
        void setFastcgiPass(const std::string &fastcgiPass);
        std::string getFastcgiIndex() const;
        void setFastcgiIndex(const std::string &fastcgiIndex);
        std::vector<std::pair<std::string, std::string> > getFastcgiParams() const;
        void addFastcgiParam(const std::string &param, const std::string &value);
        size_t getMaxBodySize() const;
        void setMaxBodySize(size_t maxBodySize);
};

#endif