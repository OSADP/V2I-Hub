"use strict"; 

var commonLibraryJsVersion = "0.0.1";

// Element to attach events to
var eventElement = document;


var socketTimeout_ms = 600000;

var screenNotification = true;
var soundNotification = true;
var screenNotificationSetting = true;
var soundNotificationSetting = true;


var currentAudio = -1;
var defaultSoundTimeout = 15000;
var SoundTimerID = null;

//Image timeout variables
var timerID = null;
var defaultTimout = 3000;

var socketItems = 0;

var telemLists = [];
var socketObj = null;
var logFileName = "";
var msgBuffer = "";

var pluginsInitialized=false;

var cmdCntr = 0;
var currUser = null;
var permissions = 0;

var useSSL = true;
var timeOffsetMs = 0;

function ssl_changed()
{
    if (useSSL) socketObj.url = "wss://" + socketObj.ip + ':' + socketObj.port + "/";
    else socketObj.url = "ws://" + socketObj.ip + ':' + socketObj.port + "/";

}

/**
*   Print text to console with a timestamp.
*   @private
*   @param {string} msg - Text to be displayed on console
**/
function log(msg) {
    console.log(timeStamp() + " " + msg);
}

function setUrlCookie(urlValue) {
    var d = new Date();
    d.setTime(d.getTime() + (7*24*60*60*1000));
    var expires = "expires="+ d.toUTCString();
    document.cookie = "connectURL" + "=" + urlValue + ";" + expires + ";path=/";
}

function getUrlCookie(cname) {
    var name = "connectURL=";
    var decodedCookie = decodeURIComponent(document.cookie);
    var ca = decodedCookie.split(';');
    for(var i = 0; i <ca.length; i++) {
        var c = ca[i];
        while (c.charAt(0) == ' ') {
            c = c.substring(1);
        }
        if (c.indexOf(name) == 0) {
            return c.substring(name.length, c.length);
        }
    }
    return "";
}

/** 
* Create a new socket connection to the specified IP and port. 
* @private
* @param {Object} devices - Array of devices for this connection
* @param {string} ip - IP Address for socket connection
* @param {string} port - Port for socket connection
* @param {string} led - Name of HTML DOM element to show connections status
* @returns {Object} New socket connection
**/
function createSocketObject(ip, port, led) {
	var socketObject = new Object();
	socketObject.ip = ip;
	socketObject.port = port;
	socketObject.connected = false;
	socketObject.connecting = false;
	socketObject.led = led;
	socketObject.socket = null;
	socketObject.url = null;
	socketObject.timerID = null;
	socketObject.timeoutID = null;
	socketObject.startTimer = function (value)
	{
		if ((socketObject.timeoutID != null) && (socketObject.timeoutID != undefined))
		{
			console.log("Clearing timeout for " + socketObject.url + " socketObject.timeoutID:" + socketObject.timeoutID);
			clearTimeout(socketObject.timeoutID);
			socketObject.timeoutID = null;
		}
		socketObject.timeoutID = setTimeout(
			function()
			{ 
				console.log("------- Socket Timeout Closing " + socketObject.url + " ID:" + socketObject.timeoutID + "----------------");
				socketObject.socket.close();
				updateConnected(socketObject.url, false);
				socketObject.connected = false;
				socketObject.connecting = false;
			}, 
			socketTimeout_ms
		);
//		console.log("--------------------------------------- Starting Timer " + socketObject.timeoutID + "-------------------------------------------------------------")
 	};

	return socketObject;
}

var printed = false;

function updateListObject(plugin, name, value) {

    var found = false;

    if (name.toUpperCase() == "ENABLED") {
        RefreshHeaderStatus(plugin, value);
    } else if (name.toUpperCase() == "VERSION") {
        $("div[id=\"pluginHeader_" + plugin + "\"] > .headerVersion").html(value);
    } else if (name.toUpperCase() == "DESCRIPTION") {
        $("div[id=\"infoHeader_" + plugin + "\"]").html(value);
    }
    // Look for table row in table
    $("[id=\"infoTable_" + plugin + "\"] > tbody  > tr").each(function () {
        // if found just update the values
        if ($(this).find("td").eq(0).html() == name) {
            found = true;
            $(this).find("td").eq(1).html(value.trim());
        }
    });

    // Add new table row
    if (!found) {
        $("[id=\"infoTable_" + plugin + "\"]").append("<tr><td data-type='key'>" + name + "</td><td data-type='value'>" + value.trim() + "</td></tr>");
    }
}

function updateStateObject(plugin, name, value) {
    var found = false;
    // Look for table row in table
    $("[id=\"stateTable_" + plugin + "\"] > tbody  > tr").each(function() {
        // if found just update the values
        if ($(this).find("td").eq(0).html() == name)
        {
            found = true;
            $(this).find("td").eq(1).html(value.trim());
        }
        
    });

    // Add new table row
    if (!found)
    {
        $("[id=\"stateTable_" + plugin + "\"]").append("<tr><td data-type='key'>" + name + "</td><td data-type='value'>" + value + "</td></tr>");
    }
}

function updateStatusObject(plugin, name, value) {
    var state = value.trim().toLowerCase();
    if (state.indexOf("running") != -1) {
        $("div[id=\"pluginHeader_" + plugin + "\"] > .pluginState").attr("data-state", "green");
    } else if (state.indexOf("stopped / disconnected") != -1) {
        $("div[id=\"pluginHeader_" + plugin + "\"] > .pluginState").attr("data-state", "red");
    } else {
        $("div[id=\"pluginHeader_" + plugin + "\"] > .pluginState").attr("data-state", "off");
    }
}


