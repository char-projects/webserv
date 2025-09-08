# Webserv

## OBLIGATORIO
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

- ........... !!! REVISAR SUBJECT ¡¡¡


## OBLIGATORIO (ARCHIVO DE CONFIGURACIÓN)
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


## OBLIGATORIO (CGI)
- [ ] Los argumentos en las peticiones se deben pasar al CGI
- [ ] Las solicitudes fragmentadas deben unirse antes de procesarse
- [ ] Lo mismo para las respuestas
- [ ] Debe correr en el directorio correcto dependiendo del la ruta relativa
- [ ] Debe soportar al menos un lenguaje (PHP, python, ...)

- ........... !!! REVISAR SUBJECT ¡¡¡

---


## Cosas que NO hay que hacer

- Mantener sesiones (cookies)

---

## PREGUNTAS

- Como no es obligatorio usar la misma estructura que en NGINX, usar algo como JSON, XML, yaml ???
- Y qué hay de las web dinámicas ??

---

## SUGERENCIAS

- Usar tests tanto de funcionamiento como de stress

---


## RECURSOS

- ### epoll - I/O event notification facility
https://man7.org/linux/man-pages/man7/epoll.7.html
- ### Protocolo
https://es.wikipedia.org/wiki/Protocolo_de_transferencia_de_hipertexto
- ### Diferentes versiones
https://www.webperformance.es/la-version-de-http/
- ### [!] HTTP/1.0 – 1996 (RFC 1945)
https://datatracker.ietf.org/doc/html/rfc1945
- ### Códigos de estado
https://es.wikipedia.org/wiki/Anexo:C%C3%B3digos_de_estado_HTTP
- ### CGI
https://en.wikipedia.org/wiki/Common_Gateway_Interface
- ### Para testear el protocolo
https://blog.baens.net/posts/http-with-telnet/
