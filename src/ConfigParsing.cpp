#include "../includes/ConfigParsing.hpp"

ConfigParsing::ConfigParsing() {}

ConfigParsing::ConfigParsing(const ConfigParsing &other) {
    *this = other;
}

ConfigParsing &ConfigParsing::operator=(const ConfigParsing &other) {
    if (this != &other) {
        this->servers = other.servers;
    }
    return *this;
}

ConfigParsing::~ConfigParsing() {
    for (size_t i = 0; i < servers.size(); i++)
        delete servers[i];
    servers.clear();
    for (std::map<ServerConfig*, std::vector<LocationConfig*> >::iterator it = locations.begin(); it != locations.end(); ++it) {
        for (size_t j = 0; j < it->second.size(); j++) {
            delete it->second[j];
        }
    }
    locations.clear();
}

std::vector<ServerConfig *> ConfigParsing::getServers() const {
    return servers;
}

const std::map<ServerConfig *, std::vector<LocationConfig*> >& ConfigParsing::getLocations() const {
    return locations;
}

void ConfigParsing::setConfigFile(const std::string &configFile) {
    this->configFile = configFile;
}

std::string ConfigParsing::getConfigFile() const {
    return this->configFile;
}

bool ConfigParsing::isFileReadable(const std::string &filePath) {
    std::ifstream file(filePath.c_str());
    struct stat s;
    if (stat(filePath.c_str(), &s) == 0 && S_ISDIR(s.st_mode))
        return false;
    if (filePath.size() < 5 || filePath.substr(filePath.size() - 5) != ".conf")
        return false;
    return file.good();
}

std::vector<std::string> ConfigParsing::tokenize(const std::string &filename, const std::string &delimiters) {
    std::ifstream file(filename.c_str());
    std::string content;
    if (file) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        content = buffer.str();
    }
    std::vector<std::string> tokens;
    std::stringstream ss(content);
    std::string line;
    int brace = 0;
    while (std::getline(ss, line)) {
        size_t comment_pos = line.find('#');
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
            line.erase(line.find_last_not_of(" \t") + 1);
        }
        if (line.find("{") != std::string::npos)
            brace++;
        if (line.find("}") != std::string::npos)
            brace--;
        if (!line.empty() && line[line.length() - 1] != ';' && line[line.length() - 1] != '{' && line[line.length() - 1] != '}') {
            std::cerr << "Error: Missing semicolon or brace in line: " << line << std::endl;
            continue;
        } else if (!line.empty() && line[line.length() - 1] == ';') {
            line = line.substr(0, line.length() - 1); // remove the semicolon
        }
        size_t pos = 0;
        while (pos < line.length()) {
            pos = line.find_first_not_of(delimiters, pos);
            if (pos == std::string::npos)
                break;
            size_t end = line.find_first_of(delimiters, pos);
            std::string token = (end == std::string::npos) ? line.substr(pos) : line.substr(pos, end - pos);
            if (!token.empty())
                tokens.push_back(token);
            pos = (end == std::string::npos) ? line.length() : end + 1;
        }
    }
    if (brace < 0)
        std::cerr << "Error: Unmatched closing brace '}'" << std::endl;
    if (brace > 0)
        std::cerr << "Error: Unmatched opening brace '{'" << std::endl;
    file.close();
    return tokens;
}

// Also check for unknown directives, paths, ...