function updateMessagesObject(plugin, id, type, subtype, count, lastTimestamp, averageInterval) {
    var found = false;
    // Look for table row in plugin table
    $("[id=\"messagesTable_" + plugin + "\"] > tbody  > tr").each(function() {
        // if found just update the values
        if (($(this).find("td").eq(0).html() == type) && ($(this).find("td").eq(1).html() == subtype))
        {
            found = true;
            $(this).find("td").eq(2).html(count);
            $(this).find("td").eq(3).html(lastTimestamp);
            $(this).find("td").eq(4).html(averageInterval);
        }
        
    });

    // Add new table row
    if (!found)
    {
        var tableRow = "<tr><td data-type='type'>" + type + "</td><td data-type='subtype'>" + subtype + "</td><td data-type='count'>" + count + "</td><td data-type='lastTimestamp'>" + lastTimestamp + "</td><td data-type='averageInterval'>" + averageInterval + "</td></tr>";
        var table = $("[id=\"messagesTable_" + plugin + "\"]");
        $("[id=\"messagesTable_" + plugin + "\"]").append(tableRow);
    }

    // Update the main table
    var table = $("[id=messagesTable]").DataTable();
    var indexes = table.rows().eq(0).filter( function (rowIndex) {
        return ((table.cell(rowIndex, 0).data() === plugin) && (table.cell(rowIndex, 1).data() === type) && (table.cell(rowIndex, 2).data() === subtype)) ? true : false;
    } );
    
    if (indexes.length > 0) { // Check for existing rows
        // Update values
        for (var i=0; i < indexes.length; i++) {
            var rowData = table.row(indexes[i]).data();
            rowData[3] = count;
            rowData[4] = lastTimestamp;
            rowData[5] = averageInterval;
            table.row(indexes[i]).data(rowData).draw('full-hold');
        }
    } else { // No rows with that message so create one
       table.row.add([
            plugin,
            type,
            subtype,
            count,
            lastTimestamp,
            averageInterval
       ]).draw('full-hold');
    }
}

function updateEventsObject(description, id, level, source, timestamp) {
   var table = $("[id=eventLogTable]").DataTable();
   table.row.add([
        level,
        source,
        description,
        timestamp
   ]);
}


function updateConfigurationObject(plugin, name, value, defaultValue, Description) {
    var found = false;
    // Look for table row in table
    $("[id=\"configsTable_" + plugin + "\"] > tbody  > tr").each(function() {
        // if found just update the values
        if ($(this).find("td").eq(0).html() == name)
        {
            found = true;
            if (value != null) {
                if (permissions == 1) {
                    $(this).find("td").eq(1).html(value);
                } else if (permissions == 2 || permissions == 3) {
                    var cell = $(this).find("td").eq(1).find("input");
                    cell.attr("data-value", value);
                    cell.val(value);
                    cell.css("background-color", "");
                    cell.css("color", "black");
                }
            }
            if (defaultValue != null) {
                $(this).find("td").eq(2).html(defaultValue);
            }
            if (Description != null) {
                $(this).find("td").eq(3).html(Description);
            }
            // TODO Can we add abreak here?
        }
        
    });

    // Add new table row
    if (!found)
    {
        if (permissions == 1) {
            $("[id=\"configsTable_" + plugin + "\"]").append("<tr><td data-type='key'>" + name + "</td><td data-type='value'>" + value + "</td><td data-type='defaultValue'>" + defaultValue + "</td><td data-type='description'>" + Description + "</td></tr>");
        } else if (permissions == 2 || permissions == 3) {
            $("[id=\"configsTable_" + plugin + "\"]").append("<tr><td data-type='key'>" + name + "</td><td data-type='value'><input class='configInput' onfocusout='ResetDisplay(this);' data-plugin='" + plugin + "' data-name='" + name + "' data-value='" + value + "' value='" + value + "'/></td><td data-type='defaultValue'>" + defaultValue + "</td><td data-type='description'>" + Description + "</td></tr>");
        }
    }
}

function ResetDisplay(input) {
    input.value = input.getAttribute("data-value");
    //input.setAttribute("disabled", "true");
}

/**
*   Message Handler. All incoming messages are sent to this handler.
*   @private
*   @param {string} - Incoming message
**/
function messageHandler(msg)
{
//	console.log(msg);
	// Strip control characters
	msg = msg.replace(/[\x00-\x1F\x7F-\x9F]/g, "");
	var json_obj = JSON.parse(msg);
//    console.log("Message " + json_obj.header.type + ":" + json_obj.header.subtype);
	if (json_obj.header.type.toUpperCase() === "TELEMETRY") {
	    if (json_obj.header.subtype.toUpperCase() === "LIST") {
	        handleListMessage(json_obj.payload);
	    }
	    else if (json_obj.header.subtype.toUpperCase() === "STATUS") {
	        handleStatusMessage(json_obj.payload);
	    }
	    else if (json_obj.header.subtype.toUpperCase() === "STATE") {
	        handleStateMessage(json_obj.payload);
	    }
	    else if (json_obj.header.subtype.toUpperCase() === "CONFIG") {
	        handleConfigMessage(json_obj.payload);
	    }
	    else if (json_obj.header.subtype.toUpperCase() === "MESSAGES") {
	        handleMessagesMessage(json_obj.payload);
	    }
        else if (json_obj.header.subtype.toUpperCase() === "EVENTS") {
            handleEventsMessage(json_obj.payload);
        }
        else if (json_obj.header.subtype.toUpperCase() === "REMOVE_LIST") {
            handleRemoveListMessage(json_obj.payload);
        }
        else if (json_obj.header.subtype.toUpperCase() === "REMOVE_STATUS") {
            handleRemoveStatusMessage(json_obj.payload);
        }
        else if (json_obj.header.subtype.toUpperCase() === "REMOVE_STATE") {
            handleRemoveStateMessage(json_obj.payload);
        }
        else if (json_obj.header.subtype.toUpperCase() === "REMOVE_CONFIG") {
            handleRemoveConfigMessage(json_obj.payload);
        }
        else if (json_obj.header.subtype.toUpperCase() === "REMOVE_MESSAGES") {
            handleRemoveMessagesMessage(json_obj.payload);
        }
        else if (json_obj.header.subtype.toUpperCase() === "SYSTEMCONFIG") {
            handleSystemConfigMessage(json_obj.payload);
        }
        else if (json_obj.header.subtype.toUpperCase() === "HEARTBEAT") {
            if (json_obj.header.timestamp != null && json_obj.header.timestamp != undefined) {
                var timeStamp = json_obj.header.timestamp;
                var d = new Date().getTime();
                timeOffsetMs = d - timeStamp;
//                console.log("Received HB Time Offset: " + timeOffsetMs);
            }
        }
	} else if (json_obj.header.type.toUpperCase() === "COMMAND") {
	    handleCommandMessage(json_obj.payload);
	} else {
	    console.log("Message:" + msg);
	}
}

