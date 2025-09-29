# Webserv

## ❗ OBLIGATORIO
- [X] El programa debe obtener la configuración de un archivo, con la posibilidad de pasarle como argumento el ruta del archivo **./webserv [configuration file]**
- [X] Servidor **no bloqueante**, lo que quiere decir que debe responder varias peticiones "al mismo tiempo"
- [X] 1 Solo poll para entrada / salida (select(), kqueue(), or epoll())
- [X] Poner algún tiempo límite para las respuestas
- [X] Códigos de estado precisos
- [X] Página de error por defecto (típico not found, etc)
- [X] Servir web estáticas con enlaces y demás
- [ ] Subir archivos
- [ ] Implementar al menos los métodos **GET, POST y DELETE**.
- [X] El servidor debe poder escuchar desde múltiples puertos (ex: 80, 8080, ...)
- [X] Proporcionar páginas y archivos de configuración para demostrar que todo lo que se pide funciona
- [ ] Todo I/O debe pasar por select()
- [ ] Solo debe haber una lectura o una escritura por cliente por select()
- ........... !!! REVISAR SUBJECT ¡¡¡


## ❗ OBLIGATORIO (ARCHIVO DE CONFIGURACIÓN)
- [X] Definición de múltiples puertos
- [X] Ruta del HTML de errores
- [X] Tamaño máximo de las respuestas del cliente
- [X] Listado de métodos aceptados por el servidor
- [X] Redirecciones
- [X] Ruta donde se aloja la web
- [X] Habilitar o deshabilitar el listado de directorios
- [X] Archivo entregado por defecto cuando la petición es un directorio
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
- Mantener sesiones (cookies) ?? BONUS
- CGI varios BONUS

---

## ❔ PREGUNTAS
- Bonus sí o no?

---

## ⌛ SUGERENCIAS
- Usar tests tanto de funcionamiento como de stress
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


## ERRORES

- [ ] Webs de error, 
- [ ] En chrome los errores fugan código
- [ ] No upload en la carpeta apropiada
- [ ] no funciona el server42.com


## TODO

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

pro
TOSEE:
- [ ] NULL (bloqueo indefinido)

RESOURCES:
- https://www.transparentedge.eu/blog/transfer-encoding-chunked/

ADD:

	en default.conf
		- upload_path

	REQUEST
```
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

```

```

POST /test.html HTTP/1.1
Host: example.org
Content-Type: multipart/form-data;boundary="boundary"

--boundary
Content-Disposition: form-data; name="field1"

value1
--boundary
Content-Disposition: form-data; name="field2"; filename="example.txt"

value2 ```

