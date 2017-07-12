console.log("including tmx.common.library.js");
var commonLibraryJsVersion = "0.0.1";
var commonBackgroundImage = "../images/Common/Background.png";
var activeBackgroundImage = "../images/Common/Background.png";
var VTRWSeverity = 0;
var CSWSeverity = 0;
var EEBLSeverity = 0;
var FCWSeverity = 0;

var socketTimeout_ms = 600000;

var screenNotification = true;
var soundNotification = true;
var screenNotificationSetting = true;
var soundNotificationSetting = true;

var MaintenanceMode = false;
var OperationalModeDegraded = false;
var DSRCDegraded = false;
var GPSDegraded = false;
var CAN1Degraded = false;

var his = createHisMessageObject();
var hisID = 0;

var hisConfig = createHisMessageObject();
var hisConfigID = 0;

// Audio variable
var hisAudio = createHisMessageObject();
var hisAudioID = 0;
var currentAudio = -1;
var defaultSoundTimeout = 5000;
var SoundTimerID = null;

//Image timeout variables
var timerID = null;
var defaultTimout = 3000;
 
function handleStatusMessage(json_obj)
{
/*
    ModuleTypes - NoModuleType = 0, SystemOpMode = 1, Bluetooth = 2, DSRC = 3, GNSS = 4, Cellular = 5, WiFi = 6, CAN1 = 7, CAN2 = 8
    OperationalModeTypes - NoOpModeType = 0, Standby = 1, Normal = 2, Degraded = 3, Maintenance = 4, Fault = 5, Offline = 6
*/
//	console.log(json_obj);
    switch (json_obj.ModuleType) {
	    case "3":
	        if ((json_obj.OpModeType == "3") || (json_obj.OpModeType == "5"))
	        {
	        	if (!DSRCDegraded) DSRCDegraded = true;
	        }
	        else if (DSRCDegraded) DSRCDegraded = false;
	    break;
	    case "4":
	        if ((json_obj.OpModeType == "3") || (json_obj.OpModeType == "5"))
	        {
	        	if (!GPSDegraded) GPSDegraded = true;
	        }
	        else if (GPSDegraded) GPSDegraded = false;
	    break;
	    case "7":
	        if ((json_obj.OpModeType == "3") || (json_obj.OpModeType == "5"))
	        {
	        	if (!CAN1Degraded) CAN1Degraded = true;
	        }
	        else if (CAN1Degraded) CAN1Degraded = false;
	    break;
	}

//	console.log("DSRC:" + DSRCDegraded + " GPS:" + GPSDegraded + " CAN:" + CAN1Degraded)
    if ((!DSRCDegraded) && (!GPSDegraded) && (!CAN1Degraded))
    {
    	// Not degraded
    	if (OperationalModeDegraded)
    	{
    		OperationalModeDegraded = false;
		    console.log("Transitioning from Degraded DSRC:" + DSRCDegraded + " GPS:" + GPSDegraded + " CAN:" + CAN1Degraded);
		    screenNotification = screenNotificationSetting;
		    soundNotification = soundNotificationSetting;
		    $("#ActiveImage").attr('src', commonBackgroundImage);
		    $("#ActiveImage").removeClass('hidden');
		    // TODO set correctly
		    populateHisImageMessageObject(0, '', 0);
    	}
    }
    else if ((DSRCDegraded) || (GPSDegraded) || (CAN1Degraded))
    {
    	if (!OperationalModeDegraded)
    	{
    		OperationalModeDegraded = true;
    		console.log("Transitioning to Degraded DSRC:" + DSRCDegraded + " GPS:" + GPSDegraded + " CAN:" + CAN1Degraded);
    		screenNotification = false;
    		soundNotification = false;
   		    $("#ActiveImage").removeClass('hidden');
   		    $("#ActiveImage").attr('src', "../images/Common/DegradedMode.png");
   		    // TODO set correctly
		    populateHisImageMessageObject(0, '', 0);
    	}
    }

}

