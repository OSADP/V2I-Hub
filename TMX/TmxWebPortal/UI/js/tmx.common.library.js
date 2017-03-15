console.log("including tmx.common.library.js");
var commonLibraryJsVersion = "0.0.1";
var commonBackgroundImage = "../images/Common/Background.png";
var VTRWSeverity = 0;
var CSWSeverity = 0;
var EEBLSeverity = 0;
var FCWSeverity = 0;

//Image timeout variables
var timerID = null;
var defaultTimout = 3000;

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
				console.log("------- Closing Socket " + socketObject.url + " ID:" + socketObject.timeoutID + "----------------");
				socketObject.socket.close();
				updateConnected(socketObject.url, false);
				socketObject.connected = false;
				socketObject.connecting = false;
			}, 
			200000
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
/*
	try {
		json_obj = JSON.parse(msg);

	}
	catch (e)
	{
		console.log("Exception:" + msg);
		return;
	}
	*/

	if (0) {}
	/*
	if (json_obj.header.subtype === "SPAT-P") { processSpat(json_obj); }
	else if (json_obj.header.subtype === "MAP-P") { processMap(json_obj); }
	else if (json_obj.header.subtype === "RemoteVehicle") { handleTC(json_obj); }
	else if (json_obj.header.subtype === "BSM") { handleBSM(json_obj); }
	*/
	else
	{
		// If message not handled, create an event and dispatch
		var event = new CustomEvent("newMessage", {
			detail: {
				message: msg,
				time: new Date(),
			},
		});
		this.dispatchEvent(event);
	}

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
				var msgBuffer;
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
					messageHandler(decode);

					/*

					msgBuffer += decode;
					endsWithNewline = msgBuffer.endsWith("\n");
					if (msgBuffer.indexOf("\n") >= 0) 
					{
						var newMessages = msgBuffer.split("\n");
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
					*/
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
    hisMessage.payload.TriggeringAlertId = "1";
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

function populateHisImageMessageObject(appID, event, severity)
{
    his.header.timestamp = new Date().getTime();
    // 
    his.payload.Id = generateUUID();
    his.payload.HISType = 1;
    his.payload.StateChangeTimestamp = new Date().getTime();
    his.payload.TriggeringAlertType = appID;
    his.payload.TriggeringAlertId = event;
    his.payload.Severity = severity;

    hisAudio.header.timestamp = new Date().getTime();
    // 
    hisAudio.payload.Id = generateUUID();
    hisAudio.payload.HISType = 2;
    hisAudio.payload.StateChangeTimestamp = new Date().getTime();
    hisAudio.payload.TriggeringAlertType = appID;
    hisAudio.payload.TriggeringAlertId = event;
    hisAudio.payload.Severity = severity;

}

function populateHisSoundMessageObject()
{
    // 
    his.payload.HISType = 2;

}

function setTimedImage(image, displayText)
{

//    console.log( $("#ActiveImage").attr('src'));

    if ($("#ActiveImage").attr('src') != image)
    {
        his.payload.HISPreState = $("#ActiveImage").attr('src');
        his.payload.HISPostState = image;
        sendCommand("TEST;" + JSON.stringify(his));     
       $("#ActiveImage").attr('src', image);
    }
 //   $("#bottomDiv").html(displayText);

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
    $("#ActiveImage").attr('src', commonBackgroundImage);
//    $("#bottomDiv").html("");
    VTRWSeverity = 0;
    EEBLSeverity = 0;
    FCWSeverity = 0;
}

function round(value, decimals) {
    return Number(Math.round(value+'e'+decimals)+'e-'+decimals);
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