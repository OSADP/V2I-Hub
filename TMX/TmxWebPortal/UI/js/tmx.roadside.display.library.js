console.log("Including tmx.roadside.display.library.js")
var deviceName = "TMX";
var customLibraryJsVersion = "0.0.1";

// Event listener from common.library 
addEventListener("newMessage", newMessageHandler, false);
addEventListener("disconnect", disconnectHandler, false);
addEventListener("connect", connectHandler, false);

// Handle connect
function connectHandler()
{
    console.log("Connected Setting Background Image");
     $("#ActiveImage").attr('src', '../images/Common/battelle_blue.png');
}

// Receiver for unhandled messages from the common.library
function newMessageHandler(evt)
{
    var json_obj = JSON.parse(evt.detail.message);
    if (json_obj.header.type === "Application") { handleAppMessage(json_obj.payload); }
    else if (json_obj.header.subtype === "Basic") { handleAppMessage(json_obj.payload); }
    else if (json_obj.header.type === "__config") { 
        // Need to handle 
        if (json_obj.payload != undefined)
        {
            for (var i = 0; i < json_obj.payload.length; i++)
            {
                handleConfig(json_obj.payload[i]); 
            }
        }
    }
    else { console.log(timeStamp(evt.detail.time) + " Unknown Message: " + evt.detail.message);	}
}

// Handle disconnect
function disconnectHandler()
{
    console.log("Disconnected - Removing all images");
    TspwClearAllZones();
    stopSoundTimer();
    $("#ActiveImage").removeClass("hidden");
    $("#CloakImage").addClass("hidden");
     $("#ActiveImage").attr('src', '../images/Common/Splash_Screen_2.png');
}

/*
This function handles all of the Application:Application messages and displays the appropriate 
Image.
*/
function handleAppMessage(json_obj)
{
	if (json_obj.AppId == 12) // TSPWRD
    {
    	TspwHandleAlert(json_obj);
    }
	// else if - add other Applications here
}

// Overwrite some common functions
function setImage(zone, image)
{

    if (zone.attr('src') != image)
    {        
        his.payload.HISPreState = zone.attr('src');
        his.payload.HISPostState = image;
        sendCommand("TEST;" + JSON.stringify(his));       
        zone.attr('src', image);
    }
}

function SoundTimeout()
{
    console.log("Sound Timeout Clearing");
    currentAudio = -1;
    TspwPlayNextAudio();
}


// ------- TSPW Start ----------------------------------------------------
var tspwImgToggleInterval = 2000;
var tspwValidSeverity = {"0": true, "1": true, "2": true};

var tspwBusStates = {};

var tspwSevToBlinkImg = { "0": "Info", "1": "Inform", "2": "Warn"};

defaultSoundTimeout = tspwImgToggleInterval;

var tspwZoneInfoTable = {
    currentSev: 0, // default to inform message until ped Alerts are received.
    nextAudioIdx: -1, // next audio to play.
	"29": {
		zoneName: "BusApproaching",
		timerId: null, // for Image blink toggle
		files: {
        "0":{
                imgFile: ["../images/RP/01-approaching1.png", "../images/RP/02-approaching2.png"], // small and large info images
                audFileIdx: [0, -1],
                zIndex: [5,6,7] // base, flash and bus route on top of all other images
            },
		"1":{
				imgFile: ["../images/RP/05-inform-curb2.png", "../images/RP/04-inform-curb1.png"],
				audFileIdx: [0, 2],
                zIndex: [5,7,6] // base, bus route and flash on top of all other images
			},
		"2":{
				imgFile: ["../images/RP/07-warning-road2.png", "../images/RP/06-warning-road1.png"],
				audFileIdx: [0, 4],
                zIndex: [5,7,6] // base, bus route and flash on top of all other images
			}}},
	"30": {
		zoneName: "BusAtStop",
		timerId: null,
		files: {
        "0":{
                imgFile: ["../images/RP/03-at-stop.png", "../images/RP/03-at-stop.png"],
                audFileIdx: [1, -1],
                zIndex: [8,9,10] // base, flash and bus route on top of all other images
            },
		"1":{
				imgFile: ["../images/RP/09-inform-road2.png", "../images/RP/08-inform-road1.png"],
				audFileIdx: [1, 3],
                zIndex: [8,10,9] // base, bus route and flash  on top of all other images
			},
		"2":{
				imgFile: ["../images/RP/09-inform-road2.png", "../images/RP/08-inform-road1.png"], // no warning image
				audFileIdx: [1, 3], // no warning audio
                zIndex: [8,10,9] // base, bus route and flash  on top of all other images
			}}},
	"31": {
		zoneName: "BusDeparting",
		timerId: null,
		files: {
        "0":{
                imgFile: ["",""], // no info image present
                audFileIdx: [-1, -1],
                zIndex: [0, 0, 0] // no display
            },
		"1":{
				imgFile: ["../images/RP/04-inform-curb1.png","../images/RP/04-inform-curb1.png"],
				audFileIdx: [-1, 2],
                zIndex: [3,4,0] // no bus route 
			},
		"2":{
				imgFile: ["../images/RP/06-warning-road1.png","../images/RP/06-warning-road1.png"],
				audFileIdx: [-1, 4],
                zIndex: [3,4,0] // no bus route 
			}}}
};

