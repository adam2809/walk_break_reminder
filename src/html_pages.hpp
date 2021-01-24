const char config_html[] = R"rawliteral(
<!DOCTYPE HTML>
<html>
    <head>
        <title>ESP Web Server</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <style>
            html {font-family: Arial; display: inline-block; text-align: center;}
            body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
            table {text-align: center;}
        </style>  
        <script src = "https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js"></script>
    </head>
    <body>
        <h2>ESP Strava Tracker config</h2>

        <h3>Current wifi network:<h3>
        %CURRENT_WIFI_SSID%

        <form action="/wifi">
            <label for="ssid">Ssid</label><br>
            <input type="text" id="ssid" name="ssid"><br>
            <label for="password">Password:</label><br>
            <input type="text" id="password" name="password"><br><br>
            <input type="submit" value="Add network">
        </form>
        <h3>Saved wifi networks:</h3>
        <table border=1 frame=void rules=rows>
            <thead>
                <tr> 
                    <th>SSID</th>
                    <th>Password</th>
                </tr> 
            </thead>
            <tbody id='table_body'>
                <script> 
                    $(document).ready(function () { 
                        $.get("/wifi")
                            .done(function(data){
                                data['wifi'].forEach(network => {
                                    console.log(network);
                                    var newRow = '';
                                    newRow += '</tr>';
                                    newRow += '<td>' +  network.ssid + '</td>'; 
                                    newRow += '<td>' + network.password + '</td>';
                                    newRow += '</tr>';
                                    console.log(newRow);
                                    $('#table_body').append(newRow);
                                });
                            })
                            .fail(function(error){
                                console.log("Error fetching current wifi networks");
                                console.log(error);
                            })
                    }); 
                </script>
            </tbody>
        </table>
    </body>
</html>
)rawliteral";