void ConfigParsing::parse(std::vector<std::string> &tokens) {
    size_t i = 0;
    while (i < tokens.size()) {
        if (tokens[i] == "server" && i + 1 < tokens.size() && tokens[i + 1] == "{") {
            ServerConfig* server = new ServerConfig();
            i += 2; // Skip "server" and "{"
            while (i < tokens.size() && tokens[i] != "}") {
                if (tokens[i] == "listen") {
                    size_t j = i + 1;
                    while (j < tokens.size() && isdigit(tokens[j][0])) {
                        int port = atoi(tokens[j].c_str());
                        if (port <= 0 || port > 65535) {
                            std::cerr << "Error: Invalid port number " << tokens[j] << std::endl;
                            j++;
                            continue;
                        }
                        server->setPorts(port);
                        j++;
                    }
                    i = j;
                } else if (tokens[i] == "server_name") {
                    if (i + 1 < tokens.size()) {
                        server->setHost(tokens[i + 1]);
                        i += 2;
                    } else {
                        std::cerr << "Error: Expected server name after 'server_name'" << std::endl;
                        i++;
                    }
                } else if (tokens[i] == "root") {
                    if (i + 1 < tokens.size()) {
                        server->setRoot(tokens[i + 1]);
                        i += 2;
                    } else {
                        std::cerr << "Error: Expected path after 'root'" << std::endl;
                        i++;
                    }
                } else if (tokens[i] == "error_page") {
                    size_t j = i + 1;
                    std::vector<size_t> errorCodes;
                    std::string errorPage;
                    while (j < tokens.size()) {
                        if (tokens[j].find(".html") != std::string::npos) {
                            errorPage = tokens[j];
                            break;
                        } else {
                            size_t code = atoi(tokens[j].c_str());
                            if (code != 0)
                                errorCodes.push_back(code);
                        }
                        j++;
                    }
                    if (!errorPage.empty() && !errorCodes.empty()) {
                        for (size_t k = 0; k < errorCodes.size(); k++) {
                            server->addErrorPage(errorPage, errorCodes[k]);
                        }
                        i = j + 1;
                    } else {
                        std::cerr << "Error: Invalid error_page directive" << std::endl;
                        i++;
                    }
				} else if (tokens[i] == "index") {
					size_t j = i + 1;
					while (j < tokens.size() && tokens[j] != "}" && tokens[j] != "location" &&
						tokens[j] != "listen" && tokens[j] != "server_name" &&
						tokens[j] != "root" && tokens[j] != "error_page" &&
						tokens[j] != "client_max_body_size" && tokens[j] != "method") {
						server->addIndexFile(tokens[j]);
						j++;
					}
					i = j;
				} else if (tokens[i] == "client_max_body_size") {
                    if (i + 1 < tokens.size()) {
                        size_t maxBodySize = static_cast<size_t>(atoi(tokens[i + 1].c_str()));
                        server->setMaxBodySize(maxBodySize);
                        i += 2;
                    } else {
                        std::cerr << "Error: Expected size after 'client_max_body_size'" << std::endl;
                        i++;
                    }
				} else if (tokens[i] == "upload_path") {
					if (i + 1 < tokens.size()) {
						server->setUploadPath(tokens[i + 1]);
						i += 2;
					} else {
						std::cerr << "Error: Expected path after 'upload_path'" << std::endl;
						i++;
					}
                } else if (tokens[i] == "method") {
                    std::vector<std::string> methods;
                    size_t j = i + 1;
                    while (j < tokens.size() && (tokens[j] == "GET" || tokens[j] == "POST" || tokens[j] == "DELETE" || tokens[j] == "HEAD")) {
                        methods.push_back(tokens[j]);
                        j++;
                    }
                    server->setMethods(methods);
                    i = j;
                } else if (tokens[i] == "location") {
                    LocationConfig* location = new LocationConfig();
                    if (i + 2 < tokens.size() && tokens[i + 2] == "{") {
                        location->setLocationPath(tokens[i + 1]);
                        i += 3; // Skip "location", path, and "{"
                        while (i < tokens.size() && tokens[i] != "}") {
                            if (tokens[i] == "autoindex") {
                                if (i + 1 < tokens.size()) {
                                    location->setAutoIndex(tokens[i + 1]);
                                    i += 2;
                                } else {
                                    std::cerr << "Error: Expected 'on' or 'off' after 'autoindex'" << std::endl;
                                    i++;
                                }
                            } else if (tokens[i] == "return") {
                                if (i + 2 < tokens.size() && isdigit(tokens[i + 1][0])
                                    && tokens[i + 1].size() == 3 && tokens[i + 1][0] == '3') {
                                    location->addRedirect(location->getLocationPath(), tokens[i + 2]);
                                    i += 3;
                                } else {
                                    std::cerr << "Error: Expected return code and path after 'return'" << std::endl;
                                    i++;
                                }
                            } else if (tokens[i] == "try_files") {
                                i++;
                                while (i < tokens.size() && tokens[i] != "}") {
                                    // if (tokens[i].find("=") != std::string::npos) {
                                    //     std::string codeStr = tokens[i].substr(1);
                                    //     int code = atoi(codeStr.c_str());
                                    //     std::cout << "Return error code " << code << " if not found" << std::endl;
                                    // } else {
                                        location->addTryFile(tokens[i]);
                                    // }
                                    i++;
                                }
                            } else if (tokens[i] == "cgi_enabled") {
                                if (i + 1 < tokens.size()) {
                                    if (tokens[i + 1] == "on")
                                        location->setCgiEnabled(true);
                                    else if (tokens[i + 1] == "off")
                                        location->setCgiEnabled(false);
                                    else
                                        std::cerr << "Error: Invalid value for cgi_enabled" << std::endl;
                                    i += 2;
                                } else {
                                    std::cerr << "Error: Expected 'on' or 'off' after 'cgi_enabled'" << std::endl;
                                    i++;
                                }
                            } else if (tokens[i] == "cgi") {
                                if (i + 2 < tokens.size()) {
                                    location->addCgi(tokens[i + 1], tokens[i + 2]);
                                    i += 3;
                                } else {
                                    std::cerr << "Error: Expected extension and interpreter after 'cgi'" << std::endl;
                                    i++;
                                }
                            } else if (tokens[i] == "include") {
                                if (i + 1 < tokens.size()) {
                                    location->addCgiInclude(tokens[i + 1]);
                                    i += 2;
                                } else {
                                    std::cerr << "Error: Expected file path after 'include'" << std::endl;
                                    i++;
                                }
                            } else if (tokens[i] == "fastcgi_pass") {
                                if (i + 1 < tokens.size()) {
                                    location->setFastcgiPass(tokens[i + 1]);
                                    i += 2;
                                } else {
                                    std::cerr << "Error: Expected address after 'fastcgi_pass'" << std::endl;
                                    i++;
                                }
                            } else if (tokens[i] == "fastcgi_index") {
                                if (i + 1 < tokens.size()) {
                                    location->setFastcgiIndex(tokens[i + 1]);
                                    i += 2;
                                } else {
                                    std::cerr << "Error: Expected file name after 'fastcgi_index'" << std::endl;
                                    i++;
                                }
                            } else if (tokens[i] == "fastcgi_param") {
                                if (i + 2 < tokens.size()) {
                                    location->addFastcgiParam(tokens[i + 1], tokens[i + 2]);
                                    i += 3;
                                } else {
                                    std::cerr << "Error: Expected parameter and value after 'fastcgi_param'" << std::endl;
                                    i++;
                                }
                            } else {
                                std::cerr << "Unknown directive in location block: " << tokens[i] << std::endl;
                                i++;
                            }
                        }
                        locations[server].push_back(location);
                        if (i < tokens.size() && tokens[i] == "}")
                            i++;
                    } else {
                        std::cerr << "Error: Expected path and '{' after 'location'" << std::endl;
                        i++;
                    }
                } else {
                    i++;
                }
            }
            servers.push_back(server);
            if (i < tokens.size() && tokens[i] == "}")
                i++;
        } else {
            i++;
        }
    }
    printConfig();
}

