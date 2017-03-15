var deviceName = "MDInVehicle";
var customLibraryJsVersion = "0.0.1";

// Event listener from common.library 
addEventListener("newMessage", newMessageHandler, false);
addEventListener("disconnect", disconnectHandler, false);
addEventListener("connect", connectHandler, false);

// Handle connect
function connectHandler()
{
	
}


// Receiver for unhandled messages from the common.library
function newMessageHandler(evt)
{
    //console.log(evt.detail.message);
    var json_obj = JSON.parse(evt.detail.message);
    
    handelMessages(json_obj);
    /*
    if (json_obj.header.type === "Application") { handleAppMessage(json_obj.payload); }
    else if (json_obj.header.subtype === "Basic") { handleAppMessage(json_obj.payload); }
    else if (json_obj.header.type === "Pmm" && json_obj.header.subtype === "NextRequest") { handlePmmMessage(json_obj.payload); }
    else if (json_obj.header.type === "Decoded" && json_obj.header.subtype === "Location") { handleLocation(json_obj.payload); }
    */
//    else { console.log(timeStamp(evt.detail.time) + " Unknown Message: " + evt.detail.message);   }
}



//--------------------------HNADLERS----------------------------

function handelMessages(json_obj)
{
	if (isSubscribedTo(json_obj.header)) {
		console.log(json_obj.payload);	
		switch(json_obj.header.type) {
			case "Decoded":
			{
				switch(json_obj.header.subtype) {
				case "Location":
					setLocationForm(json_obj.payload);
					break;
				case "Vehicle":
					setVehicleForm(json_obj.payload);
					break;					
				}
				break;
			}
			case "Vehicle":
			{
				switch(json_obj.header.subtype) {
				case "Basic":
					setVehicleForm(json_obj.payload);
					break;					
				}
				break;
			}
			case APPLICATIONMESSAGE_TYPE:
			{
				switch(json_obj.header.subtype) {
				case APPLICATIONMESSAGE_SUBTYPE:
					console.log("APPLICATION MESSAGE");
					break;					
				}
				break;
			}				
		}
	}
}
/*-------------------------------------------------------------------------------*/

//Function to parse the JSON values and set them in the GPS modal
function setLocationForm(payload)
{

	$("#location_latitude").text(round(payload.Latitude, 6));
	$("#location_longitude").text(round(payload.Longitude, 6));
	$("#location_speed").text(round(payload.Speed, 4));
	$("#location_heading").text(round(payload.Heading, 2));
	$("#location_altitude").text(payload.Altitude);
	$("#location_fixquality").text(getFixType(payload.FixQuality));
	$("#location_horizontaldop").text(payload.HorizontalDOP);
	$("#location_numsatellites").text(payload.NumSatellites);
	$("#location_signalquality").text(getSignalQuality(payload.SignalQuality));
	$("#location_time").text(moment.unix(payload.Time / 1000).format("MM/DD/YYYY hh:mm a"));
}

//Function to parse the JSON values and set them in the CAN modal
function setVehicleForm(payload)
{
	//console.log("in setLocationForm");
	$("#vehicle_brakeapplied").text(payload.BrakeApplied == 0 ? "Not Applied" : "Applied");
	$("#vehicle_frontdoorsopen").text(payload.FrontDoorsOpen == 0 ? "Closed" : "Open");
	$("#vehicle_gearposition").text(getGearPosition(payload.GearPosition));
	$("#vehicle_reardoorsopen").text(payload.RearDoorsOpen == 0 ? "Closed" : "Open");
	$("#vehicle_speedmph").text(round(payload.SpeedMph, 4));
	$("#vehicle_turnsignalposition").text(getTurnSignalPosition(payload.TurnSignalPosition));		
}

