#ifdef COMM
#define INDEX_HTML "<!DOCTYPE html> \n \
<html> \n \
    <head> \n \
        <meta charset=\"utf-8\"> \n \
        <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"> \n \
        <title>Home</title> \n \
        <meta name=\"description\" content=\"\"> \n \
        <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> \n \
        <link rel=\"stylesheet\" href=\"./style.css\"> \n \
    </head> \n \
    <body> \n \
        <!--Toolbar--> \n \
        <div class=\"topnav\"> \n \
            <a class=\"active\" href=\"#./index.html\"> Home</a> \n \
            <a href=\"./device.html\">                Devices</a> \n \
            <a href=\"./profile.html\">               Profiles</a> \n \
            <a href=\"./setting.html\">               Settings</a> \n \
        </div> \n \
        <h1>Device Information</h1> \n \
        <div> \n \
            <table style=\"float: left\"> \n \
                <tr> \n \
                    <td >Device Name:</td> \n \
                    <td id=\"deviceName0\"></td> \n \
                </tr> \n \
                <tr> \n \
                    <td>Device Type:</td> \n \
                    <td id=\"deviceType\">Sensor</td> \n \
                </tr> \n \
                <tr> \n \
                    <td id=\"deviceDataType00\">Temperature (°C):</td> \n \
                    <td id=\"temperatureData\"></td> \n \
                </tr> \n \
                <tr> \n \
                    <td id=\"deviceDataType01\">Rel. Humidity (%):</td> \n \
                    <td id=\"humidityData\"></td> \n \
                </tr> \n \
                <tr> \n \
                    <td id=\"deviceDataType02\">Rel. Soil Moisture (%):</td> \n \
                    <td id=\"soilMoistData\"></td> \n \
                </tr> \n \
            </table> \n \
            <table style=\"float: left\"> \n \
                <tr> \n \
                    <td >Device Name:</td> \n \
                    <td id=\"deviceName1\"></td> \n \
                </tr> \n \
                <tr> \n \
                    <td>Device Type:</td> \n \
                    <td id=\"deviceType1\">Power</td> \n \
                </tr> \n \
                <tr> \n \
                    <td id=\"deviceDataType0\"></td> \n \
                    <td id=\"deviceData0\"></td> \n \
                </tr> \n \
                <tr> \n \
                    <td id=\"deviceDataType1\"></td> \n \
                    <td id=\"deviceData1\"></td> \n \
                </tr> \n \
                <tr> \n \
                    <td id=\"deviceDataType2\"></td> \n \
                    <td id=\"deviceData2\"></td> \n \
                </tr> \n \
            </table>     \n \
        </div> \n \
 \n \
        <script> \n \
            var portTypes = new Array(3); \n \
            document.addEventListener(\"DOMContentLoaded\", function() { \n \
                updatePage(); \n \
                setInterval(updatePage, 1000); \n \
            }); \n \
            function updatePage() \n \
            {  \n \
                getDeviceName(\'s\'); \n \
                getSensorData(0); \n \
                getSensorData(1); \n \
                getSensorData(2); \n \
 \n \
                getDeviceName(\'p\'); \n \
                getPortType(0); \n \
                getPortType(1); \n \
                getPortType(2); \n \
                updatePortLevel(); \n \
            } \n \
            function updatePortLevel() \n \
            {  \n \
                getPowerTypeLevel(\'f\');  \n \
                getPowerTypeLevel(\'l\');  \n \
                getPowerTypeLevel(\'p\');  \n \
                getPowerTypeLevel(\'n\');  \n \
            } \n \
            function getPowerTypeLevel(type) \n \
            {  \n \
                var requestStr = \'p\' + \'n\' + type; \n \
                console.log(\'Request send:\' + \'/info/\' + requestStr) \n \
                var xhr = new XMLHttpRequest(); \n \
                xhr.onreadystatechange = function() { \n \
                    if (xhr.readyState === XMLHttpRequest.DONE) { \n \
                        if (xhr.status === 200) { \n \
                            console.log(\'power  port type\' + type + \':\' + xhr.responseText); \n \
                            for (let it = 0; it < 3; it++) \n \
                            { \n \
                                if(portTypes[it] == type) \n \
                                { \n \
                                    if(xhr.responseText == \'0\') \n \
                                    { \n \
                                        document.getElementById(\'deviceData\' + it).innerHTML = \'Off\'; \n \
                                    } \n \
                                    else \n \
                                    { \n \
                                        document.getElementById(\'deviceData\' + it).innerHTML = \'On\'; \n \
                                    } \n \
                                } \n \
                            } \n \
                        } else { \n \
                            console.log(\'AJAX request failed with status \' + xhr.status); \n \
                        } \n \
                    } \n \
                }; \n \
                xhr.open(\'GET\', (\'/info/\' + requestStr)); \n \
                xhr.send(); \n \
            } \n \
            function getSensorData(type) \n \
            { \n \
                var requestStr = \'s\' + \'n\' + type; \n \
                console.log(\'Request send:\' + \'/info/\' + requestStr); \n \
                var xhr = new XMLHttpRequest(); \n \
                xhr.onreadystatechange = function() { \n \
                    if (xhr.readyState === XMLHttpRequest.DONE) { \n \
                        if (xhr.status === 200) { \n \
                                console.log(\'sensor data type: \' + type + \' data: \' + xhr.responseText); \n \
                                switch(type) \n \
                                { \n \
                                    case 0: \n \
                                        document.getElementById(\'temperatureData\').innerHTML = xhr.responseText; \n \
                                    break; \n \
                                    case 1: \n \
                                        document.getElementById(\'humidityData\').innerHTML = xhr.responseText; \n \
                                    break; \n \
                                    case 2: \n \
                                        document.getElementById(\'soilMoistData\').innerHTML = xhr.responseText; \n \
                                    break; \n \
                                } \n \
                        } else { \n \
                            console.log(\'AJAX request failed with status \' + xhr.status); \n \
                        } \n \
                    } \n \
                }; \n \
                xhr.open(\'GET\', (\'/info/\' + requestStr)); \n \
                xhr.send(); \n \
            } \n \
            function getPortType(i) \n \
            { \n \
                var requestStr = \'n\' + i + \'g\'; \n \
                console.log(\'Request send:\' + \'/portType/\' + requestStr); \n \
                var xhr = new XMLHttpRequest(); \n \
                xhr.onreadystatechange = function() { \n \
                    if (xhr.readyState === XMLHttpRequest.DONE) { \n \
                        if (xhr.status === 200) { \n \
                            console.log(\'power device port \' + i + \' type: \' + xhr.responseText); \n \
                            portTypes[i] = xhr.responseText \n \
                            switch( xhr.responseText ) \n \
                            { \n \
                                case \'f\': \n \
                                    document.getElementById(\'deviceDataType\' + i).innerHTML = \'Fan\'; \n \
                                break; \n \
                                case \'l\': \n \
                                    document.getElementById(\'deviceDataType\' + i).innerHTML = \'Light\'; \n \
                                break; \n \
                                case \'p\': \n \
                                    document.getElementById(\'deviceDataType\' + i).innerHTML = \'Pump\'; \n \
                                break; \n \
                                case \'n\': \n \
                                    document.getElementById(\'deviceDataType\' + i).innerHTML = \'None\'; \n \
                                break; \n \
                                default: \n \
                                    document.getElementById(\'deviceDataType\' + i).innerHTML = \'Error\'; \n \
                            } \n \
                             \n \
                        } else { \n \
                            console.log(\'AJAX request failed with status \' + xhr.status); \n \
                        } \n \
                    } \n \
                }; \n \
                xhr.open(\'GET\', (\'/portType/\' + requestStr)); \n \
                xhr.send(); \n \
            } \n \
            function getDeviceName(sp) \n \
            { \n \
                var requestStr = \'g\' + sp + \'n\'; \n \
                console.log(\'Request send:\' + \'/name/\' + requestStr); \n \
                var xhr = new XMLHttpRequest(); \n \
                xhr.onreadystatechange = function() { \n \
                    if (xhr.readyState === XMLHttpRequest.DONE) { \n \
                        if (xhr.status === 200) { \n \
                            if(sp == \'p\') \n \
                            { \n \
                                console.log(\'power device name: \' + xhr.responseText); \n \
                                document.getElementById(\'deviceName1\').innerHTML = xhr.responseText; \n \
                            } \n \
                            else \n \
                            { \n \
                                console.log(\'sensor device name: \' + xhr.responseText); \n \
                                document.getElementById(\'deviceName0\').innerHTML = xhr.responseText; \n \
                            } \n \
                             \n \
                        } else { \n \
                            console.log(\'AJAX request failed with status \' + xhr.status); \n \
                        } \n \
                    } \n \
                }; \n \
                xhr.open(\'GET\', (\'/name/\' + requestStr)); \n \
                xhr.send(); \n \
            } \n \
            function setCharAt(str, index, chr) { \n \
                if (index > str.length - 1) return str; \n \
                return str.substring(0, index) + chr + str.substring(index + 1); \n \
            } \n \
        </script> \n \
    </body> \n \
</html>"




