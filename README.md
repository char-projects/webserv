# Webserv

A basic HTTP server written in C++ that implements the `GET`, `POST`, and `DELETE` HTTP methods

## Features

- Fully non-blocking using a single `select()` loop for all socket I/O  
- Parses HTTP requests and generates responses  
- Serves static files from a configurable root directory  
- Handles CGI scripts for dynamic content  
- Supports file uploads via `POST`  
- Default error pages and basic cookie handling  
- Configured via a `.conf` file
- Cookie and session support
- Multiple CGI types
- Debug mode (make debug-on / make debug-off)
- Support for virtual directories
- A static website to test everything

## Usage

#### Clone the repository:
```bash
git clone https://github.com/char-projects/webserv
cd webserv
```

#### Run the server with a configuration file:

```bash
./webserv configs/default.conf
```

#### Open it in the browser:

Website: http://localhost:8080

Cookie test: http://localhost:8080/cgi-bin/cookies.py

## Configuration File

(in case you don't want to use the ones provided in configs/)
- Define ports and interfaces the server listens on
- Set maximum client request body size
- Specify default error pages
- Map directories (root directive)
- Enable/disable directory listing
- Define default files for directories
- Configure CGI execution based on file extension
- Define URL/route-specific rules, allowed methods, and HTTP redirections
- Specify server name for virtual hosts
- Configure upload storage location

## Terminal requests

Access the homepage:
```bash
curl http://localhost:8080
```

Upload a file:
```bash
curl -F "file=@example.txt" http://localhost:8080/upload
```
Run a CGI script:
```bash
curl http://localhost:8080/cgi-bin/test.py
```

Post JSON:
```bash
curl -X POST -H 'Content-Type: application/json' -d '{"thing":45}' http://localhost:8080/upload
```

Post XML:
```bash
curl -X POST -H 'Content-Type: application/xml' -d '<Person><FirstName>Joe</FirstName><LastName>Soap</LastName></Person>' http://localhost:8080/anything
```

Post as form parameters:
```bash
curl -X POST -F FirstName=Joe -F LastName=Soap http://localhost:8080/anything
```

Delete file:
```bash
curl -X DELETE http://localhost:8080/anything
```