//Translates to a human readable enum value
function getSignalQuality(signalQuality)
{
	var returnStr;
	
	//console.log(signalQuality);
	//Should reference the enum values but the name of the enum variable is a reserved type
	switch(signalQuality)
	{
		case "0":
			returnStr = "Invalid";//locationmessage.SignalQualityTypes.Invalid.string;
			break;
		case "1":
			returnStr = "GPS"; //locationmessage.SignalQualityTypes.GPS.string;
			break;
		case "2":
			returnStr = "DGPS"; //locationmessage.SignalQualityTypes.DGPS.string;
			break;
		case "3":
			returnStr = "PPS"; //locationmessage.SignalQualityTypes.PPS.string;
			break; 
		case "4":
			returnStr = "RealTimeKinematic"; //locationmessage.SignalQualityTypes.RealTimeKinematic.string;
			break;
		case "5":
			returnStr = "FloatRTK"; //locationmessage.SignalQualityTypes.FloatRTK.string;
			break; 
		case "6":
			returnStr = "DeadReckoning"; //locationmessage.SignalQualityTypes.DeadReckoning.string;
			break; 
		case "7":
			returnStr = "ManualInputMode"; //locationmessage.SignalQualityTypes.ManualInputMode.string;
			break; 
		case "8":
			returnStr = "SimulationMode"; //locationmessage.SignalQualityTypes.SimulationMode.string;
			break; 
	}
	
	return returnStr;	
}

//Translates to a human readable enum value
function getFixType(fixType)
{
	var returnStr;
	
	//console.log(fixType);
	//Should reference the enum values but the name of the enum variable is a reserved type	
	switch(fixType)
	{
		case "0":
			returnStr = "Unknown"; //location.FixTypes.Unknown.string;
			break;
		case "1":
			returnStr = "NoFix"; //location.FixTypes.NoFix.string;
			break;
		case "2":
			returnStr = "TwoD"; //location.FixTypes.TwoD.string;
			break;
		case "3":
			returnStr = "ThreeD"; //location.FixTypes.ThreeD.string;
			break; 
	}
	
	return returnStr;	
}

//Translates to the human readable enum values
function getGearPosition(gearPosition)
{
	var returnStr;
	
	//console.log(gearPosition);
	switch(gearPosition)
	{
		case vehicleparam.GearState.GearUnknown.value.toString():
			returnStr = vehicleparam.GearState.GearUnknown.string;
			break;
		case vehicleparam.GearState.Park.value.toString():
			returnStr = vehicleparam.GearState.Park.string;
			break;
		case vehicleparam.GearState.Reverse.value.toString():
			returnStr = vehicleparam.GearState.Reverse.string;
			break;
		case vehicleparam.GearState.Drive.value.toString():
			returnStr = vehicleparam.GearState.Drive.string;
			break; 
		case vehicleparam.GearState.Other.value.toString():
			returnStr = vehicleparam.GearState.Other.string;
			break; 
	}
	
	return returnStr;
}

//Translates to the human readable enum values
function getTurnSignalPosition(turnSignalPosition)
{
	var returnStr;
	
	//console.log(turnSignalPosition);
	switch(turnSignalPosition)
	{
		case vehicleparam.TurnSignalState.SignalUnknown.value.toString():
			returnStr = vehicleparam.TurnSignalState.SignalUnknown.string;
			break;
		case vehicleparam.TurnSignalState.Off.value.toString():
			returnStr = vehicleparam.TurnSignalState.Off.string;
			break;
		case vehicleparam.TurnSignalState.Left.value.toString():
			returnStr = vehicleparam.TurnSignalState.Left.string;
			break;
		case vehicleparam.TurnSignalState.Right.value.toString():
			returnStr = vehicleparam.TurnSignalState.Right.string;
			break; 
	}
	
	return returnStr;
}


function isSubscribedTo(header) {
	
	for (i = 0; i < subscriptions.length;i++){
		if (subscriptions[i].type == header.type && subscriptions[i].subtype == header.subtype) {
			return true;
		}
	}
	
	return false;
}

