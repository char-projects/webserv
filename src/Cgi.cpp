#include "../includes/Cgi.hpp"
#include "../includes/utils.hpp"

Cgi::Cgi(const std::string &scriptPath, const std::map<std::string, std::string> &env)
    : scriptPath(scriptPath), env(env), output(""), postData(""), status(0) {}

Cgi::Cgi(const Cgi &other)
    : scriptPath(other.scriptPath), env(other.env), output(other.output), postData(other.postData), status(other.status) {}

Cgi &Cgi::operator=(const Cgi &other) {
    if (this != &other) {
        scriptPath = other.scriptPath;
        env = other.env;
        output = other.output;
        postData = other.postData;
        status = other.status;
    }
    return *this;
}

Cgi::~Cgi() {}

bool Cgi::execute() {
    int inPipe[2];
    int outPipe[2];
    pid_t pid;

    if (pipe(inPipe) == -1 || pipe(outPipe) == -1) {
        logger(STDOUT_FILENO, ERROR, "Failed to create pipes for CGI");
        return false;
    }
    pid = fork();
    if (pid < 0) {
        logger(STDOUT_FILENO, ERROR, "Failed to fork process for CGI");
        return false;
    }
    if (pid == 0) { // Child process
        close(inPipe[1]);
        close(outPipe[0]);

        dup2(inPipe[0], STDIN_FILENO);
        dup2(outPipe[1], STDOUT_FILENO);
        close(inPipe[0]);
        close(outPipe[1]);

        // Change to script directory
        size_t lastSlash = scriptPath.rfind('/');
        if (lastSlash != std::string::npos) {
            std::string dir = scriptPath.substr(0, lastSlash);
            if (chdir(dir.c_str()) != 0)
                logger(STDOUT_FILENO, ERROR, "Failed to change directory for CGI: " + dir);
        }

        std::vector<char*> envp;
        for (std::map<std::string, std::string>::const_iterator it = env.begin(); it != env.end(); ++it) {
            std::string envVar = it->first + "=" + it->second;
            envp.push_back(strdup(envVar.c_str()));
        }
        envp.push_back(NULL);

        // Get interpreter and set up arguments
        std::string interpreter = getInterpreter();
        std::vector<char*> argv;
        
        if (!interpreter.empty()) {
            argv.push_back(strdup(interpreter.c_str()));
            argv.push_back(strdup(scriptPath.c_str()));
        } else {
            argv.push_back(strdup(scriptPath.c_str()));
        }
        argv.push_back(NULL);

        if (!interpreter.empty()) {
            if (execve(("/usr/bin/" + interpreter).c_str(), argv.data(), envp.data()) == -1) {
                if (execve(("/bin/" + interpreter).c_str(), argv.data(), envp.data()) == -1) {
                    logger(STDOUT_FILENO, ERROR, "Failed to execute CGI interpreter: " + interpreter);
                    exit(1);
                }
            }
        } else {
            if (execve(scriptPath.c_str(), argv.data(), envp.data()) == -1) {
                logger(STDOUT_FILENO, ERROR, "Failed to execute CGI script: " + scriptPath);
                exit(1);
            }
        }
    } else { // Parent process
        close(inPipe[0]);
        close(outPipe[1]);
        // Write POST data to CGI stdin if available
        if (!postData.empty())
            write(inPipe[1], postData.c_str(), postData.size());
        close(inPipe[1]);

        char buffer[4096];
        ssize_t bytesRead;
        output.clear();
        while ((bytesRead = read(outPipe[0], buffer, sizeof(buffer))) > 0) {
            output.append(buffer, bytesRead);
        }
        close(outPipe[0]);

        int wstatus;
        waitpid(pid, &wstatus, 0);
        if (WIFEXITED(wstatus)) {
            status = WEXITSTATUS(wstatus);
            return status == 0;
        } else {
            logger(STDOUT_FILENO, ERROR, "CGI script did not terminate normally");
            return false;
        }
    }
    return true;
}

std::string Cgi::getOutput() const {
    return output;
}

int Cgi::getStatus() const {
    return status;
}

