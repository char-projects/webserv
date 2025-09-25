# Webserv

## ❗ OBLIGATORIO
- [ ] El programa debe obtener la configuración de un archivo, con la posibilidad de pasarle como argumento el ruta del archivo **./webserv [configuration file]**
- [ ] Servidor **no bloqueante**, lo que quiere decir que debe responder varias peticiones "al mismo tiempo"
- [ ] 1 Solo poll para entrada / salida (select(), kqueue(), or epoll())
- [ ] Poner algún tiempo límite para las respuestas
- [ ] Códigos de estado precisos
- [ ] Página de error por defecto (típico not found, etc)
- [ ] Servir web estáticas con enlaces y demás
- [ ] Subir archivos
- [ ] Implementar al menos los métodos **GET, POST y DELETE**.
- [ ] El servidor debe poder escuchar desde múltiples puertos (ex: 80, 8080, ...)
- [ ] Proporcionar páginas y archivos de configuración para demostrar que todo lo que se pide funciona
- [ ] Todo I/O debe pasar por select()
- [ ] Solo debe haber una lectura o una escritura por cliente por select()
- ........... !!! REVISAR SUBJECT ¡¡¡


## ❗ OBLIGATORIO (ARCHIVO DE CONFIGURACIÓN)
- [ ] Definición de múltiples puertos
- [ ] Ruta del HTML de errores
- [ ] Tamaño máximo de las respuestas del cliente
- [ ] Listado de métodos aceptados por el servidor
- [ ] Redirecciones
- [ ] Ruta donde se aloja la web
- [ ] Habilitar o deshabilitar el listado de directorios
- [ ] Archivo por entregado por defecto cuando la petición es un directorio
- [ ] Si el cliente puede o no alojar archivos en el servidor y dar el directorio donde se aloja
- ........... !!! REVISAR SUBJECT ¡¡¡


## ❗ OBLIGATORIO (CGI)
- [ ] Los argumentos en las peticiones se deben pasar al CGI
- [ ] Las solicitudes fragmentadas deben unirse antes de procesarse
- [ ] Lo mismo para las respuestas
- [ ] Debe correr en el directorio correcto dependiendo del la ruta relativa
- [ ] Debe soportar al menos un lenguaje (PHP, python, ...)
- ........... !!! REVISAR SUBJECT ¡¡¡

---

## ❌ Cosas que NO hay que hacer
- Mantener sesiones (cookies) ??

---

## ❔ PREGUNTAS
- Como no es obligatorio usar la misma estructura que en NGINX, usar algo como JSON, XML, yaml ???
- Y qué hay de las web dinámicas ??
- HTTP/1.1 o HTTP/1.0 ¿?
- poll() <poll.h> o select() <sys/select.h> o epoll() <sys/epoll.h>
- Contenedores a usar?
- Bonus sí o no?
- CHECK SIZE DEL RESPONSE?, CHUNK RESPONSE??

---

## ⌛ SUGERENCIAS
- Usar tests tanto de funcionamiento como de stress
- Particionar en carpetas, cada una con su TODO.md / main / Makefile para trabajar y testear
	y mostrar avances, por separado y luego unificar ?
- El que termina que empiece con la web de pruebas
- Conseguir la hoja de corrección

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


## TODO

- [ ] Problems al usar HTTPS://
- [ ] Comprobar que no se puede usar DELETE en carpetas que no tienen permitido el método
- [ ] Limpiar estructura addr antes de configurar
- [ ] HTTP/1.1 keep-alive OJO!!!! leer cabecera request
- [ ] BUFFER_SIZE no debe ser un tamaño fijo??, leer cabecera request
- [ ] Hacer la parte "NULL" de excepciones del select (select(max_fd + 1, &read_fds, &write_fds, NULL, &timeout);)
- [ ] Simplificar ResponseHeader::setContent(size_t status_code)

TOSEE:
- [ ] NULL (bloqueo indefinido)
- [ ] con default.conf soporta comentarios?? #

RESOURCES:
- https://www.transparentedge.eu/blog/transfer-encoding-chunked/

ADD:

	en default.conf
		- allow_methods o methods
		- upload_path
		- Validar rango de puertos validos del 0 al 65535.
		- map<string, string> redirects;
			location = /content/algo {
				return 301 /new-name/newalgo;
			}

	REQUEST
		- request.getParams()
		- request.getRedirects()
		- status_code = 200; Por defecto

- Transfer-Encoding: chunked
Decodificar el body leyendo “tamaño en hex + CRLF + datos + CRLF … 0\r\n\r\n”


		HEADER
		- Content-Type: application/x-www-form-urlencoded  == POST FORMULARIO
			Ex:

POST /formulario HTTP/1.1
Content-Type: application/x-www-form-urlencoded
Content-Length: 31

nombre=Juan&apellido=Perez&edad=25

		- Content-Type: multipart/form-data == POST ARCHIVO

			Ex:

POST /upload HTTP/1.1
Content-Type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW

------WebKitFormBoundary7MA4YWxkTrZu0gW
Content-Disposition: form-data; name="nombre"

Juan
------WebKitFormBoundary7MA4YWxkTrZu0gW
Content-Disposition: form-data; name="archivo"; filename="documento.pdf"
Content-Type: application/pdf

[contenido binario del archivo]
------WebKitFormBoundary7MA4YWxkTrZu0gW--

------------------

POST /test.html HTTP/1.1
Host: example.org
Content-Type: multipart/form-data;boundary="boundary"

--boundary
Content-Disposition: form-data; name="field1"

value1
--boundary
Content-Disposition: form-data; name="field2"; filename="example.txt"

value2



	SERVERCONFIG
		- vector<LocationConfig *> locations;

	LocationConfig.cpp
		maxBodySize = 0 ---> maxBodySize = MAX_BODY_SIZE

*/