//--------------------------DOCUMENT----------------------------
$(document).ready(function () {
	
    $(".headbtn").mousedown(function () {
        $(".headbtn").removeClass('ui-btn-active');
        $(this).addClass('ui-btn-active');
    });
    
    //Add click event to the HIS start button
    $("#his-strobe-sounder-start").on("click", function(){
    	$(this).prop("disabled",true);
    	sendHisStartMessage();
    });
    
    //Add click event to the HIS stop button
    $("#his-strobe-sounder-stop").on("click", function(){
    	$("#his-strobe-sounder-start").prop("disabled",false);
    	sendHisStopMessage();
    });	

	//Add on change event to the DSRC1 bootswitch
    $("#DSRC1").on('switchChange.bootstrapSwitch', function(event, state) {
    	//console.log(this); // DOM element
    	//console.log(event); // jQuery event
    	//console.log(state); // true | false
    	toggleDsrc('DSRC1', state)
    });
    
    //Add on change event to the DSRC2 bootswitch
    $("#DSRC2").on('switchChange.bootstrapSwitch', function(event, state) {
    	//console.log(this); // DOM element
    	//console.log(event); // jQuery event
    	//console.log(state); // true | false
    	toggleDsrc('DSRC2', state) 	
    });
    
	$('#DSRCState').on('hidden.bs.select', function (e) {
		console.log("Selected");
		
		if ($('#DSRCState').val() == 1) {
		  toggleDsrc('DSRC1', 'true');
		  toggleDsrc('DSRC2', 'false');
		} 
		else if ($('#DSRCState').val() == 2) {
		  toggleDsrc('DSRC1', 'false');
		  toggleDsrc('DSRC2', 'true');
		} 
		else if ($('#DSRCState').val() == 3) {
		  toggleDsrc('DSRC1', 'true');
		  toggleDsrc('DSRC2', 'true');	
		}
		else {
		  toggleDsrc('DSRC1', 'false');
		  toggleDsrc('DSRC2', 'false');			
		}
	});    
});

//Function to send the application message to start the strobe/sounder
function sendHisStartMessage() {
	console.log("Sending His Start Message");
	
	var applicationMessage = createApplicationMessageObject();
	applicationMessage.payload.Id = generateUUID();
	applicationMessage.payload.Timestamp = new Date().getTime();
	applicationMessage.payload.AppId = appmessage.ApplicationTypes.TSPW.value;
	//Setting to Audio to trigger the set alert case in the PedestrianAlertFromBusPlugin
	applicationMessage.payload.Severity = appmessage.Severity.Audio.value;
	applicationMessage.payload.EventCode = "";
	//console.log("TEST;" + JSON.stringify(applicationMessage));
	sendCommand("TEST;" + JSON.stringify(applicationMessage));
}

//Function to send the application message to start the strobe/sounder
function sendHisStopMessage() {
	console.log("Sending His Stop Message");
	
	var applicationMessage = createApplicationMessageObject();
	applicationMessage.payload.Id = generateUUID();
	applicationMessage.payload.Timestamp = new Date().getTime();
	applicationMessage.payload.AppId = appmessage.ApplicationTypes.TSPW.value;
	//Setting to -1 to trigger default case in the PedestrianAlertFromBusPlugin
	applicationMessage.payload.Severity = "-1";
	//Setting the EventComplete to go into the default case if statement and also use ApplicationInactive
	applicationMessage.payload.EventCode = appmessage.EventCodeTypes.EventComplete.value; //Need proper event code
	//console.log("TEST;" + JSON.stringify(applicationMessage));
	sendCommand("TEST;" + JSON.stringify(applicationMessage));
}

//Call the PHP action to make the changes supplied via the POST parameters 
function toggleDsrc(dsrc, state) {
	console.log("Toggle DSRC: DSRC, " + dsrc + " - State, " + state);
	
	$.post('api/PluginParamUpdateActions.php?action=CohdaUpdate', {dsrcname:dsrc, dscrstate: state},
	function (data) {
		$("#result").html(data);
	});
}

//Handle disconnect
function disconnectHandler()
{
    console.log("Disconnected - Removing all images");
}