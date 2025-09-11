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
>> ⚡ epoll - I/O event notification facility
> - https://man7.org/linux/man-pages/man7/epoll.7.html
>> ⚡ HTML
> - https://es.wikipedia.org/wiki/Protocolo_de_transferencia_de_hipertexto
>> ⚡ Códigos de estado
> - https://es.wikipedia.org/wiki/Anexo:C%C3%B3digos_de_estado_HTTP
>> ⚡ Para testear el protocolo
> - https://blog.baens.net/posts/http-with-telnet/
>> ⚡ Nginx Configuration File Structure and Configuration Contexts
> - https://www.digitalocean.com/community/tutorials/understanding-the-nginx-configuration-file-structure-and-configuration-contexts
> - https://dev.to/takahiro_82jp/what-types-of-nginx-configuration-files-exist-3o72