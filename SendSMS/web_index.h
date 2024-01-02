/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-web-server-multiple-pages
 */



const char *HTML_CONTENT_HOME = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <link rel="icon" href="data:,">
    <title>Home Page</title>
</head>
<body>
    <h1>Welcome to the Tsim Configuration Page</h1>

    <h3>Wifi</h3>
    <form action="/savewifi" method="post">
      <input type="text" name="wifi-ssid" value="ssid">
      <input type="text" name="wifi-password" value="password">
      <button type="submit" name="submitform" class="btn btn-success" value="save">Save</button>
    </form>

    <br>
    <h3>Webhook</h3>
    <form action="/savewebhook" method="post">
      <input type="text" name="webhook-url" value="url">
      <button type="submit" name="submitform" class="btn btn-success" value="save">Save</button>
    </form>


</body>
</html>
)=====";


