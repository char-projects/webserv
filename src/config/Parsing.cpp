#include "../../includes/parsing/ConfigParsing.hpp"

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
}

bool ConfigParsing::isFileReadable(const std::string &filePath) {
    std::ifstream file(filePath.c_str());
    struct stat s;
    if (stat(filePath.c_str(), &s) == 0 && S_ISDIR(s.st_mode))
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
            LocationConfig location;
            i += 2; // Skip "server" and "{"
            while (i < tokens.size() && tokens[i] != "}") {
                if (tokens[i] == "listen") {
                    if (i + 1 < tokens.size()) {
                        int port = atoi(tokens[i + 1].c_str());
                        server->setPorts(port);
                        i += 2;
                    } else {
                        std::cerr << "Error: Expected port number after 'listen'" << std::endl;
                        i++;
                    }
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
                        location.setRoot(tokens[i + 1]);
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
                            location.addErrorPage(errorPage, errorCodes[k]);
                        }
                        i = j + 1;
                    } else {
                        std::cerr << "Error: Invalid error_page directive" << std::endl;
                        i++;
                    }
                } else if (tokens[i] == "index") {
                    size_t j = i + 1;
                    while (j < tokens.size() && tokens[j].find("index") != std::string::npos) {
                        location.addIndexFile(tokens[j]);
                        j++;
                    }
                    i = j;
                } else if (tokens[i] == "client_max_body_size") {
                    if (i + 1 < tokens.size()) {
                        size_t maxBodySize = static_cast<size_t>(atoi(tokens[i + 1].c_str()));
                        location.setMaxBodySize(maxBodySize);
                        i += 2;
                    } else {
                        std::cerr << "Error: Expected size after 'client_max_body_size'" << std::endl;
                        i++;
                    }
                } else if (tokens[i] == "location") {
                    if (i + 2 < tokens.size() && tokens[i + 2] == "{") {
                        location.setLocationPath(tokens[i + 1]);
                        i += 3; // Skip "location", path, and "{"
                        while (i < tokens.size() && tokens[i] != "}") {
                            if (tokens[i] == "root") {
                                if (i + 1 < tokens.size()) {
                                    location.setRoot(tokens[i + 1]);
                                    i += 2;
                                } else {
                                    std::cerr << "Error: Expected path after 'root'" << std::endl;
                                    i++;
                                }
                            } else if (tokens[i] == "autoindex") {
                                if (i + 1 < tokens.size()) {
                                    location.setAutoIndex(tokens[i + 1]);
                                    i += 2;
                                } else {
                                    std::cerr << "Error: Expected 'on' or 'off' after 'autoindex'" << std::endl;
                                    i++;
                                }
                            } else if (tokens[i] == "try_files") {
                                i++;
                                while (i < tokens.size() && tokens[i] != "}") {
                                    if (tokens[i].find("=") != std::string::npos) {
                                        std::string codeStr = tokens[i].substr(1);
                                        int code = atoi(codeStr.c_str());
                                        std::cout << "Return error code " << code << " if not found" << std::endl;
                                    } else {
                                        location.addTryFile(tokens[i]);
                                    }
                                    i++;
                                }
                            } else if (tokens[i] == "include") {
                                if (i + 1 < tokens.size()) {
                                    location.addCgiInclude(tokens[i + 1]);
                                    i += 2;
                                } else {
                                    std::cerr << "Error: Expected file path after 'include'" << std::endl;
                                    i++;
                                }
                            } else if (tokens[i] == "fastcgi_pass") {
                                if (i + 1 < tokens.size()) {
                                    location.setFastcgiPass(tokens[i + 1]);
                                    i += 2;
                                } else {
                                    std::cerr << "Error: Expected address after 'fastcgi_pass'" << std::endl;
                                    i++;
                                }
                            } else if (tokens[i] == "fastcgi_index") {
                                if (i + 1 < tokens.size()) {
                                    location.setFastcgiIndex(tokens[i + 1]);
                                    i += 2;
                                } else {
                                    std::cerr << "Error: Expected file name after 'fastcgi_index'" << std::endl;
                                    i++;
                                }
                            } else if (tokens[i] == "fastcgi_param") {
                                if (i + 2 < tokens.size()) {
                                    location.addFastcgiParam(tokens[i + 1], tokens[i + 2]);
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
                        server->addLocation(location.getLocationPath());
                        location = LocationConfig(); // Reset for next location
                        if (i < tokens.size() && tokens[i] == "}")
                            i++;
                    } else {
                        std::cerr << "Error: Expected path and '{' after 'location'" << std::endl;
                        i++;
                    }
                } else {
                    std::cerr << "Unknown directive: " << tokens[i] << std::endl;
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
}
