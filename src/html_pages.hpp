const char config_html[] = R"rawliteral(
<!DOCTYPE HTML>
<html>
    <head>
        <title>ESP Web Server</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <style>
            html {
                font-family: Arial;
                display: inline-block;
                text-align: center;
            }
            body {
                max-width: 600px;
                margin:0px auto;
                padding-bottom: 25px;
            }
            table {
                margin-left: auto;
                margin-right: auto;
                border: 1px solid black;
                border-collapse: collapse;
            }
        </style>  
        <script src = "https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js"></script>
        <!-- <script src = "//cdn.datatables.net/1.10.23/js/jquery.dataTables.min.js"></script> -->
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
        <div id='wifi_table'></div>
        <script>
            function makeTable(container, data) {
                var table = $("<table/>");
                $.each(data, function(rowIndex, r) {
                    var row = $("<tr/>");
                    $.each(r, function(colIndex, c) { 
                        row.append($("<t"+(rowIndex == 0 ?  "h" : "d")+"/>").text(c));
                    });
                    table.append(row);
                });
                return container.append(table);
            }

            $(document).ready(function () { 
                $.get("/wifi")
                    .done(function(data){
                        makeTable($('#wifi_table'),[['ssid','password']].concat(
                            data.map(function(net){return [net.ssid,net.password]})
                        ))
                    })
                    .fail(function(error){
                        console.log("Error fetching current wifi networks");
                        console.log(error);
                    })
            }); 
        </script>
    </body>
</html>
)rawliteral";