#define GROUP_HTML "test"




#define PROFILE_HTML "<!DOCTYPE html> \n \
<html> \n \
    <head> \n \
        <meta charset=\"utf-8\"> \n \
        <title>Profile</title> \n \
        <meta name=\"description\" content=\"\"> \n \
        <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> \n \
        <link rel=\"stylesheet\" href=\"./style.css\"> \n \
 \n \
    </head> \n \
    <body> \n \
        <!--Toolbar--> \n \
        <div class=\"topnav\"> \n \
            <a href=\"./index.html\">                     Home</a> \n \
            <a href=\"./device.html\">                    Devices</a> \n \
            <a class=\"active\" href=\"#./profile.html\">   Profiles</a> \n \
            <a href=\"./setting.html\">                   Settings</a> \n \
        </div> \n \
        <br> \n \
        <div> \n \
            <table> \n \
                <tr> \n \
                    <td>Primary Light Cycle:</td> \n \
                    <td> \n \
                        <label for=\"lightTimer_p\">Duration (hr:min):<br></label> \n \
                        <input type=\"time\" id=\"lightTimer_p\" name=\"lightTimer_p\">  \n \
                    </td> \n \
                    <td> \n \
                        <label for=\"startTime_p\">Start time (hr:min):<br></label> \n \
                        <input type=\"time\" id=\"startTime_p\" name=\"startTime_p\"> \n \
                    </td> \n \
                    <td> \n \
                        <label for=\"lightDuration_p\">Duration (days):<br></label> \n \
                        <input type=\"number\" id=\"lightDuration_p\" name=\"lightDuration_p\" min=\"1\" max=\"255\"> \n \
                    </td> \n \
                </tr> \n \
                <tr> \n \
                    <td>Secondary Light Cycle:</td> \n \
                    <td> \n \
                        <label for=\"lightTimer_s\">Duration (hr:min):<br></label> \n \
                        <input type=\"time\" id=\"lightTimer_s\" name=\"lightTimer_s\"> \n \
                    </td> \n \
                    <td> \n \
                        <label for=\"startTime_s\">Start time (hr:min):<br></label> \n \
                        <input type=\"time\" id=\"startTime_s\" name=\"startTime_s\"> \n \
                    </td> \n \
                    <td> \n \
                        <label for=\"lightDuration_s\">Duration (days):<br></label> \n \
                        <input type=\"number\" id=\"lightDuration_s\" name=\"lightDuration_s\" min=\"1\" max=\"255\"> \n \
                    </td> \n \
                </tr> \n \
                <tr> \n \
                    <td>Fan Threshold:&ensp;</td> \n \
                    <td> \n \
                        <label for=\"humidityThreshold\">Humidity threshold (%):<br></label> \n \
                        <input type=\"number\" id=\"humidityThreshold\" name=\"humidityThreshold\" min=\"0\" max=\"100\"> \n \
                    </td> \n \
                    <td> \n \
                        <label for=\"tempThreshold\">Temperature threshold (°C):<br></label> \n \
                        <input type=\"number\" id=\"tempThreshold\" name=\"tempThreshold\" min=\"0\" max=\"45\"> \n \
                    </td> \n \
 \n \
                </tr> \n \
                <tr> \n \
                    <td>Pump Threshold:&ensp;</td> \n \
                    <td> \n \
                        <label for=\"soilMoistThreshold\">Soil moisture threshold (%):<br></label> \n \
                        <input type=\"number\" id=\"soilMoistThreshold\" name=\"soilMoistThreshold\" min=\"0\" max=\"100\"> \n \
                    </td> \n \
                </tr> \n \
            </table> \n \
            <button id=\"submitChanges\">Submit changes</button> \n \
        </div> \n \
 \n \
        <script> \n \
            var profileData = new Array(8); \n \
            document.addEventListener(\"DOMContentLoaded\", function() { \n \
 \n \
                function handleResponse(i) { \n \
                    return function() { \n \
                        console.log(\'Request for index \' + i + \' completed\'); \n \
                        var element = getIdFromInd(i); \n \
                        element.value = profileData[i]; \n \
                    }; \n \
                } \n \
 \n \
                for (let i = 0; i < 9; i++) { \n \
                    getProfileValue(i, handleResponse(i)); \n \
                } \n \
            }); \n \
 \n \
            function getProfileValue(i, callback) { \n \
                var requestStr = \"xng\"; \n \
                requestStr = setCharAt(requestStr, 1, i); \n \
                 \n \
                var xhr = new XMLHttpRequest(); \n \
                xhr.onreadystatechange = function() { \n \
                    if (xhr.readyState === XMLHttpRequest.DONE) { \n \
                        if (xhr.status === 200) { \n \
                            console.log(\'Return str for index \' + i + \': \' + xhr.responseText); \n \
                            profileData[i] = xhr.responseText; \n \
                        } else { \n \
                            console.log(\'AJAX request failed with status \' + xhr.status); \n \
                        } \n \
                        callback(); \n \
                    } \n \
                }; \n \
                xhr.open(\'GET\', (\'/profile/\' + requestStr)); \n \
                xhr.send(); \n \
            } \n \
 \n \
            function setProfileValue(i, value) \n \
            { \n \
                var requestStr = \"xns\"; \n \
                requestStr = setCharAt(requestStr, 1, i); \n \
                 \n \
                requestStr = requestStr + value; \n \
                var xhr = new XMLHttpRequest(); \n \
                xhr.onreadystatechange = function() { \n \
                    if (xhr.readyState === XMLHttpRequest.DONE) { \n \
                        if (xhr.status === 200) { \n \
                            console.log(\'update successful\'); \n \
                        } else { \n \
                            console.log(\'AJAX request failed with status \' + xhr.status); \n \
                        } \n \
                    } \n \
                }; \n \
                xhr.open(\'GET\', (\'/profile/\' + requestStr)); \n \
                xhr.send(); \n \
            } \n \
             \n \
            function setCharAt(str, index, chr) { \n \
                if (index > str.length - 1) return str; \n \
                return str.substring(0, index) + chr + str.substring(index + 1); \n \
            } \n \
 \n \
            function getIdFromInd(i) { \n \
                switch(i) \n \
                { \n \
                    case 0: \n \
                        return document.getElementById(\"lightTimer_p\"); \n \
                        break; \n \
                    case 1: \n \
                        return document.getElementById(\"startTime_p\"); \n \
                        break; \n \
                    case 2: \n \
                        return document.getElementById(\"lightDuration_p\"); \n \
                        break; \n \
                    case 3: \n \
                        return document.getElementById(\"lightTimer_s\"); \n \
                        break; \n \
                    case 4: \n \
                        return document.getElementById(\"startTime_s\"); \n \
                        break; \n \
                    case 5: \n \
                        return document.getElementById(\"lightDuration_s\"); \n \
                        break; \n \
                    case 6: \n \
                        return document.getElementById(\"humidityThreshold\"); \n \
                        break; \n \
                    case 7: \n \
                        return document.getElementById(\"tempThreshold\"); \n \
                        break; \n \
                    case 8: \n \
                        return document.getElementById(\"soilMoistThreshold\"); \n \
                        break; \n \
                    default: \n \
                        console.log(\"getIdFromInd: Index out of range\"); \n \
                } \n \
            } \n \
             \n \
            document.getElementById(\"submitChanges\").addEventListener(\"click\", function () { \n \
                console.log(\"checking elements for changes\"); \n \
                var element; \n \
                for(let i = 0; i < 9; i++) \n \
                { \n \
                    element = getIdFromInd(i); \n \
                    if (element.value !== profileData[i]) \n \
                    { \n \
                        console.log(\"Element \" + i + \" does not match\"); \n \
                        profileData[i] = element.value; \n \
                        setProfileValue(i, profileData[i]); \n \
                    } \n \
                } \n \
            }); \n \
        </script> \n \
    </body> \n \
</html>"




