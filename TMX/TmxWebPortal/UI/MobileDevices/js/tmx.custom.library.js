var deviceName = "MDInVehicle";
var customLibraryJsVersion = "0.0.1";
var online = false;

var MDCurrentEvent = 0;

var his = createHisMessageObject();
var hisAudio = createHisMessageObject();
var hisID = 0;
var hisAudioID = 0;

var mapTimerID = null;
var defaultMapTimout = 6000;
var pedMonTimerID = null;
var defaultPedMonTimout = 6000;
var pointDivisor = 10000000; 
var map = null;
var mapZoom = 16;

var MapActive = false;
var PedMonitorActive = false;

var ActivePedMonitorImage;
var PedMonCautionImage = "../images/MD/MD_caution_base.png";
var PedMonWarningImage = "../images/MD/MD_warning_base.png";
var PedMonAdvisoryImage = "../images/MD/MD_advisory_base.png";
var MapCar = "../images/MD/car.png";
var MapPickup = "../images/MD/pickup_pin.png";
var MapDestination = "../images/MD/destination_pin.png";
var SystemSplash = "../images/Common/Splash_Screen.png";
var SystemActiveImage = "../images/MD/MD_system_active.png";
var SystemInactiveImage = "../images/MD/MD_system_inactive.png";

var pickUpTime;
var numberOfPassengers;
var numberOfRegPassengers;
var numberOfHandiPassengers;

//initial dataset for markers, note the images will most likely change
var locationList = {
	Vehicle: {
        info: 'Vehicle Location',
        lat: 0,
        lon: 0,
        icon: MapCar
    },
    PickUp: {
        info: 'PickUp Location',
        lat: 0,
        lon: 0,
        icon: MapPickup
    },
    Destination: {
        info: 'Destination Location',
        lat: 0,
        lon: 0,
        icon: MapDestination
    }
};

var locations = {}; //A repository for markers (and the data from which they were contructed).
var infowindow;

// Event listener from common.library 
addEventListener("newMessage", newMessageHandler, false);
addEventListener("disconnect", disconnectHandler, false);
addEventListener("connect", connectHandler, false);

// Handle connect
function connectHandler()
{
    console.log("Connected Setting Background Image");
    console.log(subscriptions);
    MapActive = false;
    PedMonitorActive = false;    
    initializeScreen()
    setActiveImage('show',SystemSplash);
}


// Receiver for unhandled messages from the common.library
function newMessageHandler(evt)
{
    //console.log(evt.detail.message);
    var json_obj = JSON.parse(evt.detail.message);
    if (json_obj.header.type === "Application") { handleAppMessage(json_obj.payload); }
    else if (json_obj.header.subtype === "Basic") { handleAppMessage(json_obj.payload); }
    else if (json_obj.header.type === "Pmm" && json_obj.header.subtype === "NextRequest") { handlePmmMessage(json_obj.payload); }
    else if (json_obj.header.type === "Decoded" && json_obj.header.subtype === "Location") { handleLocation(json_obj.payload); }
//    else { console.log(timeStamp(evt.detail.time) + " Unknown Message: " + evt.detail.message);   }
}



//--------------------------HNADLERS----------------------------

/*
This function handles all of the Location messages and sets the appropriate 
points on the map.
*/
function handleLocation(json_obj)
{
	//console.log(json_obj);
	locationList["Vehicle"].lat = round(json_obj.Latitude, 6);
	locationList["Vehicle"].lon = round(json_obj.Longitude, 6);
	console.log("Vehicle Lat/Lon: " + locationList["Vehicle"].lat + "/" + locationList["Vehicle"].lon);
}

/*
This function handles all of the Application:Application messages and displays the appropriate 
Image.
*/
function handleAppMessage(json_obj)
{
	//Only prosessing the MD application type
    if (json_obj.AppId == 11) // MD
    {       
    	//console.log(json_obj);
    	
    	PedMonitorActive = true;
        initializeScreen();
        
        switch (json_obj.EventCode) {
        case "26":
        	if (MDCurrentEvent != Number(json_obj.EventCode))
    		{
        		populateHisImageMessageObject(11, 26, 0);
        		MDCurrentEvent = Number(json_obj.EventCode);
    		}
        	
        	ActivePedMonitorImage = PedMonCautionImage;
        	setTimedPedMon(PedMonCautionImage, json_obj.CustomText);
    		//console.log(JSON.stringify(his));
            break;
        case "27":
        	if (MDCurrentEvent != Number(json_obj.EventCode))
    		{
        		populateHisImageMessageObject(11, 27, 1);
        		MDCurrentEvent = Number(json_obj.EventCode);
    		}
        	ActivePedMonitorImage = PedMonWarningImage;
        	setTimedPedMon(PedMonWarningImage, json_obj.CustomText);
    		//console.log(JSON.stringify(his));        	
            break;
        case "28":
        	if (MDCurrentEvent != Number(json_obj.EventCode))
    		{
           		populateHisImageMessageObject(11, 28, 2);
           		MDCurrentEvent = Number(json_obj.EventCode);
    		}
        	
        	ActivePedMonitorImage = PedMonAdvisoryImage;
        	setTimedPedMon(PedMonAdvisoryImage, json_obj.CustomText);
       		//console.log(JSON.stringify(his));        	
            break;
        default:
            clearImage();
            break;
        }
		
        MDCurrentEvent = Number(json_obj.EventCode);
    }	
}