int Cgi::checkExtension() {
    size_t dotPos = scriptPath.rfind('.');
    if (dotPos == std::string::npos)
        return -1; // No extension found
    std::string ext = scriptPath.substr(dotPos);
    if (ext == ".php")
        return (std::system("php --version > /dev/null 2>&1") == 0) ? 0 : -1;
    else if (ext == ".py")
        return (std::system("python3 --version > /dev/null 2>&1") == 0) ? 0 : -1;
    else if (ext == ".sh")
        return (std::system("bash --version > /dev/null 2>&1") == 0) ? 0 : -1;
    return -1; // Unsupported extension
}

void Cgi::handleCookies(std::map<std::string, std::string> &headers) {
    size_t pos = 0;
    while ((pos = output.find("Set-Cookie:", pos)) != std::string::npos) {
        size_t endPos = output.find("\r\n", pos);
        if (endPos == std::string::npos)
            break;
        std::string cookie = output.substr(pos + 11, endPos - (pos + 11));
        headers["Set-Cookie"] = cookie; // Overwrite previous cookies
        pos = endPos + 2;
    }
}

void Cgi::setPostData(const std::string &data) {
    postData = data;
}

void Cgi::setupEnvironment(const std::string &method, const std::string &uri, 
                         const std::string &queryString, const std::string &contentType,
                         size_t contentLength, const std::string &serverName, 
                         const std::string &serverPort, const std::map<std::string, std::string> &headers) {
    
    // Basic CGI environment variables
    env["REQUEST_METHOD"] = method;
    env["REQUEST_URI"] = uri;
    env["QUERY_STRING"] = queryString;
    env["CONTENT_TYPE"] = contentType;
    env["CONTENT_LENGTH"] = stringify(contentLength);
    env["SERVER_NAME"] = serverName;
    env["SERVER_PORT"] = serverPort;
    env["SERVER_SOFTWARE"] = "webserv/1.0";
    env["GATEWAY_INTERFACE"] = "CGI/1.1";
    env["SERVER_PROTOCOL"] = "HTTP/1.1";
    
    // Script information
    size_t lastSlash = scriptPath.rfind('/');
    if (lastSlash != std::string::npos) {
        env["SCRIPT_NAME"] = scriptPath.substr(lastSlash);
        env["SCRIPT_FILENAME"] = scriptPath;
        env["DOCUMENT_ROOT"] = scriptPath.substr(0, lastSlash);
    } else {
        env["SCRIPT_NAME"] = scriptPath;
        env["SCRIPT_FILENAME"] = scriptPath;
        env["DOCUMENT_ROOT"] = ".";
    }
    
    // Path info (part of URI after script name)
    size_t scriptPos = uri.find(env["SCRIPT_NAME"]);
    if (scriptPos != std::string::npos) {
        std::string pathInfo = uri.substr(scriptPos + env["SCRIPT_NAME"].length());
        if (!pathInfo.empty() && pathInfo[0] == '/') {
            env["PATH_INFO"] = pathInfo;
            env["PATH_TRANSLATED"] = env["DOCUMENT_ROOT"] + pathInfo;
        }
    }
    
    // HTTP headers as environment variables
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        std::string headerName = "HTTP_" + it->first;
        // Convert to uppercase and replace - with _
        for (size_t i = 0; i < headerName.length(); ++i) {
            if (headerName[i] == '-')
                headerName[i] = '_';
            else
                headerName[i] = std::toupper(headerName[i]);
        }
        env[headerName] = it->second;
    }
}

std::string Cgi::getInterpreter() const {
    size_t dotPos = scriptPath.rfind('.');
    if (dotPos == std::string::npos)
        return "";
    std::string ext = scriptPath.substr(dotPos);
    if (ext == ".php")
        return "php";
    else if (ext == ".py")
        return "python3";
    else if (ext == ".sh")
        return "bash";
    return "";
}

std::string Cgi::parseHeaders(std::string &body) {
    std::string headers;
    size_t headerEnd = output.find("\r\n\r\n");
    if (headerEnd != std::string::npos) {
        headers = output.substr(0, headerEnd);
        body = output.substr(headerEnd + 4);
    } else {
        // No headers found, treat entire output as body
        headers = "";
        body = output;
    }
    return headers;
}