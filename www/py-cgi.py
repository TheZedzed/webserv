import time

header = "Content-type: text/html\n\r"

date_string = time.strftime('%A, %B %d, %Y at %I:%M:%S %p %Z')

body = """
\r\n<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge" />
  <title>Current date</title>
</head>
<body>
  <p>Python CGI is working</p>
  <object type="image/svg+xml" data="python-cgi.svg"></object>
  <p>Current date: """ + date_string + """</p>
</body>
</html>
"""
print(header + body)