/*
This function handles all of the Pmm messages and displays the appropriate 
points on the map.
*/
function handlePmmMessage(json_obj)
{
	console.log(json_obj);
	
    MapActive = true;	   
    initializeScreen();
    
    if (json_obj.PickupLatitude == "" || json_obj.PickupLongitude == "" || json_obj.DestLatitude == "" || json_obj.DestLongitude == "" || 
    		json_obj.PickupLatitude == undefined || json_obj.PickupLongitude == undefined || json_obj.DestLatitude  == undefined || json_obj.DestLongitude == undefined) {
    	console.log("One or more of the Pmm Lat/Longs is empty");
    	return;
    }
    
    var newPickUpLat = json_obj.PickupLatitude.includes(".") ? round(json_obj.PickupLatitude, 6) : round(parseInt(json_obj.PickupLatitude) / pointDivisor, 6);
    var newPickUpLong = json_obj.PickupLongitude.includes(".") ? round(json_obj.PickupLongitude, 6) : round(parseInt(json_obj.PickupLongitude) / pointDivisor, 6);
    var newDestinationLat = json_obj.DestLatitude.includes(".") ? round(json_obj.DestLatitude, 6) : round(parseInt(json_obj.DestLatitude) / pointDivisor, 6);
    var newDestinationLong = json_obj.DestLongitude.includes(".") ? round(json_obj.DestLongitude, 6) : round(parseInt(json_obj.DestLongitude) / pointDivisor, 6);
    
    console.log("Old Pick UP LAT/LONG: " + locationList["PickUp"].lat + ", " + locationList["PickUp"].lon);
    console.log("New Pick UP LAT/LONG: " + newPickUpLat + ", " + newPickUpLong);
    console.log("Old Destination LAT/LONG: " + locationList["Destination"].lat + ", " + locationList["Destination"].lon);
    console.log("New Destination LAT/LONG: " + newDestinationLat + ", " + newDestinationLong);
    
    pickUpTime = json_obj.PickupDate !== undefined ? json_obj.PickupDate : "Unavailable";
    //var jsonData = JSON.stringify(json_obj.__proto__);
    var seatsByType = json_obj.SeatsByTypes;
    
    if (seatsByType != null && seatsByType != "")
    {
    	numberOfPassengers = Number(seatsByType[0].Count) + Number(seatsByType[1].Count);
    	numberOfRegPassengers = Number(seatsByType[0].Count);
    	numberOfHandiPassengers = Number(seatsByType[1].Count);
    }
    
    //Only send out HIS message if the pick up location or destination location has changed
    if (locationList["PickUp"].lat != newPickUpLat || locationList["PickUp"].lon != newPickUpLong ||
    		locationList["Destination"].lat != newDestinationLat || locationList["Destination"].lon != newDestinationLong) {
    	
    	console.log("Sending Map HIS chnage");
    	his.payload.HISPreState = "Previous PickUp Latitude, Logitude: " + locationList["PickUp"].lat + ", " + locationList["PickUp"].lon + 
    	" - Previous Destination Latitude, Longitude: " + locationList["Destination"].lat + ", " + locationList["Destination"].lon;
        his.payload.HISPostState = "New PickUp Latitude, Logitude: " + newPickUpLat + ", " + newPickUpLong + 
    	" - New Destination Latitude, Longitude: " + newDestinationLat + ", " + newDestinationLong;
        sendCommand("TEST;" + JSON.stringify(his));   
    } 
    
	locationList["PickUp"].lat = newPickUpLat;
	locationList["PickUp"].lon = newPickUpLong;
	locationList["Destination"].lat = newDestinationLat;
	locationList["Destination"].lon = newDestinationLong;
    
	setTimedMap();
}

//---------------------------------HANDLE VARIOUS SCREEN STATE----------------------

function initializeScreen()
{	
	console.log("MapActive ," + MapActive + " - PedMonitorActive: " + PedMonitorActive);
	
	if (!MapActive && !PedMonitorActive) {
		initializeMapDiv();
		initializePedMonitorDiv();		
		setActiveImage('show',SystemActiveImage);
	} else if (MapActive && PedMonitorActive) {
		setMapAndPedMonitorDivActive();
	} else if (MapActive && !PedMonitorActive) {
		setMapDivActive();
		initializePedMonitorDiv();
	} else if (!MapActive && PedMonitorActive) {
		initializeMapDiv();
		setPedMonitorDivActive();
	}	
}