function LogOut() {
    removePlugins();
    $(".logoutBtn").css("display", "none");
    $("[id=page1]").css("display", "none");
    if (socketObj.connected) {
        $("[id=loginPage]").css("display", "");
        $("[id=connectingPage]").css("display", "none");
    } else {
        $("[id=loginPage]").css("display", "none");
        $("[id=connectingPage]").css("display", "");
    }
    currUser = null;
    $(".userLabel").html("");
    $(".permissionsLabel").html("");
    $("#machineName").html("");
    permissions = 0;
}

function handleSystemConfigMessage(msgData) {
    // TODO Create a table for all information and set the values
    // TODO Add ability to modify
    for (var i=0; i<msgData.length; i++) {
        var item = msgData[i];
        if (msgData[i].name == "SYSTEM_NAME")
        {
            $("#machineName").html(item.value);
            break;
        }
    }
}

function handleCommandMessage(msgData) {
    // TODO: Add logout functionality

    if (msgData["command"] != null && msgData["command"] != undefined) {
        if (msgData["command"].toUpperCase() == "LOGIN") {
            if (msgData["status"] != null && msgData["status"] != undefined) {
                var status = msgData["status"].toUpperCase();
                if (status == "SUCCESS") {
                    if (msgData["level"] != null && msgData["level"] != undefined) {
                        var role = "No Permissions";
                        permissions = msgData["level"];
                        if (msgData["level"] == "1") {
                            // TODO : Read Only – Can view everything but cannot change anything
                            $("[id=\"uploadButton\"]").css("display", "none");
                            role = "Read Only";
                        } else if (msgData["level"] == "2") {
                            // TODO : Application Administrator – Can change all settings on the plugins as well as add new plugins
                            $("[id=\"uploadButton\"]").css("display", "");
                            role = "Application Administrator";
                        } else if (msgData["level"] == "3") {
                            // TODO : System Administrator – Can do all Application Administrator functions plus add/remove/change passwords of users
                            $("[id=\"uploadButton\"]").css("display", "");
                            role = "System Administrator";
                        } else {
                            // TODO : Let them see nothing...
                            $("[id=\"uploadButton\"]").css("display", "none");
                            // TODO: Switch to Log out instead of login 
                            return;
                        }
                        $("[id=loginFeedback]").html("");
                        $("[id=loginPage]").css("display", "none");
                        $("[id=page1]").css("display", "");
                        $(".logoutBtn").css("display", "");
                        $(".userLabel").html(currUser);
                        $(".permissionsLabel").html("(" + role + ")");
                    }
                } else if (status == "FAILED") {
                    var feedback = status;
                    if (msgData["reason"] != null && msgData["reason"] != undefined) {
                        feedback += " : " + msgData["reason"];
                    }
                    $("[id=loginFeedback]").html(feedback);
                }
            }
        } else if (msgData["command"].toUpperCase() == "LOGOUT") {
            if (msgData["status"] != null && msgData["status"] != undefined) {
                var status = msgData["status"].toUpperCase();
                if (status == "SUCCESS") {
                    LogOut();
                } else {
                    // TODO?
                }
            }
        }else if (msgData["command"].toUpperCase() == "CLEARLOG") {
            if (msgData["status"] != null && msgData["status"] != undefined) {
                var status = msgData["status"].toUpperCase();
                if (status == "SUCCESS") {
                    $("[id=eventLogTable]").DataTable().clear().draw('full-hold');
                } else {
                    // TODO?
                }
            }
        }
    }
}

function removePlugins()
{
	var pluginList = document.getElementById("pluginList");
    if (pluginList != null && pluginList != undefined) {
		pluginList.innerHTML = "";
		 pluginsInitialized = false;
    }
    $("[id=messagesTable]").DataTable().clear().draw('full-hold');
    $("[id=eventLogTable]").DataTable().clear().draw('full-hold');
}

