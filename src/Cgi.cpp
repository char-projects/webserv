#include "../includes/Cgi.hpp"
#include "../includes/Utils.hpp"

/*
Execution of CGI, based on file extension (for example .php). Here are some
specific remarks regarding CGIs:
∗ Have a careful look at the environment variables involved in the web
server-CGI communication. The full request and arguments provided by
the client must be available to the CGI.
∗ Just remember that, for chunked requests, your server needs to un-chunk
them, the CGI will expect EOF as the end of the body.
∗ The same applies to the output of the CGI. If no content_length is
returned from the CGI, EOF will mark the end of the returned data.
∗ The CGI should be run in the correct directory for relative path file access.
∗ Your server should support at least one CGI (php-CGI, Python, and so
forth).
*/

Cgi::Cgi(const std::string &scriptPath, const std::map<std::string, std::string> &env)
    : scriptPath(scriptPath), env(env), output(""), status(0) {}

Cgi::Cgi(const Cgi &other)
    : scriptPath(other.scriptPath), env(other.env), output(other.output), status(other.status) {}

Cgi &Cgi::operator=(const Cgi &other) {
    if (this != &other) {
        scriptPath = other.scriptPath;
        env = other.env;
        output = other.output;
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

        std::vector<char*> envp;
        for (std::map<std::string, std::string>::const_iterator it = env.begin(); it != env.end(); ++it) {
            std::string envVar = it->first + "=" + it->second;
            envp.push_back(strdup(envVar.c_str()));
        }
        envp.push_back(NULL);

        char *argv[] = {strdup(scriptPath.c_str()), NULL};
        if (execve(scriptPath.c_str(), argv, envp.data()) == -1) {
            logger(STDOUT_FILENO, ERROR, "Failed to execute CGI script: " + scriptPath);
            return false;
        }
    } else { // Parent process
        close(inPipe[0]);
        close(outPipe[1]);

        // Optionally write to CGI stdin
        // write(inPipe[1], inputData.c_str(), inputData.size());
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