function setActiveImage(displayState, image)
{
	//console.log("show avtive imageL " + image);
	if (displayState == 'show') {
		console.log("show avtive image: " + image);
		$("#ActiveImageDiv").show();
		$("#ActiveImage").attr('src', image);
		$("#ActiveImage").show();
	} else {
		$("#ActiveImageDiv").hide();
		$("#ActiveImage").attr('src', image);
		$("#ActiveImage").hide();
	}
}

function initializeMapDiv(){
	$("#TImedMapDiv").removeClass('col-lg-12');
	$("#TImedMapDiv").removeClass('col-lg-6');
	$("#TImedMapDiv").hide();
	$("#MapHeader").hide();
	$("#MapHeader_PickUpTime").text("");
	$("#MapHeader_NumberPassengers").text("");
}

function initializePedMonitorDiv() {
	$("#ActiveImageDiv").removeClass('col-lg-12');
	$("#ActiveImageDiv").removeClass('col-lg-6');
	$("#ActiveImageDiv").hide();
	$("#ActiveImage").width("");
}

function setMapDivActive(){
	initializeMapDiv();
	$("#TImedMapDiv").addClass('col-lg-12');
	$("#TImedMapDiv").show();
	$("#MapHeader").show();
	$("#MapHeader_PickUpTime").text("Pickup Time: " + pickUpTime);
	$("#MapHeader_NumberPassengers").text("Total Number of Passengers: " + numberOfPassengers + " (Regular Seats: " + numberOfRegPassengers + " and Handicap Seats: " + numberOfHandiPassengers + ")");
	refreshMapCenter();
}

function setPedMonitorDivActive() {
	$("#ActiveImageDiv").addClass('col-lg-12');
	$("#ActiveImage").width('');
	$("#ActiveImageDiv").show();
}

function setMapAndPedMonitorDivActive(){
	initializeMapDiv();
	initializePedMonitorDiv();
	$("#TImedMapDiv").addClass('col-lg-6');
	$("#MapHeader").show();
	$("#MapHeader_PickUpTime").text("Pickup Time: " + pickUpTime);
	$("#MapHeader_NumberPassengers").text("Total Number of Passengers: " + numberOfPassengers + " (Regular Seats: " + numberOfRegPassengers + " and Handicap Seats: " + numberOfHandiPassengers + ")");
	$("#TImedMapDiv").show();
	$("#ActiveImageDiv").addClass('col-lg-6');
	$("#ActiveImage").width('100%');
	$("#ActiveImageDiv").show();
	//chaning the map size so we need to refresh it
	refreshMapCenter();
}

//--------------------------HANDLE THE MAP AND PED MONITOR EVENTS-----------------------------

//-------------------------MAP EVENTS-----------------------------
function setTimedMap()
{
	if (!isNetworkOnline()) return;
	
	if (locationList["Vehicle"].lat != 0 && locationList["Vehicle"].lon != 0) {
		if (map == null){
			initializeMap();	
		}		
		
		setMarkers(locationList);

		initializeScreen();
		
	    stopMapTimer();
	    startMapTimer(defaultMapTimout);
	}
}

function stopMapTimer()
{
    if (mapTimerID != null)
    {
        clearTimeout(mapTimerID);
        mapTimerID = null;
    }
}

function startMapTimer(duration)
{
	mapTimerID = setTimeout(clearMap, duration);
}

function clearMap()
{
	stopMapTimer();
    MapActive = false;
	his.payload.HISPreState = "Current PickUp Latitude, Logitude: " + locationList["PickUp"].lat + ", " + locationList["PickUp"].lon + 
	" - Current Destination Latitude, Longitude: " + locationList["Destination"].lat + ", " + locationList["Destination"].lon;
    his.payload.HISPostState = "Removing map from display";
    sendCommand("TEST;" + JSON.stringify(his));        
    initializeScreen()
}

//-------------------------PED MONITOR EVENTS-----------------------------

function setTimedPedMon(image, displayText)
{   
	//Send out the state change message
    if ($("#ActiveImage").attr('src') != image)
    {
        his.payload.HISPreState = $("#ActiveImage").attr('src');
        his.payload.HISPostState = image;
        sendCommand("TEST;" + JSON.stringify(his));       
       $("#ActiveImage").attr('src', image);
    }
    $("#bottomDiv").html(displayText);
	stopPedMonTimer();
	startPedMonTimer(defaultPedMonTimout);
	initializeScreen();
}

function stopPedMonTimer()
{
    if (pedMonTimerID != null)
    {
        clearTimeout(pedMonTimerID);
        pedMonTimerID = null;
    }
}