#define SETTINGS_HTML "<!DOCTYPE html> \n \
<html> \n \
    <head> \n \
        <meta charset=\"utf-8\"> \n \
        <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"> \n \
        <title>Settings</title> \n \
        <meta name=\"description\" content=\"\"> \n \
        <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> \n \
        <link rel=\"stylesheet\" href=\"./style.css\"> \n \
    </head> \n \
    <body> \n \
        <!--Toolbar--> \n \
        <div class=\"topnav\"> \n \
            <a href=\"./index.html\">                   Home</a> \n \
            <a href=\"./device.html\">                  Devices</a> \n \
            <a href=\"./profile.html\">                 Profiles</a> \n \
            <a class=\"active\" href=\"#./setting.html\">Settings</a> \n \
        </div> \n \
        <br> \n \
        <label for=\"systemDay\">Current Day:</label> \n \
        <input type=\"number\" id=\"systemDay\" name=\"systemDay\" value=\"0\" readonly> \n \
        <button id=\"reset\">Reset</button> \n \
 \n \
        <br> \n \
        <label for=\"systemTime\">System time:</label> \n \
        <input type=\"time\" id=\"systemTime\" name=\"systemTime\" value=\"01:00\"> \n \
        <button id=\"changeTime\">Change</button> \n \
        <br> \n \
         \n \
        <script> \n \
            document.addEventListener(\"DOMContentLoaded\", function() { \n \
                getLocalTime(); \n \
                getDay(); \n \
            }); \n \
 \n \
            function getDay() \n \
            { \n \
                console.log(\"Sending get day command\"); \n \
                var xhr = new XMLHttpRequest(); \n \
                xhr.onreadystatechange = function() { \n \
                    if (xhr.readyState === XMLHttpRequest.DONE) { \n \
                        if (xhr.status === 200) { \n \
                            console.log(\"get day command transmission successful.\"); \n \
                            document.getElementById(\"systemDay\").value = xhr.responseText; \n \
                        } else { \n \
                            console.log(\'AJAX request failed with status \' + xhr.status); \n \
                        } \n \
                    } \n \
                } \n \
                xhr.open(\'GET\', (\'/day/\' + \"g\")); \n \
                xhr.send(); \n \
            } \n \
 \n \
            function getLocalTime() \n \
            { \n \
                console.log(\"Sending get local time command\"); \n \
                var xhr = new XMLHttpRequest(); \n \
                xhr.onreadystatechange = function() { \n \
                    if (xhr.readyState === XMLHttpRequest.DONE) { \n \
                        if (xhr.status === 200) { \n \
                            console.log(\"get local time command transmission successful.\"); \n \
                            document.getElementById(\"systemTime\").value = xhr.responseText; \n \
                        } else { \n \
                            console.log(\'AJAX request failed with status \' + xhr.status); \n \
                        } \n \
                    } \n \
                } \n \
                xhr.open(\'GET\', (\'/time/\' + \"g\")); \n \
                xhr.send(); \n \
            } \n \
 \n \
            document.getElementById(\"changeTime\").addEventListener(\"click\", function () { \n \
                var newTime = document.getElementById(\"systemTime\").value; \n \
                console.log(\"Sending set local time command\"); \n \
                console.log(\"Request time: \" + newTime); \n \
                var xhr = new XMLHttpRequest(); \n \
                xhr.onreadystatechange = function() { \n \
                    if (xhr.readyState === XMLHttpRequest.DONE) { \n \
                        if (xhr.status === 200) { \n \
                            console.log(\"set local time command transmission successful.\"); \n \
                        } else { \n \
                            console.log(\'AJAX request failed with status \' + xhr.status); \n \
                        } \n \
                    } \n \
                } \n \
                xhr.open(\'GET\', (\'/time/\' + \"s\" + newTime)); \n \
                xhr.send(); \n \
            }); \n \
 \n \
            document.getElementById(\"reset\").addEventListener(\"click\", function () { \n \
                console.log(\"Sending reset day command\"); \n \
                var xhr = new XMLHttpRequest(); \n \
                xhr.onreadystatechange = function() { \n \
                    if (xhr.readyState === XMLHttpRequest.DONE) { \n \
                        if (xhr.status === 200) { \n \
                            console.log(\"reset day command transmission successful.\"); \n \
                            document.getElementById(\"systemDay\").value = \'0\'; \n \
                        } else { \n \
                            console.log(\'AJAX request failed with status \' + xhr.status); \n \
                        } \n \
                    } \n \
                } \n \
                xhr.open(\'GET\', (\'/day/\' + \"s\")); \n \
                xhr.send(); \n \
            }); \n \
        </script> \n \
    </body> \n \
</html>"