void ConfigParsing::printConfig() const {
    for (size_t i = 0; i < servers.size(); i++) {
        std::cout << "Server " << i + 1 << ":" << std::endl;
        std::cout << "  Host: " << servers[i]->getHost() << std::endl;
        std::cout << "  Ports: ";
        std::vector<int> ports = servers[i]->getPorts();
        for (size_t j = 0; j < ports.size(); j++) {
            std::cout << ports[j];
            if (j < ports.size() - 1)
                std::cout << ", ";
        }
        std::cout << std::endl;
        std::cout << "  Root: " << servers[i]->getRoot() << std::endl;
        std::cout << "  Index: ";
        for (size_t j = 0; j < servers[i]->getIndexFiles().size(); j++) {
            std::cout << servers[i]->getIndexFiles()[j];
            if (j < servers[i]->getIndexFiles().size() - 1)
                std::cout << ", ";
        }
        std::cout << std::endl;
        std::cout << "  Error Pages: ";
        for (size_t j = 0; j < servers[i]->getErrorPages().size(); j++) {
            std::cout << servers[i]->getErrorPages()[j].second << "->" << servers[i]->getErrorPages()[j].first;
            if (j < servers[i]->getErrorPages().size() - 1)
                std::cout << ", ";
        }
        if (servers[i]->getMaxBodySize() > 0)
            std::cout << std::endl << "  Max Body Size: " << servers[i]->getMaxBodySize() << " bytes";
        if (!servers[i]->getMethods().empty()) {
            std::cout << std::endl << "  Methods: ";
            std::vector<std::string> methods = servers[i]->getMethods();
            for (size_t j = 0; j < methods.size(); j++) {
                std::cout << methods[j];
                if (j < methods.size() - 1)
                    std::cout << ", ";
            }
        }
        std::cout << std::endl;
        std::cout << "  Locations: ";
        std::map<ServerConfig*, std::vector<LocationConfig*> >::const_iterator locIt = locations.find(servers[i]);
        if (locIt != locations.end()) {
            const std::vector<LocationConfig*>& locVec = locIt->second;
            for (size_t j = 0; j < locVec.size(); j++) {
                std::cout << locVec[j]->getLocationPath();
                if (j < locVec.size() - 1)
                    std::cout << ", ";
            }
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;
    size_t locCount = 1;
    for (std::map<ServerConfig*, std::vector<LocationConfig*> >::const_iterator it = locations.begin(); it != locations.end(); ++it) {
        for (size_t i = 0; i < it->second.size(); i++) {
            std::cout << "Location " << locCount << ":" << std::endl;
            if (!it->second[i]->getLocationPath().empty())
                std::cout << "  Path: " << it->second[i]->getLocationPath() << std::endl;
            std::cout << "  AutoIndex: " << (it->second[i]->getAutoIndex() ? "on" : "off") << std::endl;
            if (!it->second[i]->getRedirects().empty()) {
                std::cout << "  Redirects: ";
                std::vector<std::pair<std::string, std::string> > redirects = it->second[i]->getRedirects();
                for (size_t j = 0; j < redirects.size(); j++) {
                    std::cout << redirects[j].first << "->" << redirects[j].second;
                    if (j < redirects.size() - 1)
                        std::cout << ", ";
                }
                std::cout << std::endl;
            }
            if (!it->second[i]->getTryFiles().empty()) {
                std::cout << "  Try Files: ";
                std::vector<std::string> tryFiles = it->second[i]->getTryFiles();
                for (size_t j = 0; j < tryFiles.size(); j++) {
                    std::cout << tryFiles[j];
                    if (j < tryFiles.size() - 1)
                        std::cout << ", ";
                }
                std::cout << std::endl;
            }
            std::cout << "  CGI Enabled: " << (it->second[i]->getCgiEnabled() ? "on" : "off") << std::endl;
            if (!it->second[i]->getCgi().empty()) {
                std::cout << "  CGI: ";
                std::vector<std::pair<std::string, std::string> > cgi = it->second[i]->getCgi();
                for (size_t j = 0; j < cgi.size(); j++) {
                    std::cout << cgi[j].first << "->" << cgi[j].second;
                    if (j < cgi.size() - 1)
                        std::cout << ", ";
                }
                std::cout << std::endl;
            }
            if (!it->second[i]->getCgiIncludes().empty()) {
                std::cout << "  CGI Includes: ";
                std::vector<std::string> cgiIncludes = it->second[i]->getCgiIncludes();
                for (size_t j = 0; j < cgiIncludes.size(); j++) {
                    std::cout << cgiIncludes[j];
                    if (j < cgiIncludes.size() - 1)
                        std::cout << ", ";
                }
                std::cout << std::endl;
            }
            if (!it->second[i]->getFastcgiPass().empty())
                std::cout << "  FastCGI Pass: " << it->second[i]->getFastcgiPass() << std::endl;
            if (!it->second[i]->getFastcgiIndex().empty())
                std::cout << "  FastCGI Index: " << it->second[i]->getFastcgiIndex() << std::endl;
            if (!it->second[i]->getFastcgiParams().empty()) {
                std::cout << "  FastCGI Params: ";
                std::vector<std::pair<std::string, std::string> > fastcgiParams = it->second[i]->getFastcgiParams();
                for (size_t j = 0; j < fastcgiParams.size(); j++) {
                    std::cout << fastcgiParams[j].first << "=" << fastcgiParams[j].second;
                    if (j < fastcgiParams.size() - 1)
                        std::cout << ", ";
                }
                std::cout << std::endl;
            }
            locCount++;
        }
    }
    std::cout << std::endl;
}