function TspwHandleAlert(json_obj)
{
    var eventValid = true;
    switch (json_obj.EventCode) {

    case "4": // Exited Area
    case "20": // EventComplete
        if (json_obj.InteractionId) {
            // The bus has left the area
            TspwHandleBusExited(json_obj.InteractionId);
        }
        else {
            console.log("TSPW No Pedestrians! Clear Ped Alerts.");
            TspwHandlePedStateChange(json_obj);
        }
    	break;

    case "3":  // Entered Area == Bus Approaching
    case "29": // Bus Landing = Arriving,
        json_obj.EventCode = "29";
        // FALL THROUGH!!!!!!
    case "30": // Bus Arrived = At Stop,
    case "31": // Bus Leaving,
        var busObj = {EventCode: json_obj.EventCode, CustomText: json_obj.CustomText};
        tspwBusStates[json_obj.InteractionId] = busObj;
        TspwHandleBusStateChange();
        //Need to set the HIS object before sending the HIS message to the core
        if (screenNotification) {
            populateHisImageMessageObject(12, json_obj.Id, json_obj.Severity);
            TspwDisplayBusInfo(json_obj.InteractionId, json_obj.EventCode);
        }
        break;

    case "10": // OnCurbsidePed
    case "11": // InRoadwayPed
        console.log("Pedestrians Detected! Display Alerts for event " + json_obj.EventCode);
        TspwHandlePedStateChange(json_obj);
        break;
    default:
        eventValid = false;
	}

    if (eventValid) TspwPlayAudio(1); // pedestrian first.
}

function TspwPlayAudio(seq)
{
    var busObj = TspwGetHighestBus();
    var evCode = null;
    if (busObj) {
        evCode  = busObj.EventCode;

        // First play Pedestrian audible if applicable based on current severity,
        // then play bus info audible
        var zoneObj    = tspwZoneInfoTable[evCode];
        var sev = tspwZoneInfoTable.currentSev;
        var audFileIdx = zoneObj.files[sev].audFileIdx[seq];
        tspwZoneInfoTable.nextAudioIdx = (seq == 1) ? zoneObj.files[sev].audFileIdx[0] : -1;

        if (audFileIdx !== -1 && soundNotification) {
            playAudio(audFileIdx, audioFiles);
        }
        else {
            // Play the next audible right away if applicable.
            TspwPlayNextAudio();
        }       
    }
}

function TspwPlayNextAudio()
{
    if (tspwZoneInfoTable.nextAudioIdx !== -1 && soundNotification){
        playAudio(tspwZoneInfoTable.nextAudioIdx, audioFiles);
        tspwZoneInfoTable.nextAudioIdx = -1;
    }
}

function TspwHandlePedStateChange(json_obj)
{
    // Tspw data structure is arranged such that pedestrian EventCode is irrelvant for the graphics.
    // Graphics can be uniquely identified based on the severity of the Pedestrian Alert
    // and the bus state atm.
    tspwZoneInfoTable.currentSev = json_obj.Severity;

    // Update bus images based on new severity.
    for (var prop in tspwBusStates) {
        if (tspwBusStates.hasOwnProperty(prop) && screenNotificiation) {
            TspwDisplayBusInfo(prop, tspwBusStates[prop].EventCode);
        }
    }
}

function TspwHandleBusStateChange()
{
    var busObj = TspwGetHighestBus();
    var evCode = null;
    if (busObj)
        evCode = busObj.EventCode;

    if (evCode) {
        if (evCode !== "30")
            TspwClearZone("30");
        if (!TspwIsBusInState("29"))
            TspwClearZone("29");
        if (!TspwIsBusInState("31"))
            TspwClearZone("31");

        // Set Custom Text of the highest priority bus
        //TspwSetTextFields(busObj.CustomText);
    }
    else {
        TspwClearAllZones();
    }
    return evCode;
}

function TspwSetTextFields(text)
{
    $("#bottomDiv").html(text);
}

function TspwSetBusRoute(zoneName, text)
{
    var busRoute = TspwGetBusRoute(text);
    zoneName.html(busRoute);
}

function TspwGetBusRoute(text)
{
    var busRoute = "&nbsp"; // default to nothing in case there is no valid bus route.
    if (text) {
        var toks = text.split(";");
        for (var i = 0; i < toks.length; i++){
            if (toks[i].indexOf('Route') > -1) {
                var route = toks[i].split(" ");
                if (route[1]) busRoute = route[1];
                break;
            }
        }
    }
    return busRoute;
}

function TspwHandleBusExited(InteractionId)
{
    console.log("TSPW Bus Exited: " + InteractionId);
    delete tspwBusStates[InteractionId];
    TspwHandleBusStateChange();
}

