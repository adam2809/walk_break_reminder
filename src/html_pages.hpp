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

        <h3>Current wifi network:</h3>
        <h4>%CURRENT_WIFI_SSID%</h4>

        <h3>Add new wifi network:</h3>
        <form id='new_wifi_form'>
            <script>
                function submitNewWiFiForm(){
                    console.log('clicked button');
                    var formData = {};
                    $('#new_wifi_form').serializeArray().forEach(function(formRow){
                        formData[formRow['name']] = formRow['value']
                    });

                    $.ajax({
                        url:'/wifi',
                        type:'POST',
                        contentType:'application/json',
                        data:JSON.stringify(formData),
                        success: function(data) {
                            fillTableWithWiFiArray(data);
                        },
                        failure: function(data) {
                            console.log("Failed POST");
                        }
                    });
                };
            </script>
            <label for="ssid">Ssid</label><br>
            <input type="text" id="ssid" name="ssid"><br>
            <label for="password">Password:</label><br>
            <input type="text" id="password" name="password"><br><br>
            <input type="button" value="Add network" onClick='submitNewWiFiForm()'>
        </form>
        <h3>Saved wifi networks:</h3>
        <div id='wifi_table'>
            <script>
                function makeTable(container, data) {
                    var table = $("<table/>");
                    $.each(data, function(rowIndex, r) {
                        var row = $("<tr/>");
                        $.each(r, function(colIndex, c) { 
                            row.append($("<t"+(rowIndex == 0 ?  "h" : "d")+"/>").text(c));
                        });
                        if(rowIndex!=0) {
                            const deleteCol = $("<td/>")
                            const deleteButton = $("<button/>")
                            deleteButton.attr('onClick',`deleteWifi('${r[0]}')`);
                            deleteButton.attr('type',"button");
                            deleteCol.append(deleteButton);
                            row.append(deleteCol)
                        }
                        table.append(row);
                    });
                    return container.append(table);
                }

                function deleteWifi(ssid){
                    $.ajax({
                        url:`/wifi?ssid=${ssid}`,
                        type:'DELETE',
                        success: function(data) {
                            fillTableWithWiFiArray(data);
                        },
                        failure: function(data) {
                            console.log("Failed DELETE");
                        }
                    })
                }

                function fillTableWithWiFiArray(wifiArr){
                    $('#wifi_table').empty();
                    makeTable($('#wifi_table'),[['ssid','password']].concat(
                        wifiArr.map(function(net){return [net.ssid,net.password]})
                    ))
                }
    
                $(document).ready(function () { 
                    $.get("/wifi")
                        .done(function(data){
                            fillTableWithWiFiArray(data);
                        })
                        .fail(function(error){
                            console.log("Error fetching current wifi networks");
                            console.log(error);
                        })
                }); 
            </script>
        </div>
        <br>
        <div>
            <script>                
                function submitEspCode(){
                    $.ajax({
                            url:`/submit_esp_code?esp_code=${$('#esp_code').val()}`,
                            type:'POST',
                            success: function(data) {
                                console.log("suceessful esp code submit")
                            },
                            failure: function(data) {
                                console.log("failed esp code submit")
                            }
                        })
                }
            </script>
            <label for="esp_code">Type in the esp code to log in on this device:</label><br>
            <input type="text" id="esp_code" name="esp_code"><br>
            <input type="button" value="Submit" onClick="submitEspCode()">
        </div>
        <br>
        <div>
            <script>                
                function sleep(){
                    $.ajax({
                            url:`/sleep`,
                            type:'GET',
                            success: function(data) {
                                console.log("ESP went to sleep")
                            },
                            failure: function(data) {
                                console.log("Insomniac esp")
                            }
                        })
                }
            </script>
            <input type="button" value="Sleep" onClick="sleep()">
        </div>
    </body>
</html>
)rawliteral";