#define DEVICES_HTML "<!DOCTYPE html> \n \
<html> \n \
    <head> \n \
        <meta charset=\"utf-8\"> \n \
        <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"> \n \
        <title>Device</title> \n \
        <meta name=\"description\" content=\"\"> \n \
        <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> \n \
        <link rel=\"stylesheet\" href=\"./style.css\"> \n \
 \n \
    </head> \n \
    <body> \n \
        <!--Toolbar--> \n \
        <div class=\"topnav\"> \n \
            <a href=\"./index.html\">                   Home</a> \n \
            <a class=\"active\" href=\"#./device.html\">   Devices</a> \n \
            <a href=\"./profile.html\">                 Profiles</a> \n \
            <a href=\"./setting.html\">                Settings</a> \n \
        </div> \n \
        <br> \n \
        <div> \n \
            <!-- \n \
            <table> \n \
                <tr> \n \
                    <td id=\"powerNumb\">Total power hub(s):</td> \n \
                    <td id=\"sensorNumb\">Total sensor hub(s):</td> \n \
                    <td><button id=\"discover\">Discover</button></td> \n \
                </tr> \n \
            </table> \n \
            <br> \n \
            --> \n \
            <table> \n \
                <tr> \n \
                    <td> \n \
                        <label for=\"hubType\">Hub type:</label> \n \
                        <select name=\"hubType\" id=\"hubType\"> \n \
                            <option value=\"sensor\">Sensor</option> \n \
                            <option value=\"power\">Power</option> \n \
                        </select> \n \
                        <!--<label for=\"hubNumb\">Device Number:</label> \n \
                        <select name=\"hubNumb\" id=\"hubNumb\"></select> \n \
                    </td>--> \n \
                    <!--<td> \n \
                        <button id=\"blink\">Blink</button> \n \
                    </td>--> \n \
                </tr> \n \
                <tr> \n \
                    <td> \n \
                        <label for=\"deviceName\">Device name:</label> \n \
                        <input type=\"text\" minlength=\"1\" maxlength=\"48\" id=\"deviceName\" name=\"deviceName\" value=\"Power_Hub\"> \n \
                    </td> \n \
                    <td> \n \
                        <button id=\"changeName\">Change</button> \n \
                    </td> \n \
            </table> \n \
            <br> \n \
            <div id=\"powerSettings\"> \n \
                <table> \n \
                    <tr> \n \
                        <td> \n \
                            <label for=\"port0\">Port 0: </label> \n \
                            <select name=\"port0\" id=\"port0\"> \n \
                                <option value=\"none\">None</option> \n \
                                <option value=\"fan\">Fan</option> \n \
                                <option value=\"pump\">Pump</option> \n \
                                <option value=\"light\">Light</option> \n \
                            </select>  \n \
                        </td> \n \
                        <td> \n \
                            <button id=\"changeP0\">Change</button> \n \
                        </td> \n \
                    </tr> \n \
                    <tr> \n \
                        <td> \n \
                            <label for=\"port1\">Port 1: </label> \n \
                            <select name=\"port1\" id=\"port1\"> \n \
                                <option value=\"none\">None</option> \n \
                                <option value=\"fan\">Fan</option> \n \
                                <option value=\"pump\">Pump</option> \n \
                                <option value=\"light\">Light</option> \n \
                            </select>  \n \
                        </td> \n \
                        <td> \n \
                            <button id=\"changeP1\">Change</button> \n \
                        </td> \n \
                    </tr> \n \
                    <tr> \n \
                        <td> \n \
                            <label for=\"port2\">Port 2: </label> \n \
                            <select name=\"port2\" id=\"port2\"> \n \
                                <option value=\"none\">None</option> \n \
                                <option value=\"fan\">Fan</option> \n \
                                <option value=\"pump\">Pump</option> \n \
                                <option value=\"light\">Light</option> \n \
                            </select>  \n \
                        </td> \n \
                        <td> \n \
                            <button id=\"changeP2\">Change</button> \n \
                        </td> \n \
                    </tr> \n \
                </table> \n \
                <br> \n \
            </div> \n \
 \n \
            <div id=\"sensorSettings\"> \n \
                <table> \n \
                    <tr> \n \
                        <td>Calibrate soil moisture sensor</td> \n \
                        <td> \n \
                            <button id=\"calibrateSoil\">Calibrate</button> \n \
                        </td> \n \
                    </tr> \n \
                    <tr id=\"sensorCalibration\"> \n \
                        <td>Place soil moisture sensor in fully saturated soil for 1-5 minutes then</td> \n \
                        <td><button id=\"calibrateBtn\">click me</button></td> \n \
                    </tr> \n \
                     \n \
                     \n \
                </table> \n \
                <br> \n \
            </div> \n \
             \n \
        </div> \n \
        <script> \n \
            document.addEventListener(\"DOMContentLoaded\", function() { \n \
                getName(\'s\'); \n \
                showSensorOptionUI(); \n \
                getPortType(0); \n \
                getPortType(1); \n \
                getPortType(2); \n \
            }); \n \
 \n \
            document.getElementById(\"hubType\").addEventListener(\"change\", function()  \n \
            { \n \
                if(document.getElementById(\"hubType\").value == \"power\") \n \
                { \n \
                    hideSensorOptionUI(); \n \
                    showPowerOptionUI(); \n \
                    getName(\'p\'); \n \
                } \n \
                else \n \
                { \n \
                    hidePowerOptionUI(); \n \
                    showSensorOptionUI(); \n \
                    getName(\'s\'); \n \
                } \n \
                document.getElementById(\"sensorCalibration\").style.display = \"none\"; \n \
                console.log(\"value of selection: \" + document.getElementById(\"hubType\").value); \n \
            }); \n \
 \n \
            function getName(sp) \n \
            { \n \
                requestStr = \"g\" + sp + \"n\"; \n \
                console.log(\"Sending name request\"); \n \
                console.log(\"request: \" + requestStr); \n \
                var xhr = new XMLHttpRequest(); \n \
                xhr.onreadystatechange = function() { \n \
                    if (xhr.readyState === XMLHttpRequest.DONE) { \n \
                        if (xhr.status === 200) { \n \
                            console.log(\"get name command transmission successful.\"); \n \
                            document.getElementById(\"deviceName\").value = xhr.responseText; \n \
                        } else { \n \
                            console.log(\'AJAX request failed with status \' + xhr.status); \n \
                        } \n \
                    } \n \
                } \n \
                xhr.open(\'GET\', (\'/name/\' + requestStr)); \n \
                xhr.send(); \n \
            } \n \
 \n \
            function changeName() \n \
            { \n \
                console.log(\"change name command sending\"); \n \
                var requestStr; \n \
                document.getElementById(\"deviceName\").value = document.getElementById(\"deviceName\").value.replace(/\\s/g, \'_\'); \n \
                if(document.getElementById(\"hubType\").value == \"power\") \n \
                { \n \
                    requestStr = \"spn\"; \n \
                } \n \
                else \n \
                { \n \
                    requestStr = \"ssn\"; \n \
                } \n \
                var requestStr = requestStr + document.getElementById(\"deviceName\").value; \n \
                console.log(\"Request String: \" + requestStr); \n \
                var xhr = new XMLHttpRequest(); \n \
                xhr.onreadystatechange = function() { \n \
                    if (xhr.readyState === XMLHttpRequest.DONE) { \n \
                        if (xhr.status === 200) { \n \
                            console.log(\"name change command transmission successful.\"); \n \
                        } else { \n \
                            console.log(\'AJAX request failed with status \' + xhr.status); \n \
                        } \n \
                    } \n \
                } \n \
                xhr.open(\'GET\', (\'/name/\' + requestStr)); \n \
                xhr.send(); \n \
            } \n \
            document.getElementById(\"changeName\").addEventListener(\"click\", changeName); \n \
 \n \
            function setCharAt(str, index, chr) { \n \
            if (index > str.length - 1) return str; \n \
            return str.substring(0, index) + chr + str.substring(index + 1); \n \
            } \n \
 \n \
            function hidePowerOptionUI() \n \
            { \n \
                document.getElementById(\"powerSettings\").style.display = \"none\"; \n \
            } \n \
             \n \
            function hideSensorOptionUI() \n \
            { \n \
                document.getElementById(\"sensorSettings\").style.display = \"none\"; \n \
            } \n \
 \n \
            function showPowerOptionUI() \n \
            { \n \
                document.getElementById(\"powerSettings\").style.display = \"inline\"; \n \
            } \n \
            function showSensorOptionUI() \n \
            { \n \
                document.getElementById(\"sensorSettings\").style.display = \"inline\"; \n \
            } \n \
 \n \
            function getPortType(i) \n \
            { \n \
                var requestStr = \"n\" + i + \"g\"; \n \
                var xhr = new XMLHttpRequest(); \n \
                xhr.onreadystatechange = function() { \n \
                    if (xhr.readyState === XMLHttpRequest.DONE) { \n \
                        if (xhr.status === 200) { \n \
                            console.log(\'power device port \' + i + \' type: \' + xhr.responseText); \n \
                            switch( xhr.responseText ) \n \
                            { \n \
                                case \'f\': \n \
                                    document.getElementById(\'port\' + i).value = \'fan\'; \n \
                                break; \n \
                                case \'l\': \n \
                                    document.getElementById(\'port\' + i).value = \'light\'; \n \
                                break; \n \
                                case \'p\': \n \
                                    document.getElementById(\'port\' + i).value = \'pump\'; \n \
                                break; \n \
                                case \'n\': \n \
                                    document.getElementById(\'port\' + i).value = \'none\'; \n \
                                break; \n \
                                default: \n \
                                    document.getElementById(\'port\' + i).value = \'error\'; \n \
                            } \n \
                        } else { \n \
                            console.log(\'AJAX request failed with status \' + xhr.status); \n \
                        } \n \
                    } \n \
                }; \n \
                xhr.open(\'GET\', (\'/portType/\' + requestStr)); \n \
                xhr.send(); \n \
            } \n \
 \n \
            document.getElementById(\"changeP0\").addEventListener(\"click\", function()  \n \
            { \n \
                requestStr = \"n\" + \"0\" + \'s\' + document.getElementById(\"port0\").value[0]; \n \
 \n \
                console.log(\"Request String: \" + requestStr); \n \
                var xhr = new XMLHttpRequest(); \n \
                xhr.onreadystatechange = function() { \n \
                    if (xhr.readyState === XMLHttpRequest.DONE) { \n \
                        if (xhr.status === 200) { \n \
                            console.log(\"port change transmission successful.\"); \n \
                        } else { \n \
                            console.log(\'AJAX request failed with status \' + xhr.status);  \n \
                        } \n \
                    } \n \
                } \n \
                xhr.open(\'GET\', (\'/portType/\' + requestStr)); \n \
                xhr.send(); \n \
            }); \n \
 \n \
            document.getElementById(\"changeP1\").addEventListener(\"click\", function()  \n \
            { \n \
                requestStr = \"n\" + \"1\" + \'s\' + document.getElementById(\"port1\").value[0]; \n \
 \n \
                console.log(\"Request String: \" + requestStr); \n \
                var xhr = new XMLHttpRequest(); \n \
                xhr.onreadystatechange = function() { \n \
                    if (xhr.readyState === XMLHttpRequest.DONE) { \n \
                        if (xhr.status === 200) { \n \
                            console.log(\"port change transmission successful.\"); \n \
                        } else { \n \
                            console.log(\'AJAX request failed with status \' + xhr.status);  \n \
                        } \n \
                    } \n \
                } \n \
                xhr.open(\'GET\', (\'/portType/\' + requestStr)); \n \
                xhr.send(); \n \
            }); \n \
 \n \
            document.getElementById(\"changeP2\").addEventListener(\"click\", function()  \n \
            { \n \
                requestStr = \"n\" + \"2\" + \'s\' + document.getElementById(\"port2\").value[0]; \n \
 \n \
                console.log(\"Request String: \" + requestStr); \n \
                var xhr = new XMLHttpRequest(); \n \
                xhr.onreadystatechange = function() { \n \
                    if (xhr.readyState === XMLHttpRequest.DONE) { \n \
                        if (xhr.status === 200) { \n \
                            console.log(\"port change transmission successful.\"); \n \
                        } else { \n \
                            console.log(\'AJAX request failed with status \' + xhr.status);  \n \
                        } \n \
                    } \n \
                } \n \
                xhr.open(\'GET\', (\'/portType/\' + requestStr)); \n \
                xhr.send(); \n \
            }); \n \
 \n \
            document.getElementById(\"calibrateSoil\").addEventListener(\"click\", showCalibration); \n \
 \n \
            function showCalibration() \n \
            { \n \
                document.getElementById(\"sensorCalibration\").style.display = \"inline\"; \n \
            } \n \
 \n \
            document.getElementById(\"calibrateBtn\").addEventListener(\"click\", setMoistMax); \n \
 \n \
            function setMoistMax() \n \
            { \n \
                getCurrentsMoist(function(response){ \n \
                    if(response != null) \n \
                    { \n \
                        var requestStr =\"/soilMax/s\" + response; \n \
                        var xhr = new XMLHttpRequest(); \n \
                        xhr.onreadystatechange = function() { \n \
                            if (xhr.readyState === XMLHttpRequest.DONE) { \n \
                                if (xhr.status === 200) { \n \
                                    console.log(\'max soil moisture value is \' + response); \n \
                                } else { \n \
                                    console.log(\'AJAX request failed with status \' + xhr.status); \n \
                                } \n \
                            } \n \
                        }; \n \
                        xhr.open(\'GET\', requestStr); \n \
                        xhr.send(); \n \
                    } \n \
                }); \n \
            } \n \
 \n \
            function getCurrentsMoist(callback) \n \
            { \n \
                var requestStr = \"/info/sn2d\"; \n \
                var xhr = new XMLHttpRequest(); \n \
                xhr.onreadystatechange = function() { \n \
                    if (xhr.readyState === XMLHttpRequest.DONE) { \n \
                        if (xhr.status === 200) { \n \
                            console.log(\'soil moisture value: \' + xhr.responseText); \n \
                            callback(xhr.responseText); \n \
                        } else { \n \
                            console.log(\'AJAX request failed with status \' + xhr.status); \n \
                            callback(null); \n \
                        } \n \
                    } \n \
                }; \n \
                xhr.open(\'GET\', requestStr); \n \
                xhr.send(); \n \
            } \n \
        </script> \n \
    </body> \n \
</html>"