function TspwIsBusInState(state)
{
    // return ture if there exists a bus on a map in a given state
    var found = false;
    for (var prop in tspwBusStates) {
        if (tspwBusStates.hasOwnProperty(prop)) {
            if (tspwBusStates[prop].EventCode == state) {
                found = true;
                break;
            }
        }
    }
    return found;
}

function TspwGetHighestBus()
{
    // If more than one bus is detected on the roadside, then the following priority
    // for the roadside display applies regardless of bus's speeds (high to low):
    // 1. Bus at stop - evCode 30
    // 2. Bus approaching - evCode 29
    // 3. Bus is leaving - evCode 31
    // Return the highest available bus state.
    var busObj = null;
    for (var prop in tspwBusStates) {
        if (tspwBusStates.hasOwnProperty(prop)) {

            switch (tspwBusStates[prop].EventCode) {
            case "30": // Bus Arrived = At Stop
                busObj = tspwBusStates[prop];
                break;
            case "29": // Bus Approaching
                if (!busObj || busObj.EventCode !== "30")
                    busObj = tspwBusStates[prop];
                break;
            case "31": // Bus Leaving
                if (!busObj)
                    busObj = tspwBusStates[prop];
                break;
            default:
                break;
            }
        }
    }
    return busObj;
}

function TspwClearZone(evCode)
{
    var zoneObj = tspwZoneInfoTable[evCode];
    var zone = $("#" + zoneObj.zoneName + "Image");
    var rZone = $("#" + zoneObj.zoneName + "Route");

    zone.addClass("hidden");
    setImage(zone, "");

    TspwClearBlinkImg(zoneObj);

    TspwSetBusRoute(rZone, "");
    rZone.addClass("hidden");

}

function TspwClearAllZones()
{
    TspwClearZone("30");
    TspwClearZone("29");
    TspwClearZone("31");
    TspwSetTextFields("");
}

function TspwDisplayBusInfo(busId, evCode)
{
    console.log("TSPW Bus Info: " + busId +", for event code: " + evCode);
    var zoneObj = tspwZoneInfoTable[evCode];
    var zone = $("#" + zoneObj.zoneName + "Image");
    var sev = tspwZoneInfoTable.currentSev;
    var imgFile = zoneObj.files[sev].imgFile[0];
    var blinkImg = zoneObj.files[sev].imgFile[1];
    var blinkZone = $("#" + zoneObj.zoneName + "FlashImage");
    var routeZone = $("#" + zoneObj.zoneName + "Route");

    TspwClearBlinkImg(zoneObj);
    console.log(imgFile);
    setImage(zone, imgFile);

    var zIndex =  "z-index-" + zoneObj.files[sev].zIndex[0];
    zone.alterClass("z-index*", zIndex);
    if (imgFile) zone.removeClass("hidden");
    else zone.addClass("hidden");

    TspwSetBusRoute(routeZone, tspwBusStates[busId].CustomText);

    zIndex = "z-index-" + zoneObj.files[sev].zIndex[2];
    routeZone.alterClass("z-index*", zIndex);
    routeZone.removeClass("hidden");

    zIndex = "z-index-" + zoneObj.files[sev].zIndex[1];
    blinkZone.alterClass("z-index*", zIndex);
    if (blinkImg) {
        blinkZone.attr('src', blinkImg);
        TspwToggleBlinkImg(zoneObj);
    }
}

function TspwToggleBlinkImg (zoneObj)
{
    var blinkZone = $("#" + zoneObj.zoneName + "FlashImage");
    blinkZone.toggleClass("hidden");
    zoneObj.timerId = setTimeout(TspwToggleBlinkImg, tspwImgToggleInterval, zoneObj);
}

function TspwClearBlinkImg(zoneObj)
{
    var blinkZone = $("#" + zoneObj.zoneName + "FlashImage");
    blinkZone.addClass("hidden");
    clearTimeout(zoneObj.timerId);
    zoneObj.timerId = null;
}

//------- TSPW End -------------------------------------------------------


//------- Roadside Sounds -----------------------

var audioFiles = [
// ----------------------------------------------------- 
	"../sounds/TSPW_RS/Bus\ Approaching.mp3",              // 0
	"../sounds/TSPW_RS/Bus\ at\ Stop.mp3",                 // 1
	"../sounds/TSPW_RS/TSPW_Clear\ the\ Curb.mp3",         // 2
	"../sounds/TSPW_RS/TSPW_Clear\ the\ Road_Inform.mp3",  // 3
	"../sounds/TSPW_RS/TSPW_Clear\ the\ Road_Warn.mp3"     // 4
];


for (var i in audioFiles) {
    preloadAudio(audioFiles[i]);
}

$(function(){
    $("[data-role='header']").toolbar();
});

$(document).ready(function () {
    $(".headbtn").mousedown(function () {
        $(".headbtn").removeClass('ui-btn-active');
        $(this).addClass('ui-btn-active');
    });
});
