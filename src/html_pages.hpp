const char config_html[] = R"rawliteral(
<!DOCTYPE HTML>
<html>
    <head>
        <title>ESP Web Server</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <link rel="icon" href="data:,">
        <style>
            html {font-family: Arial; display: inline-block; text-align: center;}
            body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
        </style>
    </head>
    <body>
        <h2>ESP Strava Tracker config</h2>

        <h3>Current wifi network:<h3>
        %CURRENT_WIFI_SSID%

        <form action="/wifi">
            <label for="ssid">First name:</label><br>
            <input type="text" id="ssid" name="ssid" value="John"><br>
            <label for="password">Last name:</label><br>
            <input type="text" id="password" name="password" value="Doe"><br><br>
            <input type="submit" value="Add network">
        </form>
        <h3>Saved wifi networks:<h3>
    </body>
</html>
)rawliteral";