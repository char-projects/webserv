#!/usr/bin/env python3

import os
import time
import cgi
import html

# Usar los mismos nombres que en el servidor C++
SESSION_COOKIE_NAME = 'SESSIONID'
USER_DATA_COOKIE = 'userName'

def get_cookie_value(name):
    cookie_header = os.environ.get('HTTP_COOKIE')
    if not cookie_header:
        return None

    cookies = cookie_header.split('; ')
    for cookie in cookies:
        if cookie.strip().startswith(name + '='):
            return cookie.split('=', 1)[1].strip()
    return None

def generate_new_session_id():
    return str(int(time.time())) + "-websvrsess"

def delete_cookie(name):
    """Genera header para eliminar una cookie"""
    return f"Set-Cookie: {name}=; Expires=Thu, 01 Jan 1970 00:00:00 GMT; Path=/"

# Procesar parámetros del formulario
form = cgi.FieldStorage()
input_name = form.getvalue('user_name', '')
delete_session = form.getvalue('delete_session')
delete_name = form.getvalue('delete_name')

session_id = get_cookie_value(SESSION_COOKIE_NAME)
stored_user_name = get_cookie_value(USER_DATA_COOKIE)

new_session = True
response_cookie_headers = []
message = ""
current_user_name = ""
session_message = ""

# Procesar eliminación de sesión
if delete_session:
    response_cookie_headers.append(delete_cookie(SESSION_COOKIE_NAME))
    session_id = None
    session_message = "🗑️ <strong>Session deleted!</strong>"

# Procesar eliminación del nombre
elif delete_name:
    response_cookie_headers.append(delete_cookie(USER_DATA_COOKIE))
    current_user_name = ""
    message = "🗑️ <strong>Name deleted!</strong>"

# Procesar nuevo nombre
elif input_name:
    current_user_name = html.escape(input_name)
    name_cookie_header = f"Set-Cookie: {USER_DATA_COOKIE}={current_user_name}; Max-Age=3600; Path=/; HttpOnly"
    response_cookie_headers.append(name_cookie_header)
    message = "✅ <strong>Data Stored!</strong> Your name has been saved in a cookie."

# Usar nombre almacenado si existe
elif stored_user_name:
    current_user_name = stored_user_name
    message = "✅ <strong>Data Found!</strong> Welcome back, <strong>{}</strong> (data retrieved from cookie).".format(current_user_name)

# Gestión de sesión
if session_id:
    new_session = False
    if not session_message:  # Solo mostrar mensaje si no estamos eliminando
        session_message = "✅ <strong>Session Found!</strong> Your ID is: <strong>{}</strong>".format(session_id)
else:
    session_id = generate_new_session_id()
    session_cookie_header = f"Set-Cookie: {SESSION_COOKIE_NAME}={session_id}; Max-Age=3600; Path=/; HttpOnly"
    response_cookie_headers.append(session_cookie_header)
    session_message = "⭐ <strong>New Session Created!</strong> Your new ID is: <strong>{}</strong>".format(session_id)

# Determinar color de fondo
bg_color = "#e6ffe6" if not new_session and stored_user_name else "#ffffe6"

# Enviar headers
print("Content-Type: text/html")
for header in response_cookie_headers:
    print(header)
print()  # Línea vacía que separa headers del body

# Generar HTML
html_content = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>CGI Session & Cookie Test</title>
    <style>
        body {{
            font-family: sans-serif;
            margin: 2em;
            background-color: {bg_color};
            color: #333;
        }}
        h1 {{ border-bottom: 2px solid #ccc; padding-bottom: 10px; }}
        h2 {{ color: #2c3e50; }}
        code {{ background-color: #eee; padding: 2px 4px; border-radius: 3px; }}
        .box {{
            margin: 20px 0;
            border: 1px solid #ddd;
            padding: 15px;
            background-color: #f9f9f9;
            border-radius: 5px;
        }}
        .form-group {{ margin-bottom: 15px; }}
        input[type="text"] {{
            padding: 8px;
            border: 1px solid #ccc;
            width: 250px;
            border-radius: 3px;
        }}
        input[type="submit"] {{
            padding: 8px 15px;
            background-color: #007bff;
            color: white;
            border: none;
            cursor: pointer;
            border-radius: 3px;
            margin: 5px;
        }}
        .danger {{ background-color: #dc3545 !important; }}
        .success {{ background-color: #28a745 !important; }}
        .info {{ background-color: #17a2b8 !important; }}
        .cookie-info {{
            background-color: #f8f9fa;
            padding: 10px;
            border-left: 4px solid #007bff;
            margin: 10px 0;
        }}
    </style>
</head>
<body>
    <h1>🔐 CGI Session & Cookie Test</h1>

    <div class="box">
        <h2>Session ID Persistence Test</h2>
        <p style="font-size: 1.1em;">{session_message}</p>
        <p>Refresh the page (<strong>F5</strong>) to confirm the session ID persists.</p>

        <div class="form-group">
            <form method="get" action="{script_path}">
                <input type="hidden" name="delete_session" value="1">
                <input type="submit" value="🗑️ Delete Session" class="danger">
            </form>
        </div>
    </div>

    <div class="box">
        <h2>Data Persistence (Form & Cookie) Test</h2>
        <p style="font-size: 1.1em;">{message}</p>

        <div class="form-group">
            <form method="get" action="{script_path}">
                <label for="user_name"><strong>Enter your name:</strong></label><br>
                <input type="text" id="user_name" name="user_name" value="{user_name_value}" placeholder="e.g., Jane Doe"><br>
                <input type="submit" value="💾 Save Name" class="success">
            </form>
        </div>

        {delete_name_button}
    </div>

    <div class="box">
        <h2>🔧 Cookie Information</h2>
        <div class="cookie-info">
            <p><strong>Received from Client:</strong><br>
            <code>HTTP_COOKIE</code> = <code>{cookie_received}</code></p>
        </div>
        <div class="cookie-info">
            <p><strong>Sent to Client:</strong><br>
            <code>Set-Cookie</code> Headers = <pre>{cookie_sent}</pre></p>
        </div>
    </div>

    <div class="box">
        <h2>🔄 Actions</h2>
        <p>
            <a href="{script_path}"><button class="info">🔄 Refresh Page</button></a>
            <a href="/"><button class="info">🏠 Go to Home</button></a>
        </p>
    </div>
</body>
</html>
""".format(
    session_message=session_message,
    message=message,
    user_name_value=current_user_name,
    script_path='/cgi-bin/cookies.py',
    cookie_received=html.escape(os.environ.get('HTTP_COOKIE', 'No cookie received from client')),
    cookie_sent=html.escape('\n'.join(response_cookie_headers) or 'No Set-Cookie sent in this response.'),
    bg_color=bg_color,
    delete_name_button='<form method="get" action="{}"><input type="hidden" name="delete_name" value="1"><input type="submit" value="🗑️ Delete Name" class="danger"></form>'.format('/cgi-bin/cookies.py') if stored_user_name else ''
)

print(html_content)
