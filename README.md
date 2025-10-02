# Webserv

📋 Requirements for Webserv Project

### Program & Execution
- [✅] Executable should be executed as: ./webserv [configuration file]
- [✅] The HTTP 1.0 is suggested as a reference point, but not enforced.
- [✅] Your program must use a configuration file, provided as an argument on the command line, or available in a default path.
- [✅] You cannot execve another web server.
- [✅] Your server must remain non-blocking at all times and properly handle client disconnections when necessary.
- [✅] It must be non-blocking and use only 1 poll() (or equivalent) for all the I/O operations between the clients and the server (listen included).
- [👀] poll() (or equivalent) must monitor both reading and writing simultaneously.
- [👀] You must never do a read or a write operation without going through poll() (or equivalent).
- [✅] Checking the value of errno to adjust the server behaviour is strictly forbidden after performing a read or write operation.
- [✅] A request to your server should never hang indefinitely.
- [✅] Your server must be compatible with standard web browsers of your choice.
- [✅] Your HTTP response status codes must be accurate.
- [✅] Your server must have default error pages if none are provided.
- [❌] You can’t use fork for anything other than CGI (like PHP, or Python, and so forth).
- [✅] You must be able to serve a fully static website.
- [✅] Clients must be able to upload files.
- [✅] You need at least the GET, POST, and DELETE methods.
- [👀] Stress test your server to ensure it remains available at all times.
- [👀] Your server must be able to listen to multiple ports to deliver different content.
- [✅] You are allowed to use fcntl() only with the following flags: F_SETFL, O_NONBLOCK, FD_CLOEXEC.

### Configuration File
- [✅] Define all the interface:port pairs on which your server will listen to.
- [✅] Set up default error pages.
- [✅] Set the maximum allowed size for client request bodies.
- [✅] Specify rules/configurations on a URL/route
- [✅] List of accepted HTTP methods for the route.
- [✅] HTTP redirection.
- [✅] Directory mapping (root directive).
- [✅] Enabling or disabling directory listing.
- [✅] Default file to serve when the resource is a directory.
- [✅] Uploading files from the clients (with storage location).
- [❌] Execution of CGI based on file extension (php, python, etc.).

### Bonus Features
- [❌] "Support cookies and session management."
- [❌] "Handle multiple CGI types."

## ERRORS
- [ ] Crash con archivos muy grandes

## TODO
- [ ] Disable DEGUB logger from the Makefile
- [ ] Comprobar permisos a la hora de subir archivos
- [ ] Cuando hay error al parsear el default.com continue; ????
- [ ] Problems al usar HTTPS://
- [ ] Comprobar que no se puede usar DELETE en carpetas que no tienen permitido el método
- [ ] Limpiar estructura addr antes de configurar
- [ ] HTTP/1.1 keep-alive OJO!!!! leer cabecera request
- [ ] BUFFER_SIZE no debe ser un tamaño fijo??, leer cabecera request
- [ ] Hacer la parte "NULL" de excepciones del select (select(max_fd + 1, &read_fds, &write_fds, NULL, &timeout);)
- [ ] Simplificar ResponseHeader::setContent(size_t status_code)
- [ ] Formalizar la forma en que se muestran mensajes de error, etc ...
- [ ] Comprobar
	- curl --resolve server42.com:80:127.0.0.1 http://server42.com/
	- curl -X POST -H "Content-Type: plain/text" --data "BODY IS HERE write something shorter or longer than body limit"

## TO_CHECK
- perform tests / stress
- telnet, curl

---

### 📑 RECURSOS
>> ⚡ Diferentes versiones
> - https://www.webperformance.es/la-version-de-http/
>> ⚡ HTTP/1.0 – 1996 (RFC 1945)
> - https://datatracker.ietf.org/doc/html/rfc1945
>> ⚡ RFC 7231 - HTTP/1.1 Semantics
> - https://datatracker.ietf.org/doc/html/rfc7231
>> ⚡ RFC 3875 - Common Gateway Interface CGI
> - https://datatracker.ietf.org/doc/html/rfc3875
> - https://en.wikipedia.org/wiki/Common_Gateway_Interface
>> ⚡ RFC 1867 - Form-based File Upload
> - https://datatracker.ietf.org/doc/html/rfc1867
>> ⚡ Example: Nonblocking I/O and select()
> - https://www.ibm.com/docs/en/i/7.2.0?topic=designs-example-nonblocking-io-select
> - https://csresources.github.io/SystemProgrammingWiki/SystemProgramming/Networking,-Part-7:-Nonblocking-I-O,-select(),-and-epoll/
> - https://en.wikipedia.org/wiki/Select_(Unix)
> - https://man7.org/linux/man-pages/man2/select.2.html
>> ⚡ HTML
> - https://es.wikipedia.org/wiki/Protocolo_de_transferencia_de_hipertexto
>> ⚡ Códigos de estado
> - https://es.wikipedia.org/wiki/Anexo:C%C3%B3digos_de_estado_HTTP
>> ⚡ Para testear el protocolo
> - https://blog.baens.net/posts/http-with-telnet/
>> ⚡ Nginx Configuration File Structure and Configuration Contexts
> - https://www.digitalocean.com/community/tutorials/understanding-the-nginx-configuration-file-structure-and-configuration-contexts
> - https://dev.to/takahiro_82jp/what-types-of-nginx-configuration-files-exist-3o72


## PROOFS

// Postear JSON
curl -X POST --header 'Content-Type: application/json' -d '{"thing": 45}' localhost:8080/anything

// Borrar archivo
curl -X DELETE localhost:8080/anything

// Postear XML
curl -X POST -H 'Content-Type: application/xml' -H 'Accept: application/xml' -d '<Person>
    <FirstName>Joe</FirstName>
    <LastName>Soap</LastName>
</Person>' localhost:8080/anything

// Postear como PARAMETROS
curl -X POST -F FirstName=Joe -F LastName=Soap localhost:8080/anything

// Subir archivo
curl -X POST -F File=@Makefile localhost:8080/www/uploads/Makefile2
<html><body><h1>File Upload Results</h1><ul><li>File 'File' uploaded successfully as: File_upload_1759157390.dat</li></ul></body></html>