function handleRemoveListMessage(msgData) {
    var plugins = Object.keys(msgData);
    for (var j = 0; j < plugins.length; j++) {
        var table = $("[id=\"infoTable_" + plugins[j] + "\"");
        if (table != null && table != undefined) {
            var object = msgData[plugins[j]];
            var jsonPosArray = [];
            var pos = 0;
            var info = "";
            var prefix = "";
            while (typeof (object) == "object") {
                var items = Object.keys(object);
                for (var i = pos; i < items.length; i++) {
                    if (typeof (object[items[i]]) == "object") {
                        jsonPosArray.push({ obj: object, pos: i + 1, prefix: prefix });
                        prefix += items[i] + "->";
                        object = object[items[i]];
                        items = Object.keys(object);
                        pos = 0;
                        i = 0;
                        break;
                    } else {
                        if ((prefix + items[i]).toUpperCase() == "ENABLED") {
                            RefreshHeaderStatus(plugins[j], "");
                        } else if ((prefix + items[i]).toUpperCase() == "VERSION") {
                            $("div[id=\"pluginHeader_" + plugins[j] + "\"] > .headerVersion").html("");
                        } else if ((prefix + items[i]).toUpperCase() == "DESCRIPTION") {
                            $("div[id=\"infoHeader_" + plugins[j] + "\"]").html("");
                        }
                        var body = table[0].querySelector("tbody");
                        if (body != null && body != undefined) {
                            for (var k = 0; k < body.children.length; k++) {
                                if (body.children[k].children.length > 1) {
                                    if (body.children[k].children[0].innerHTML == (prefix + items[i])) {
                                        body.removeChild(body.children[k]);
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }

                if (object == msgData[plugins[j]]) {
                    break;
                }

                if (i >= items.length) {
                    var infoArray = jsonPosArray.pop();
                    object = infoArray.obj;
                    pos = infoArray.pos;
                    prefix = infoArray.prefix.replace(new RegExp(items[i] + "->"), "");
                }
            }
        }
    }
}

function handleListMessage(msgData) {
    if (permissions < 1 || permissions > 3) {
        return;
    }
    var plugins = Object.keys(msgData);
    for (var j = 0; j < plugins.length; j++) {
        var pluginObj = $(".plugin[data-target=\"" + plugins[j] + "\"]");
        if (pluginObj.length <= 0) {
            var pluginList = document.getElementById("pluginList");
            pluginList.innerHTML += "<div class='plugin' data-target='" + plugins[j] + "'></div>";
            new PluginDisplay($(".plugin[data-target=\"" + plugins[j] + "\"]")[0], msgData[plugins[j]]);
        } else {
            var object = msgData[plugins[j]];
            var jsonPosArray = [];
            var pos = 0;
            var info = "";
            var prefix = "";
            while (typeof (object) == "object") {
                var items = Object.keys(object);
                for (var i = pos; i < items.length; i++) {
                    if (typeof (object[items[i]]) == "object") {
                        jsonPosArray.push({ obj: object, pos: i + 1, prefix: prefix });
                        prefix += items[i] + "->";
                        object = object[items[i]];
                        items = Object.keys(object);
                        pos = 0;
                        i = 0;
                        break;
                    } else {
                        updateListObject(plugins[j], prefix + items[i], object[items[i]]);
                    }
                }

                if (object == msgData[plugins[j]]) {
                    break;
                }

                if (i >= items.length) {
                    var infoArray = jsonPosArray.pop();
                    object = infoArray.obj;
                    pos = infoArray.pos;
                    prefix = infoArray.prefix.replace(new RegExp(items[i] + "->"), "");
                }
            }
        }
    }
    updatePluginFilter();

}

function handleRemoveConfigMessage(msgData) {
    var plugins = Object.keys(msgData);
    for (var j = 0; j < plugins.length; j++) {
        var table = $("[id=\"configsTable_" + plugins[j] + "\"");
        if (table != null && table != undefined) {
            var object = msgData[plugins[j]];
            var items = Object.keys(object);
            for (var i = 0; i < items.length; i++) {
                if (typeof (object[items[i]]) == "object") {
                    var body = table[0].querySelector("tbody");
                    if (body != null && body != undefined) {
                        for (var k = 0; k < body.children.length; k++) {
                            if (body.children[k].children.length > 1) {
                                if (body.children[k].children[0].innerHTML == items[i]) {
                                    body.removeChild(body.children[k]);
                                    break;
                                }
                            }

                        }
                    }
                }
            }
        }
    }
}

function handleConfigMessage(msgData) {
    if (permissions < 1 || permissions > 3) {
        return;
    }
//    console.log("Found Config Message");
//    console.log(msgData);
    var plugins = Object.keys(msgData);
    for (var j = 0; j < plugins.length; j++) {
        var pluginObj = $(".plugin[data-target=\"" + plugins[j] + "\"]");
        if (pluginObj.length <= 0) {
            var pluginList = document.getElementById("pluginList");
            pluginList.innerHTML += "<div class='plugin' data-target='" + plugins[j] + "'></div>";
            new PluginDisplay($(".plugin[data-target=\"" + plugins[j] + "\"]")[0], msgData[plugins[j]]);
        } else {
            var object = msgData[plugins[j]];
            var jsonPosArray = [];
            var pos = 0;
            var info = "";
            var prefix = "";

            var configItem = null;
            var val = null;
            var defaultVal = null;
            var description = null;

            while (typeof (object) == "object") {
                var items = Object.keys(object);
                for (var i = pos; i < items.length; i++) {
                    if (typeof (object[items[i]]) == "object") {
                        jsonPosArray.push({ obj: object, pos: i + 1, prefix: prefix });
                        prefix += items[i] + "->";
                        configItem = items[i];
                        object = object[items[i]];
                        items = Object.keys(object);
                        pos = 0;
                        i = 0;
                        break;
                    } else {
                        var key = items[i].toUpperCase();
                        if (key == "VALUE") {
                            val = object[items[i]];
                        } else if (key == "DEFAULTVALUE") {
                            defaultVal = object[items[i]];
                        } else if (key == "DESCRIPTION") {
                            description = object[items[i]];
                        }
                    }
                }
                if (object == msgData[plugins[j]]) {
                    break;
                }

                if (i >= items.length) {
                    var infoArray = jsonPosArray.pop();
                    object = infoArray.obj;
                    pos = infoArray.pos;
                    prefix = infoArray.prefix.replace(new RegExp(items[i] + "->"), "");
                    updateConfigurationObject(plugins[j], configItem, val, defaultVal, description);
                }
            }
        }
    }
//    console.log("Config Finished");
    return;
}


function handleRemoveStateMessage(msgData) {
    var plugins = Object.keys(msgData);
    for (var j = 0; j < plugins.length; j++) {
        var table = $("[id=\"stateTable_" + plugins[j] + "\"");
        if (table != null && table != undefined) {
            var object = msgData[plugins[j]];
            var jsonPosArray = [];
            var pos = 0;
            var info = "";
            var prefix = "";
            while (typeof (object) == "object") {
                var items = Object.keys(object);
                for (var i = pos; i < items.length; i++) {
                    if (typeof (object[items[i]]) == "object") {
                        jsonPosArray.push({ obj: object, pos: i + 1, prefix: prefix });
                        prefix += items[i] + "->";
                        object = object[items[i]];
                        items = Object.keys(object);
                        pos = 0;
                        i = 0;
                        break;
                    } else {
                        var body = table[0].querySelector("tbody");
                        if (body != null && body != undefined) {
                            for (var k = 0; k < body.children.length; k++) {
                                if (body.children[k].children.length > 1) {
                                    if (body.children[k].children[0].innerHTML == (prefix + items[i])) {
                                        body.removeChild(body.children[k]);
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }

                if (object == msgData[plugins[j]]) {
                    break;
                }

                if (i >= items.length) {
                    var infoArray = jsonPosArray.pop();
                    object = infoArray.obj;
                    pos = infoArray.pos;
                    prefix = infoArray.prefix.replace(new RegExp(items[i] + "->"), "");
                }
            }
        }
    }
}

function handleStateMessage(msgData) {
    if (permissions < 1 || permissions > 3) {
        return;
    }
//    console.log("Found Telemetry - State Message");
    var plugins = Object.keys(msgData);
    for (var j = 0; j < plugins.length; j++) {
        var pluginObj = $(".plugin[data-target=\"" + plugins[j] + "\"]");
        if (pluginObj.length <= 0) {
            var pluginList = document.getElementById("pluginList");
            pluginList.innerHTML += "<div class='plugin' data-target='" + plugins[j] + "'></div>";
            new PluginDisplay($(".plugin[data-target=\"" + plugins[j] + "\"]")[0], msgData[plugins[j]]);
        } else {
            var object = msgData[plugins[j]];
            var jsonPosArray = [];
            var pos = 0;
            var info = "";
            var prefix = "";
            while (typeof (object) == "object") {
                var items = Object.keys(object);
                for (var i = pos; i < items.length; i++) {
                    if (typeof (object[items[i]]) == "object") {
                        jsonPosArray.push({ obj: object, pos: i + 1, prefix: prefix });
                        prefix += items[i] + "->";
                        object = object[items[i]];
                        items = Object.keys(object);
                        pos = 0;
                        i = 0;
                        break;
                    } else {
                        updateStateObject(plugins[j], prefix + items[i], object[items[i]]);
                    }
                }

                if (object == msgData[plugins[j]]) {
                    break;
                }

                if (i >= items.length) {
                    var infoArray = jsonPosArray.pop();
                    object = infoArray.obj;
                    pos = infoArray.pos;
                    prefix = infoArray.prefix.replace(new RegExp(items[i] + "->"), "");
                }
            }
        }
    }
}

function handleEventsMessage(msgData)
{
//    console.log("Found Telemetry - Events Message");
    //console.log(msgData);
    for (var i in msgData) 
    {

        updateEventsObject(msgData[i].description, msgData[i].id, msgData[i].level, msgData[i].source, msgData[i].timestamp);
    }
    //table.columns.adjust().draw();
    $("[id=eventLogTable]").DataTable().columns.adjust().draw('full-hold');
    calculateRows();

//    console.log("Events Finished");
}

function handleRemoveMessagesMessage(msgData) {

    var combinedTable = $("[id=messagesTable]").DataTable();

    var plugins = Object.keys(msgData);
    for (var j = 0; j < plugins.length; j++) {
        var table = $("[id=\"messagesTable_" + plugins[j] + "\"");
        if (table != null && table != undefined) {
            var object = msgData[plugins[j]];
            var jsonPosArray = [];
            var pos = 0;
            var type = null;
            var subtype = null;
            var items = Object.keys(object);
            if (Array.isArray(msgData[plugins[j]])) {
                for (var l = 0; l < msgData[plugins[j]].length; l++) {
                    object = msgData[plugins[j]][l];
                    while (typeof (object) == "object") {
                        var items = Object.keys(object);
                        for (var i = pos; i < items.length; i++) {
                            if (typeof (object[items[i]]) == "object") {
                                jsonPosArray.push({ obj: object, pos: i + 1 });
                                object = object[items[i]];
                                items = Object.keys(object);
                                pos = 0;
                                i = 0;
                                break;
                            } else {
                                var key = items[i].toUpperCase();
                                if (key == "TYPE") {
                                    type = object[items[i]];
                                } else if (key == "SUBTYPE") {
                                    subtype = object[items[i]];
                                }
                                if (type != null && subtype != null) {
                                    var body = table[0].querySelector("tbody");
                                    if (body != null && body != undefined) {
                                        for (var k = 0; k < body.children.length; k++) {
                                            if (body.children[k].children.length > 1) {
                                                if (body.children[k].children[0].innerHTML == type && body.children[k].children[1].innerHTML == subtype) {
                                                    body.removeChild(body.children[k]);
                                                }
                                            }
                                        }
                                    }
                                    var indexes = combinedTable.rows().eq(0).filter(function (rowIndex) {
                                        return ((combinedTable.cell(rowIndex, 0).data() === plugins[j]) && (combinedTable.cell(rowIndex, 1).data() === type) && (combinedTable.cell(rowIndex, 2).data() === subtype)) ? true : false;
                                    });

                                    if (indexes.length > 0) {
                                        for (var i = 0; i < indexes.length; i++) {
                                            combinedTable.rows(indexes).remove().draw('full-hold');
                                        }
                                    }
                                }
                            }
                        }

                        if (object == msgData[plugins[j]][l]) {
                            break;
                        }

                        if (i >= items.length) {
                            var infoArray = jsonPosArray.pop();
                            object = infoArray.obj;
                            pos = infoArray.pos;
                        }
                    }

                }
            }

        }
    }
}

function handleMessagesMessage(msgData) {
    if (permissions < 1 || permissions > 3) {
        return;
    }
    //    console.log("Found Telemetry - Messages Message");
    var plugins = Object.keys(msgData);
    for (var j = 0; j < plugins.length; j++) {
        var pluginObj = $(".plugin[data-target=\"" + plugins[j] + "\"]");
        if (pluginObj.length <= 0) {
            var pluginList = document.getElementById("pluginList");
            pluginList.innerHTML += "<div class='plugin' data-target='" + plugins[j] + "'></div>";
            new PluginDisplay($(".plugin[data-target=\"" + plugins[j] + "\"]")[0], msgData[plugins[j]]);
        } else {
            var registerArray = [];
            var object = msgData[plugins[j]];
            var jsonPosArray = [];
            var pos = 0;
            var info = "";
            var prefix = "";
            if (Array.isArray(msgData[plugins[j]])) {
                for (var l = 0; l < msgData[plugins[j]].length; l++) {
                    object = msgData[plugins[j]][l];
                    var id = null;
                    var type = null;
                    var subtype = null;
                    var count = null;
                    var lastTimestamp = null;
                    var averageInterval = null;
                    while (typeof (object) == "object") {
                        var items = Object.keys(object);
                        for (var i = pos; i < items.length; i++) {
                            if (typeof (object[items[i]]) == "object") {
                                jsonPosArray.push({ obj: object, pos: i + 1, prefix: prefix });
                                prefix += items[i] + "->";
                                object = object[items[i]];
                                items = Object.keys(object);
                                pos = 0;
                                i = 0;
                                break;
                            } else {
                                var key = items[i].toUpperCase();
                                if (key == "ID") {
                                    id = object[items[i]];
                                } else if (key == "TYPE") {
                                    type = object[items[i]];
                                } else if (key == "SUBTYPE") {
                                    subtype = object[items[i]];
                                } else if (key == "COUNT") {
                                    count = object[items[i]];
                                } else if (key == "LASTTIMESTAMP") {
                                    lastTimestamp = object[items[i]];
                                } else if (key == "AVERAGEINTERVAL") {
                                    averageInterval = object[items[i]];
                                }
                            }
                        }

                        if (object == msgData[plugins[j]][l]) {
                            break;
                        }

                        if (i >= items.length) {
                            var infoArray = jsonPosArray.pop();
                            object = infoArray.obj;
                            pos = infoArray.pos;
                            prefix = infoArray.prefix.replace(new RegExp(items[i] + "->"), "");
                        }
                    }
                    updateMessagesObject(plugins[j], id, type, subtype, count, lastTimestamp, averageInterval);
                }
            }
        }
    }
}

function handleRemoveStatusMessage(msgData) {
    //    console.log("Found Telemetry - Status Message");
    var plugins = Object.keys(msgData);
    for (var j = 0; j < plugins.length; j++) {
        updateStatusObject(plugins[j], "Status", "");
    }
}

function handleStatusMessage(msgData) {
//    console.log("Found Telemetry - Status Message");
    var plugins = Object.keys(msgData);
    for (var j = 0; j < plugins.length; j++) {
        updateStatusObject(plugins[j], "Status", msgData[plugins[j]]);
    }
}

// Utilities

function generateUUID(){
    var d = new Date().getTime();
    if(window.performance && typeof window.performance.now === "function"){
        d += performance.now(); //use high-precision timer if available
    }
    var uuid = 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, function(c) {
        var r = (d + Math.random()*16)%16 | 0;
        d = Math.floor(d/16);
        return (c=='x' ? r : (r&0x3|0x8)).toString(16);
    });
    return uuid;
}


/**
*   Generate timestamp for the given time.
*   @param {Date} time - Given time
*   @returns {string} Text containing the hours, minutes, seconds, and milliseconds of given time
**/
function timeStamp(time)
{
	if (time == null)
	    time = new Date();
	var timeString = "";
    
	var hours = time.getHours().toString();
	if (hours.length == 1) {
	    hours = "0" + hours;
	}

	var minutes = time.getMinutes().toString();
	if (minutes.length == 1) {
	    minutes = "0" + minutes;
	}

	var seconds = time.getSeconds().toString();
	if (seconds.length == 1) {
	    seconds = "0" + seconds;
	}

	var timeString = hours + ":" + minutes + ":" + seconds;// + "." + time.getMilliseconds();
	return timeString;
}

function fullTimeStamp()
{
	var time = new Date();
	var timeString = "";
    
	var hours = time.getHours().toString();
	if (hours.length == 1) {
	    hours = "0" + hours;
	}

	var minutes = time.getMinutes().toString();
	if (minutes.length == 1) {
	    minutes = "0" + minutes;
	}

	var seconds = time.getSeconds().toString();
	if (seconds.length == 1) {
	    seconds = "0" + seconds;
	}

	var timeString = time.toLocaleDateString() + "  " + hours + ":" + minutes + ":" + seconds + "." + time.getMilliseconds();
	return timeString;
}

function trimLineEndings(input)
{
	return input.replace(/(\r\n|\n|\r)/gm,"");
}
function sendClearLog()
{
    var d = new Date().getTime() - timeOffsetMs;
    var msg = '{"header": {"type": "Command", "subtype": "Execute", "encoding": "jsonstring","timestamp": "' + d + '","flags": "0"},' +
    '"payload": {"command": "clearlog", "id": "' + cmdCntr + '", "args": {}}}';

    cmdCntr++;
    console.log(msg);
    sendCommand(msg);
}

function generateAndSendCommandMessage(cmd, cmdArgs) {
    var d = new Date().getTime() - timeOffsetMs;
    var msg = '{"header": {"type": "Command","subtype": "Execute","encoding": "jsonstring","timestamp": "' + d + '","flags": "0"},' +
              '"payload": {"command": "' + cmd + '", "id": "' + cmdCntr + '", ' +
              '"args": {';

    for (var i = 0; i < cmdArgs.length; i++) {
        msg += '"' + cmdArgs[i].name + '": "' + cmdArgs[i].value + '"';
        if (i < cmdArgs.length - 1) {
            msg += ", ";
        }
    }
    msg += '}}}';
    cmdCntr++;

    console.log(msg);
    sendCommand(msg);
}

function SendLoginCredentials() {
    if ($("#uname").val() == "") {
        $("#uname").css("background-color", "red");
        $("#loginFeedback").html("Username required");
        return;
    }
    else  $("#uname").css("background-color", "");
    
    if ($("#upwd").val() == "") {
        $("#upwd").css("background-color", "red");
        $("#loginFeedback").html("Password required");
        return;
    }
    else $("#upwd").css("background-color", "");

    var uname = $("#uname").val().replace(/\\(.)/mg, "").replace(/&/g, "&#038;").replace(/</g, "&#060;").replace(/>/g, "&#062;").replace(/"/g, "&#034;").replace(/'/g, "&#039;").replace(/\//g, "&#047;").replace(/\\/g, "&#092;").trim();
    var d = new Date().getTime() - timeOffsetMs;
    var msg = '{"header": {"type": "Command","subtype": "Execute","encoding": "jsonstring","timestamp": "' + d + '","flags": "0"},' +
              '"payload": {"command": "login", "id": "' + cmdCntr + '", "args": {"user": "' + uname + '","password": "' + $("#upwd").val() + '"}}}';
    currUser = uname;
    $("#uname").val("");
    $("#upwd").val("");
    $("#uname").css("background-color", "");
    $("#upwd").css("background-color", "");
    $("#loginFeedback").html("");
    cmdCntr++;

    sendCommand(msg);
}

function SendLogoutCommand() {
    generateAndSendCommandMessage("logout", []);
}

// Socket Functions ------------------------------------------------------
function sendCommand(msg)
{
	var sent = false;
	msg = "\x02" + msg + "\x03";
    var encode = window.btoa(msg);
    
    if (socketObj.socket.readyState === socketObj.socket.OPEN)
    {
        socketObj.socket.send(encode);
        sent = true;
    }
    else 
    {
        updateConnected(socketObj.url, false);
    }
}

/**
*   @private
**/
function updateConnected(url, connected) {
    // If found update all objects in that telemetry array
    if (socketObj.url == url) {
        socketObj.connected = connected;
        if (connected) {
            if (socketObj.timerID) { /* a setInterval has been fired */
                window.clearInterval(socketObj.timerID);
                socketObj.timerID = 0;
            }
            var led = document.getElementById(socketObj.led);
            if (led != null && led != undefined) {
                led.setAttribute('src', '../images/Battelle/WhiteLockup.png');
            }

            var event = new CustomEvent("connect", {
                detail: {
                    time: new Date(),
                },
            });
            eventElement.dispatchEvent(event);
        }
        else {
            if (!socketObj.timerID) {
                socketObj.timerID = setInterval(function () { connect(url) }, 10000);
                var led = document.getElementById(socketObj.led);
                if (led != null && led != undefined) {
                    led.setAttribute('src', '../images/Battelle/GrayLockup.png');
                }
                noData(socketObj);
                console.log("Disconnected...........");
                removePlugins();
                $("[id=\"connectingPage\"]").css("display", "");
                $("[id=\"loginPage\"]").css("display", "none");
                $("[id=\"page1\"]").css("display", "none");
                // TODO: Do we need to null out the socket?
            }
        }
    }
}

function noData(obj) {
    removePlugins();
}

/**
*   @private
**/
function connectInitial()
{
//	for (var i = 0; i < connections.length; i++) 
//	{
	    var tempConnection = connections[0];
        var tempIP = tempConnection.ip;

//        setUrlCookie(tempConnection.ip);
        console.log("Cookie URL:" + getUrlCookie());
        var cookieUrl = getUrlCookie();
        if (cookieUrl != "")
        {
            tempIP = cookieUrl;
            
        }
        $(".ipInput").val(tempIP);
		socketObj = createSocketObject(tempIP, tempConnection.port, tempConnection.led);
        if (useSSL) socketObj.url = "wss://" + tempIP + ':' + socketObj.port + "/";
        else socketObj.url = "ws://" + tempIP + ':' + socketObj.port + "/";
		connect(socketObj.url);
	}

//}

String.prototype.endsWith = function(str)
{
    var lastIndex = this.lastIndexOf(str);
    return (lastIndex !== -1) && (lastIndex + str.length === this.length);
}

/**
*   Socket object must already be in the socketList when this function is called.
*   @private
**/ 
var decode;
function connect(url)
{
    if (!$(".ipInput").is(":focus")) {
        $(".ipInput").val(socketObj.ip);
    }

    var msgBuffer = "";
    var endsWithNewline = false;
    
	if (socketObj.url == url) {
	    if (!socketObj.connecting) {
            if (socketObj.socket != null)
            {
                socketObj.socket.close();
                if (socketObj.socket.readyState == 2)
                {
                    setTimeout(function () { connect(url) }, 500);
                    return;
                }
            }
            console.log(timeStamp() + "  Connect to " + socketObj.url + "++++++++++++++++++++");
	        socketObj.socket = new WebSocket(socketObj.url, 'base64');
	        socketObj.socket.connecting = true;
            $("#loaderLabel").html("Connecting to " + url + " ...");

	        socketObj.socket.onopen = function (e) {
	            socketObj.socket.connecting = false;
	            updateConnected(e.target.url, true);
	            socketObj.startTimer();
                $("[id=\"connectingPage\"]").css("display", "none");
                $("[id=\"loginPage\"]").css("display", "");
                $("[id=\"page1\"]").css("display", "none");
	        }
	        socketObj.socket.onmessage = function (e) {
	            clearTimeout(socketObj.timeoutID);
	            socketObj.timeoutID = null;
	            socketObj.startTimer();
	            decode = window.atob(e.data);
	            // Check for multiple packets in message
	            msgBuffer += decode;
	            // Look for the end of text character as the message terminator
	            endsWithNewline = msgBuffer.endsWith("\x03");
	            if (msgBuffer.indexOf("\x03") >= 0) {
	                var newMessages = msgBuffer.split("\x03");
	                for (var i = 0; i < newMessages.length; i++) {
	                    if (i < newMessages.length - 1) {
	                        if (newMessages[i].length > 0) {
	                            messageHandler(newMessages[i]);
	                        }
	                    }
	                    else {
	                        if (endsWithNewline) {
	                            if (newMessages[i].length > 0) {
	                                messageHandler(newMessages[i]);
	                            }
	                            msgBuffer = "";
	                        }
	                        else {
	                            msgBuffer = newMessages[i];
	                        }
	                    }
	                }
	            }
	        }
	        socketObj.socket.onclose = function (e) {
	            clearTimeout(socketObj.timeoutID);
	            socketObj.timeoutID = null;
	            updateConnected(e.target.url, false);
	            socketObj.socket.connecting = false;
	            LogOut();
	        }
	        socketObj.socket.onerror = function (e) {
	            clearTimeout(socketObj.timeoutID);
	            socketObj.timeoutID = null;
	            LogOut();

	            updateConnected(e.target.url, false);
	            socketObj.socket.connecting = false;
	        }
            
	    }
	    else {
	    }
	}
}


function round(value, decimals) {
    return Number(Math.round(value+'e'+decimals)+'e-'+decimals);
}

function removeClassMatch(div, matchStr) {
	var reg = new RegExp("(^|\\s)"+matchStr+"\\S+", 'gi');
	div.removeClass (function (index, className, reg) {
    	return (className.match (reg || []).join(' '));
//    return (className.match (/(^|\s)z-index-\S+/g) || []).join(' ');
 //   ~'foo'.indexOf('oo')
});
}

/**
 * jQuery alterClass plugin
 *
 * Remove element classes with wildcard matching. Optionally add classes:
 *   $( '#foo' ).alterClass( 'foo-* bar-*', 'foobar' )
 *
 * Copyright (c) 2011 Pete Boere (the-echoplex.net)
 * Free under terms of the MIT license: http://www.opensource.org/licenses/mit-license.php
 *
 */
(function ( $ ) {
	
$.fn.alterClass = function ( removals, additions ) {
	
	var self = this;
	
	if ( removals.indexOf( '*' ) === -1 ) {
		// Use native jQuery methods if there is no wildcard matching
		self.removeClass( removals );
		return !additions ? self : self.addClass( additions );
	}

	var patt = new RegExp( '\\s' + 
			removals.
				replace( /\*/g, '[A-Za-z0-9-_]+' ).
				split( ' ' ).
				join( '\\s|\\s' ) + 
			'\\s', 'g' );

	self.each( function ( i, it ) {
		var cn = ' ' + it.className + ' ';
		while ( patt.test( cn ) ) {
			cn = cn.replace( patt, ' ' );
		}
		it.className = $.trim( cn );
	});

	return !additions ? self : self.addClass( additions );
};

})( jQuery );

// ------------------- AUDIO Utilities ------------------------


function startSoundTimer(duration)
{
    SoundTimerID = setTimeout(SoundTimeout, duration);
}

function restartSoundTimeout()
{
    stopSoundTimer();
    startSoundTimer(defaultSoundTimeout);
}

function stopSoundTimer()
{
    if (SoundTimerID != null)
    {
        clearTimeout(SoundTimerID);
        SoundTimerID = null;
    }    
}
function SoundTimeout()
{
    console.log("Sound Timeout Clearing");
    currentAudio = -1;
}

function preloadAudio(url) {
    var audio = new Audio();
    audio.src = url;
    audio.preload = "auto";
}

var player = document.getElementById('player');
function playAudio(index, audioFiles) {
    console.log("Playing Audio index:" + index + " Current Audio:" + currentAudio);
    if (soundNotification)
    {
        if (index != currentAudio)
        {
            currentAudio = index;
            stopAudio();
            player.src = audioFiles[index];
            player.playbackRate = 1.0;
            player.play();

            restartSoundTimeout(defaultSoundTimeout);
			hisAudio.payload.Id = generateUUID();
            hisAudio.payload.HISPostState = audioFiles[index];
        }
    }
}

function stopAudio()
{
    player.pause();
    player.currentTime = 0;
}

function changeIPAddress()
{
    socketObj.socket.close();

    console.log(socketObj.ip);
    setUrlCookie(socketObj.ip);
    
    if (useSSL) socketObj.url = "wss://" + socketObj.ip + ':' + socketObj.port + "/";
    else socketObj.url = "ws://" + socketObj.ip + ':' + socketObj.port + "/";
    socketObj.connected = false;
    socketObj.connecting = false;
    updateConnected(socketObj.url, false);
    window.clearInterval(socketObj.timerID);
    socketObj.timerID = 0;
    connect(socketObj.url);

}

function calculateRows()
{
    var numRows = Math.ceil(($(window).height() - $(".headercontainer").height() - $("#tabs > div >  ul").height()) / 50);
    if (numRows < 1) {
        numRows = 1;
    }
    $("#messagesTable").DataTable().page.len(numRows).draw('full-hold');
    $("#eventLogTable").DataTable().page.len(numRows).draw('full-hold');    
}
$(document).ready(function () {

    if (document.getElementById('server') != null) {
        document.getElementById('server').innerHTML = connections[0].ip;
    }

    if (document.getElementById('port') != null) {
        document.getElementById('port').innerHTML = connections[0].port;
    }

    var numRows = Math.ceil(($(window).height() - $(".headercontainer").height() - $("#tabs > div >  ul").height()) / 50);
    if (numRows < 1) {
        numRows = 1;
    }
    $('#messagesTable').DataTable({
        "bLengthChange": false,
        "order": [4, 'desc']
    });
    $('#eventLogTable').DataTable({
        "bLengthChange": false,
        "order": [3, 'desc']
        ,
        "columns": [{
            "width": "8%"
        }, {
            "width": "15%"
        }, {
            "width": "57%"
        }, {
            "width": "20%"
        }]
    });

    $( function() {
        $('input[type="checkbox"]').checkboxradio();
    } );

    $(window).resize(function () {
        calculateRows();
    });

    window.addEventListener("keypress", function (event) {
        if (event.which == 13) {
            var input = $("input:focus");
            if (input != null && input != undefined) {
                if (input.hasClass("configInput")) {
                    var newValue = input.val();
                    generateAndSendCommandMessage("set", [{ name: "plugin", value: input.attr("data-plugin") }, { name: "key", value: input.attr("data-name") }, { name: "value", value: newValue.replace(/"/g, "\\\"") }]);
                    input.val(input.attr("data-value"));
                    input.css("background-color", "#909090");
                    input.css("color", "white");
                } else if (input.hasClass("ipInput")) {
                    if (input.attr("data-valid") != "false") {
                        socketObj.ip = input.val();
                        changeIPAddress();
                    }
                }
            }
        }
    });

    $("input.ipInput").on("input", function (event) {
        var match = this.value.search(/[^\.0-9]+/g);

        if (match != -1) {
            this.value = this.value.substring(0, this.value.length - 1);
        } else {
            var invalid = "true";
            var tokens = this.value.split(".");
            if (tokens.length > 4) {
                this.value = this.value.substring(0, this.value.length - 1);
                return;
            } else if (tokens.length < 4 && tokens[0] != "---") {
                invalid = "false";
            }

            for (var i = 0; i < tokens.length; i++) {
                var num = parseFloat(tokens[i]);
                if ((!isNaN(tokens[i]) && (num < 0 || num > 255)) || tokens[i] == "") {
                    invalid = "false";
                    break;
                }
            }

            this.setAttribute("data-valid", invalid);
        }
    });

    $("input.portInput").on("input", function (event) {
        var match = this.value.search(/[^0-9]+/g);
        if (match != -1) {
            this.value = this.value.substring(0, this.value.length - 1);
        } else {
            if (this.value < 0 || this.value > 65535) {
                this.setAttribute("data-valid", "false");
            } else {
                this.setAttribute("data-valid", "true");
            }
        }
    });

    connectInitial();

});


// jquery UI initialization
$( function() {
   $( "#tabs" ).tabs();
} );

$( function() {
    $( ".widget input[type=submit], .widget a, .widget button" ).button();
} );