function restartPedMonTimeout()
{
    stopPedMonTimer();
    startPedMonTimer(defaultPedMonTimout);
}

function startPedMonTimer(duration)
{
	pedMonTimerID = setTimeout(pedMonTimeout, duration);
}


function pedMonTimeout()
{
    //console.log("EPCW Timerout Clearing");
    clearPedMon();
}

function clearPedMon()
{
	stopPedMonTimer();
    PedMonitorActive = false;
    his.payload.HISPreState = ActivePedMonitorImage;
    his.payload.HISPostState = SystemActiveImage;
    sendCommand("TEST;" + JSON.stringify(his));    
    initializeScreen();
}

//--------------------------GOOGLE MAP----------------------------

function refreshMapCenter()
{
	if (!isNetworkOnline()) return;
	
	if (map != null) {
	    var center = map.getCenter();
		google.maps.event.trigger(map, "resize");
	    map.setCenter(center);		
	}
}

function initializeMap() { 
	
	if (!isNetworkOnline()) return;

    map = new google.maps.Map(document.getElementById('TimedMap'), {
        zoom: mapZoom,
        maxZoom: 20,
        minZoom: 1,
        streetViewControl: false,
        center: new google.maps.LatLng(locationList["Vehicle"].lat, locationList["Vehicle"].lon),
        mapTypeId: google.maps.MapTypeId.ROADMAP
    });
    
    google.maps.event.addDomListener(window, "resize", function() {
    	   var center = map.getCenter();
    	   google.maps.event.trigger(map, "resize");
    	   map.setCenter(center); 
    	});	
}

function setMarkers(locObj) {
	
	if (!isNetworkOnline()) return;
	
    $.each(locObj, function (key, loc) {
        if (!locations[key] && loc.lat !== undefined && loc.lon !== undefined) {
            //Marker has not yet been made (and there's enough data to create one).

            //console.log("IN Create Maeker");
            //Create marker
            loc.marker = new google.maps.Marker({
                position: new google.maps.LatLng(loc.lat, loc.lon),
                map: map,
                draggable:true,
                icon: loc.icon
            });

            //Attach click listener to marker
            google.maps.event.addListener(loc.marker, 'click', (function (key) {
                return function () {
                    infowindow.setContent(locations[key].info);
                    infowindow.open(map, locations[key].marker);
                }
            })(key));

            //Remember loc in the `locations` so its info can be displayed and so its marker can be deleted.
            locations[key] = loc;
        } else if (locations[key] && loc.remove) {
            //Remove marker from map
            if (locations[key].marker) {
                locations[key].marker.setMap(null);
            }
            //Remove element from `locations`
            delete locations[key];
        } else if (locations[key]) {
            //Update the previous data object with the latest data.
            $.extend(locations[key], loc);
            if (loc.lat !== undefined && loc.lon !== undefined) {
                //Update marker position (maybe not necessary but doesn't hurt).
                locations[key].marker.setPosition(
                new google.maps.LatLng(loc.lat, loc.lon));
            }
            //locations[key].info looks after itself.
        } else {
        	console.log("Should not get this case!");
        }
    });
}

//THis was to call the set points when the map was shown not using it now
(function ($) {
	  $.each(['show', 'hide'], function (i, ev) {
	    var el = $.fn[ev];
	    $.fn[ev] = function () {
	      this.trigger(ev);
	      return el.apply(this, arguments);
	    };
	  });
})(jQuery);


function isNetworkOnline()
{
	if (typeof google === 'object' && typeof google.maps === 'object') return true;
	
    online = navigator.onLine;
    if (online)
	{
        console.log("Getting scrfipt");
        $.getScript( "https://maps.googleapis.com/maps/api/js?key=AIzaSyB8JwjS7wD5OHE_z-QKjuC4jZc3iEGxE1E" )
          .done(function( script, textStatus ) {
        	  console.log( "SUCCESS" );
            infowindow = new google.maps.InfoWindow();
          })
          .fail(function( jqxhr, settings, exception ) {
        	  console.log( "falie" );
        });
	}	
    
    return online;
}

//--------------------------DOCUMENT----------------------------
$(document).ready(function () {
    $(".headbtn").mousedown(function () {
        $(".headbtn").removeClass('ui-btn-active');
        $(this).addClass('ui-btn-active');
    });
    
    $('#TimedMap').on('show', function() {
    	setMarkers(locationList);
    });
});

//--------------------------GENERAL----------------------------

//Handle disconnect
function disconnectHandler()
{
    console.log("Disconnected - Removing all images");
    clearPedMon();
    clearMap();
    setActiveImage('show',SystemInactiveImage);
}

$(function(){
    $("[data-role='header']").toolbar();
    
    //connectInitial();
});