#define STYLE_CSS "* {  \n \
  box-sizing: border-box; \n \
  font-size: 1rem; \n \
} \n \
html, body { \n \
  min-height: 100%; \n \
  background: #363636; \n \
  color: #eeeeee; \n \
} \n \
/* sets the body height of the browser, so that backgrounds and div heights work correctly. Color and background will almost certainly be altered; they are just placeholders */ \n \
body { \n \
  font-size: 1.4rem; \n \
  text-rendering: optimizeLegibility; \n \
} \n \
/* sets the base font size for the document; turns on kerning pairs and ligatures */ \n \
body, ul, ol, dl { \n \
  margin: 0; \n \
} \n \
article, aside, audio,  \n \
footer, header, nav, section, video { \n \
  display: block;  \n \
} \n \
/* ensures that older browsers will display HTML5 elements correctly */ \n \
h1 { \n \
  font-size: 1.4rem; \n \
} \n \
/* begins to set up font sizes relative to base rem – h1 has been used as an example */ \n \
p {  \n \
  -ms-word-break: break-all; \n \
  word-break: break-all; \n \
  word-break: break-word; \n \
  -moz-hyphens: auto; \n \
  -webkit-hyphens: auto; \n \
  -ms-hyphens: auto; \n \
  hyphens: auto; \n \
}  \n \
/* sets hyphenation by default on paragraphs */ \n \
textarea {  \n \
  resize: vertical; \n \
} \n \
/* changes textarea resizing from \"both\" (UA default) to vertical only */ \n \
table { border-collapse: collapse; } \n \
td, th { \n \
    padding: .5rem; \n \
    border: 1px solid #dddddd; \n \
} \n \
/* causes tables to actually look like tables */ \n \
img {  \n \
  border: none; \n \
  max-width: 100%; \n \
} \n \
/* removes outlines from linked images (for older browsers and IE), sets up images for responsive design layout */ \n \
input[type=\"submit\"]::-moz-focus-inner,  \n \
input[type=\"button\"]::-moz-focus-inner { \n \
  border : 0px; \n \
} \n \
/* removes the inner border effect from focused buttons for form elements in Firefox */ \n \
input:required:after { \n \
  color: #f00; \n \
  content: \" *\"; \n \
}/* sets up required form fields with the conventional following red asterix */ \n \
input[type=\"email\"]:invalid {  \n \
  background: #f00; \n \
} \n \
/* causes a visual alert in browsers that do client-side checking of HTML5 email field – this may already be the default in some browsers. */ \n \
.right {  \n \
  float: right; \n \
  margin-left: 2rem; \n \
  clear: right; \n \
} \n \
.left {  \n \
  float: left; \n \
  margin-right: 2rem; \n \
  clear: left; \n \
} \n \
/* allows a quick shortcut for floating elements left and right while avoiding the “float quirk” problem */ \n \
sub, sup {  \n \
  line-height: 0; \n \
} \n \
/* sets superscript and subscript text so that it doesn\'t throw off leading for elements in which they are included */ \n \
/*TOP NAVAGATION BAR*/ \n \
/* Add a black background color to the top navigation */ \n \
.topnav { \n \
    background-color: #333; \n \
    overflow: hidden; \n \
} \n \
/* Style the links inside the navigation bar */ \n \
.topnav a { \n \
  float: left; \n \
  color: #f2f2f2; \n \
  text-align: center; \n \
  padding: 14px 16px; \n \
  text-decoration: none; \n \
  font-size: 17px; \n \
} \n \
/* Change the color of links on hover */ \n \
.topnav a:hover { \n \
  background-color: #ddd; \n \
  color: black; \n \
} \n \
/* Add a color to the active/current link */ \n \
.topnav a.active { \n \
  background-color: #04AA6D; \n \
  color: white; \n \
} \n \
/*BUTTON*/ \n \
.button { \n \
  border: none; \n \
  color: white; \n \
  padding: 16px 32px; \n \
  text-align: center; \n \
  text-decoration: none; \n \
  display: inline-block; \n \
  font-size: 16px; \n \
  margin: 4px 2px; \n \
  transition-duration: 0.4s; \n \
  cursor: pointer; \n \
} \n \
.button1 { \n \
  background-color: white; \n \
  color: black; \n \
  border: 2px solid #04AA6D; \n \
} \n \
.button1:hover { \n \
  background-color: #04AA6D; \n \
  color: white; \n \
} \n \
.button2 { \n \
  background-color: white; \n \
  color: black; \n \
  border: 2px solid #008CBA; \n \
} \n \
.button2:hover { \n \
  background-color: #008CBA; \n \
  color: white; \n \
} \n \
 \n \
#sensorSettings, #powerSettings, #sensorCalibration \n \
{ \n \
  display: none; \n \
} \n \
 \n \
input[type=\'number\']{ \n \
  width: 3em; \n \
}"
#endif