window.addEventListener("message", function(event) {
    //  console.log(event.data);
    var tokenPairs = event.data.split("@#");
    for (var i = 0; i  < tokenPairs.length; i++) {
        var tokens = tokenPairs[i].split("@$");
        switch (tokens[0]) {
            case "sendMsg":
            	console.log("Common Sending:" + tokens[1]);
                sendCommand(tokens[1]);
                break;
            case "clickBtn":
                var btn = window.document.getElementById(tokens[1]);
                if (btn != null && btn != undefined) {
                    $('#' + tokens[1]).mousedown();
		            $(".headbtn").removeClass('ui-btn-active');
                    btn.className += " ui-btn-active";
                }
                break;
            case "updateSoundOption":
                switchPopupOption(tokens[1]);
                break;
        }
    }
},false);

var socketList = [];
var socketItems = 0;

var hostAdd = location.protocol + '//' + location.hostname;
/**
*   Print text to console with a timestamp.
*   @private
*   @param {string} msg - Text to be displayed on console
**/
function log(msg) { console.log(timeStamp() + " " + msg); }

/** 
* Create a new socket connection to the specified IP and port. 
* @private
* @param {Object} devices - Array of devices for this connection
* @param {string} ip - IP Address for socket connection
* @param {string} port - Port for socket connection
* @param {string} led - Name of HTML DOM element to show connections status
* @returns {Object} New socket connection
**/
function createSocketObject(devices, ip, port, led) {
	var socketObject = new Object();
	socketObject.devices = devices;
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



/**
*	Register Object from the HTML implementation. 
**/
function registerObject(obj)
{

}

/**
*	Remove Object from the HTML implementation. 
**/
function deregisterObject(ele)
{

}		


/**
*   Message Handler. All incoming messages are sent to this handler.
*   @private
*   @param {string} - Incoming message
**/
function messageHandler(msg)
{
//	log(msg);
	// Strip control characters
	msg = msg.replace(/[\x00-\x1F\x7F-\x9F]/g, "");
	var json_obj;

	// console.log("Message:" + msg);
	// If message not handled, create an event and dispatch
	var event = new CustomEvent("newMessage", {
		detail: {
			message: msg,
			time: new Date(),
		},
	});
	this.dispatchEvent(event);

}
/**
*   Traverse through all registered telemetry set each value to no data.
*   @private
**/
function noData(device)
{
	
}


function adjustVolume(volumeslider) {
    if (bDisplayOnMain) {
        mainPageWindow.postMessage("adjustVolume@$" + volumeslider.value, mainHtmlPageLink );
    } else {
        window.volume = volumeslider.value / 100.0;
        if (gainOsc != null && gainOsc != undefined) {
            window.gainOsc.gain.value = window.volume;
        }
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


// Socket Functions ------------------------------------------------------
function sendCommand(msg)
{

	var sent = false;
	var tokens = msg.split(";");
	var dev = tokens[0];

//    msg=msg+"\n";
//  	encode=window.btoa(msg);
//	console.log("Sending:" + tokens[1]);
	tokens[1] = "\2" + tokens[1] + "\3";
  	encode=window.btoa(tokens[1]);

    var tempList = socketList;

  	// Look for socket
  	for (var j = 0; j < tempList.length; j++) {
		var tempObj = tempList[j];
		for (var k = 0; k < tempObj.devices.length; k++) {
			var tempDevice = tempObj.devices[k];
			if (dev.toUpperCase().indexOf(tempDevice.toUpperCase()) === 0)
			{
				if (tempObj.socket.readyState === tempObj.socket.OPEN)
				{
					tempObj.socket.send(encode);
					sent = true;
					break;
				}
				else 
				{
					updateConnected(tempObj.url, false);
				}
			}
		}
	} 
//	if (!sent)
//	{
//		console.log("Could not send command - Device not found:" + msg);
//	}
}

/**
*   @private
**/
function updateConnected(url, connected)
{
	for (var j = 0; j < socketList.length; j++) {
		var tempObj = socketList[j];
		// If found update all objects in that telemetry array
		if (tempObj.url == url) 
		{
			tempObj.connected = connected;
			if (connected)
			{
				if(tempObj.timerID) { /* a setInterval has been fired */
	   				window.clearInterval(tempObj.timerID);
	   				tempObj.timerID=0;
 				}
                var led = document.getElementById(tempObj.led);
                if (led != null && led != undefined) {
				    led.setAttribute('src', '../images/Battelle/BattelleLogoBlue.png');
                }

            	var event = new CustomEvent("connect", {
					detail: {
						time: new Date(),
					},
				});
				this.dispatchEvent(event);
			}
			else
			{
				if (!tempObj.timerID) {
  					tempObj.timerID=setInterval(function() { connect(url) }, 10000);
  									// Set no data for devices
                    var led = document.getElementById(tempObj.led);
                    if (led != null && led != undefined) {
					    led.setAttribute('src', '../images/Battelle/BattelleLogoBlack.png');
                    }
					for (var j = 0; j < tempObj.devices.length; j++) {
					    noData(tempObj.devices[j]);
					}
					var event = new CustomEvent("disconnect", {
						detail: {
							time: new Date(),
						},
					});
					this.dispatchEvent(event);
 				}
			}
		}
	}
}

/**
*   @private
**/
function connectInitial()
{
	for (var i = 0; i < connections.length; i++) 
	{
		var tempConnection = connections[i];

//		var tempSocket = createSocketObject(tempConnection.devices, tempConnection.ip, tempConnection.port, tempConnection.led);
//		tempSocket.url = "ws://" + tempSocket.ip + ':' + tempSocket.port + "/"; 
		var tempSocket = createSocketObject(tempConnection.devices, document.location.hostname, tempConnection.port, tempConnection.led);
		tempSocket.url = "ws://" + document.location.hostname + ':' + tempSocket.port + "/"; 
		socketList.push(tempSocket);
		connect(tempSocket.url);
	}

}

String.prototype.endsWith = function(str)
{
    var lastIndex = this.lastIndexOf(str);
    return (lastIndex !== -1) && (lastIndex + str.length === this.length);
}

/**
*   Socket object must already be in the socketList when this function is called.
*   @private
**/ 
function connect(url)
{
	console.log(timeStamp() + "  Connect to " + url + "++++++++++++++++++++");
	for (var j = 0; j < socketList.length; j++) {
		var tempObj = socketList[j];
		if (tempObj.url == url) 
		{
			if (!tempObj.connecting)
			{
				var msgBuffer = "";
				var endsWithNewline = false;
				tempObj.socket = new WebSocket(tempObj.url, 'base64');
				tempObj.socket.connecting = true;

				tempObj.socket.onopen = function(e) {
					tempObj.socket.connecting = false;
					updateConnected(e.target.url, true);
					console.log(e.target.url);
					tempObj.startTimer();
					console.log("Socket Open " + e.target.url);

					var d = new Date();
					var n = d.getTime();
					// Register
					if (uiString != undefined);
					else var connectionString = "";
					var newMess =  "TEST;" + '{"header":{"type":"__register","encoding":"JSON","timestamp":' + n + ',"flags":0},"payload":{"name":"TMX UI ' + uiString + '","description":"UI Interface","version":"1.0.0","exeLocation": "/Debug/UIManafer","coreIpAddr": "127.0.0.1","corePort": 24601,"messageTypes": [],"configuration": []}}';
					sendCommand(newMess);
					// Subscribe
					newMess =  "TEST;" + '{"header":{"type":"__subscribe","timestamp":1463616212696,"flags":0},"payload":[';
					for (var l = 0; l < subscriptions.length; l++) 
					{
						newMess += '{"type":"' + subscriptions[l].type + '","subtype":"' + subscriptions[l].subtype + '","flagmask":0}';
						if (l < subscriptions.length-1) newMess += ",";
					}
					newMess += ']}';
					sendCommand(newMess);
				}
				tempObj.socket.onmessage = function(e) {
					clearTimeout(tempObj.timeoutID);
					tempObj.timeoutID = null;
					tempObj.startTimer();
					decode=window.atob(e.data);
					// Check for multiple packets in message
//					messageHandler(decode);

					msgBuffer += decode;
					// Look for the end of text character as the message terminator
					endsWithNewline = msgBuffer.endsWith("\x03");
					if (msgBuffer.indexOf("\x03") >= 0) 
					{
						var newMessages = msgBuffer.split("\x03");
						for (var i=0; i < newMessages.length; i++)
						{
							if (i <  newMessages.length - 1)
							{
								if (newMessages[i].length > 0)
								{
									messageHandler(newMessages[i]);
								}
							}
							else
							{
								if (endsWithNewline)
								{
									if (newMessages[i].length > 0)
									{
										messageHandler(newMessages[i]);
									}
									msgBuffer = "";
								}
								else
								{
									msgBuffer = newMessages[i];
								}
							}
						}
					}			
				}
				tempObj.socket.onclose = function(e) {
					clearTimeout(tempObj.timeoutID);
					tempObj.timeoutID = null;
					updateConnected(e.target.url, false);
					tempObj.socket.connecting = false;
//					console.log("Socket Closing - " + e.data + " " + e.target.url);
				}
				tempObj.socket.onerror = function(e) {
					clearTimeout(tempObj.timeoutID);
					tempObj.timeoutID = null;

					updateConnected(e.target.url, false);
					tempObj.socket.connecting = false;
//					console.log("Socket Error - " + e.data + " " + e.target.url);
				}		
			}
			else {
//				log(tempObj.url + " Already trying to connect --------------- connecting");
			}
		}
	}
}

function getNotificationMode() {
    var mode = "";
    if (screenNotification && soundNotification) {
        mode = "All";
    } else if (screenNotification && !soundNotification) {
        mode = "Visual";
    } else if (!screenNotification && soundNotification) {
        mode = "Audible";
    } else if (!screenNotification && !soundNotification) {
        mode = "None";
    } else {
        mode = "Unknown";
    }
    return mode;
}

function  handleConfig(json_obj) 
{
//    console.log("Config Key:" + json_obj.key + " Value:" + json_obj.value);
    if (json_obj.key == "NotificationMode")
    {
        var pre = getNotificationMode();
//        console.log("Notification Mode:" + json_obj.value);
        if ((json_obj.value.trim().toLowerCase() == "none") || (json_obj.value == 0))
        {
            console.log("Notification mode: None case");
            screenNotificationSetting = false;
            soundNotificationSetting = false;
            if (!MaintenanceMode)
            {
	            screenNotification = false;
	            soundNotification = false;
	            $("#ActiveImage").addClass('hidden');
	            $("#CloakImage").removeClass('hidden');
	            populateHisConfigMessageObject(pre, "None");
	            sendCommand("TEST;" + JSON.stringify(hisConfig));
        	}
        }
        else if ((json_obj.value.trim().toLowerCase() == "audible") || (json_obj.value == 1))
        {
            console.log("Notification mode: Audible case");
            screenNotificationSetting = false;
            soundNotificationSetting = true;
            if (!MaintenanceMode)
            {
	            screenNotification = false;
	            soundNotification = true;
	            $("#ActiveImage").addClass('hidden');
	            $("#CloakImage").removeClass('hidden');
	            populateHisConfigMessageObject(pre, "Audible");
	            sendCommand("TEST;" + JSON.stringify(hisConfig));
	        }
        }
        else if ((json_obj.value.trim().toLowerCase() == "visual") || (json_obj.value == 2))
        {
        	console.log("Notification mode: Visual case");
            screenNotificationSetting = true;
            soundNotificationSetting = false;
            if (!MaintenanceMode)
            {
	            screenNotification = true;
	            soundNotification = false;
	            $("#ActiveImage").removeClass('hidden');
	            $("#CloakImage").addClass('hidden');
	            populateHisConfigMessageObject(pre, "Visual");
	            sendCommand("TEST;" + JSON.stringify(hisConfig));
	        }
        }
        else if ((json_obj.value.trim().toLowerCase() == "all") || (json_obj.value == 3))
        {
        	console.log("Notification mode: All case");
            screenNotificationSetting = true;
            soundNotificationSetting = true;
            if (!MaintenanceMode)
            {
	            screenNotification = true;
	            soundNotification = true;
	            $("#ActiveImage").removeClass('hidden');
	            $("#CloakImage").addClass('hidden');
	            populateHisConfigMessageObject(pre, "All");
	            sendCommand("TEST;" + JSON.stringify(hisConfig));
	        }
        } else {
        	console.log("Did not find notification mode");
        }
    } else if (json_obj.key == "OperationMode") {
        var pre = getNotificationMode();
    	if (json_obj.value == "MaintMode") {
    		console.log("In MaintMode");
            screenNotification = false;
            soundNotification = false;
            MaintenanceMode = true;
            $("#ActiveImage").removeClass('hidden');
            setImage('../images/Common/MaintenanceMode.png', "");
    	} else {
    		console.log("Not In MaintMode");
            screenNotification = screenNotificationSetting;
            soundNotification = soundNotificationSetting;
            MaintenanceMode = false;
            $("#ActiveImage").removeClass('hidden');
            setImage(commonBackgroundImage, "");
    	}
    	$("#CloakImage").addClass('hidden');
    	var post = getNotificationMode();
    	populateHisConfigMessageObject(pre, post);
    	sendCommand("TEST;" + JSON.stringify(hisConfig));
    }
    // KeepAliveFrequency
    else if (json_obj.key == "KeepAliveFrequency")
    {
        var newValue = (json_obj.value * 60000) + 60000;
        console.log("Socket Timeout (ms):" + newValue);
        socketTimeout_ms = newValue;

        // Need to reset all timeouts to new value
    }
    else if (json_obj.key == "TspwPedestrianTimout")
    {
        console.log("TspwPedestrianTimout (ms):" + json_obj.value);
        tspwPedestrianTimout_ms = json_obj.value;    	
    }
    else { console.log("Did not process " + json_obj.key); }
}

/**
*   Deactivate the current page's button and activate the new page's button.
*   @param {string} - Id of the page being switched to
**/
function switchPage(page)
{
		window.location.href = page;
		$(".headbtn").removeClass('ui-btn-active');
      	console.log("Setting button:" + page);
        switch (page) {
            case "#Page1":
   				$("#page1Btn").addClass('ui-btn-active');
            break;
            case "#Page2":
            	$("#page2Btn").addClass('ui-btn-active');
            break;
            case "#Page3":
            	$("#page3Btn").addClass('ui-btn-active');
            break;
            case "#Page4":
            	$("#page4Btn").addClass('ui-btn-active');
            break;
            case "#Page5":
            	$("#page5Btn").addClass('ui-btn-active');
            return;
            default:
            break;
        }
}
/*------------------ Moved from the tmx.vehicle.display.library.js------------*/
function createHisMessageObject()
{
    var hisMessage = new Object();
    hisMessage.header = new Object();
    hisMessage.header.type = "Interface";
    hisMessage.header.subtype = "StateChange";
    hisMessage.header.source = "TMX UI";
    hisMessage.header.sourceId = "0";
    hisMessage.header.encoding = "json";
    hisMessage.header.timestamp = 0;
    hisMessage.header.flags = 0;

    hisMessage.payload = new Object();
    hisMessage.payload.Id = "0";
    hisMessage.payload.StateChangeTimestamp = "0";
/*
    enum HISType
    {
        HISUnknown = 0,
        Display=1,
        Audible=2
    };
*/

    hisMessage.payload.Severity = "0";
    hisMessage.payload.HISType = "0";
    hisMessage.payload.TriggeringAlertType = "0";
    hisMessage.payload.TriggeringAlertId = "";
    hisMessage.payload.HISPreState = "";
    hisMessage.payload.HISPostState = "";

    return hisMessage;
}

function createApplicationMessageObject()
{
    var applicationMessage = new Object();
    applicationMessage.header = new Object();
    applicationMessage.header.type = APPLICATIONMESSAGE_TYPE;
    applicationMessage.header.subtype = "Basic";
    applicationMessage.header.source = "TMX UI";
    applicationMessage.header.sourceId = "0";
    applicationMessage.header.encoding = "json";
    applicationMessage.header.timestamp =  new Date().getTime();
    applicationMessage.header.flags = 0;

    applicationMessage.payload = new Object();
    applicationMessage.payload.Id = "";
    applicationMessage.payload.AppId = "";
    applicationMessage.payload.EventID = "";
    applicationMessage.payload.Timestamp = "";
    applicationMessage.payload.DisplayDuration = "";
    applicationMessage.payload.Severity = "";
    applicationMessage.payload.EventCode = "";
    applicationMessage.payload.InteractionId = "";
    applicationMessage.payload.CustomText = "";

    return applicationMessage;
}

//Changing to the triggeringAlertId from event as the HIS message triggeringAlertId
//should contian the GUID of the calling application message or null
function populateHisImageMessageObject(appID, triggeringAlertId, severity)
{
    his.header.timestamp = new Date().getTime();
    // 
    his.payload.Id = generateUUID();
    his.payload.HISType = 1;
    his.payload.StateChangeTimestamp = new Date().getTime();
    his.payload.TriggeringAlertType = appID;
    his.payload.TriggeringAlertId = triggeringAlertId;
    his.payload.Severity = severity;

    hisAudio.header.timestamp = new Date().getTime();
    // 
    hisAudio.payload.Id = generateUUID();
    hisAudio.payload.HISType = 2;
    hisAudio.payload.StateChangeTimestamp = new Date().getTime();
    hisAudio.payload.TriggeringAlertType = appID;
    hisAudio.payload.TriggeringAlertId = triggeringAlertId;
    hisAudio.payload.Severity = severity;

}

function populateHisSoundMessageObject()
{
    // 
    his.payload.HISType = 2;

}

function populateHisConfigMessageObject(preState, postState) {
    hisConfig.header.timestamp = new Date().getTime();
    hisConfig.payload.HISType = 3;
    hisConfig.payload.TriggeringAlertType = "";
    hisConfig.payload.TriggeringAlertId = "";
    if (preState == "None" || preState == "All" || preState == "Audible" || preState == "Visual") {
        hisConfig.payload.HISPreState = preState;
    } else {
        hisConfig.payload.HISPreState = "Unknown";
    }
    if (postState == "None" || postState == "All" || postState == "Audible" || postState == "Visual") {
        hisConfig.payload.HISPostState = postState;
    } else {
        hisConfig.payload.HISPostState = "Unknown";
    }
}

function setImage(image, displayText)
{

    if ($("#ActiveImage").attr('src') != image)
    {
        his.payload.HISPreState = $("#ActiveImage").attr('src');
        his.payload.HISPostState = image;
        sendCommand("TEST;" + JSON.stringify(his));
       $("#ActiveImage").attr('src', image);
    }
    $("#bottomDiv").html(displayText);

}

function setTimedImage(image, displayText)
{

    if ($("#ActiveImage").attr('src') != image)
    {
        his.payload.HISPreState = $("#ActiveImage").attr('src');
        his.payload.HISPostState = image;
        sendCommand("TEST;" + JSON.stringify(his));     
       $("#ActiveImage").attr('src', image);
    }
//    $("#bottomDiv").html(displayText);

    stopTimer();
    startTimer(defaultTimout);
}

function stopTimer()
{
    if (timerID != null)
    {
        clearTimeout(timerID);
        timerID = null;
    }
}

function startTimer(duration)
{
    timerID = setTimeout(clearImage, duration);
}


function clearImage()
{
    stopTimer();
    setImage(commonBackgroundImage, "");
    VTRWSeverity = 0;
    EEBLSeverity = 0;
    FCWSeverity = 0;
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
            sendCommand("TEST;" + JSON.stringify(hisAudio));
        }
    }
}

function stopAudio()
{
    player.pause();
    player.currentTime = 0;
}


$(document).ready(function(){
   
    if (document.getElementById('server') != null) {
        document.getElementById('server').innerHTML = connections[0].ip;   
    }

    if (document.getElementById('port') != null) {
        document.getElementById('port').innerHTML = connections[0].port;
    }

//	setTimeout(function() {
	    connectInitial();
//	}, 5000);
});