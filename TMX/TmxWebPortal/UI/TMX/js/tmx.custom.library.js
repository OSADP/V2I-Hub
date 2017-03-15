var deviceName = "TMX";
var customLibraryJsVersion = "0.0.1";

var bsmVehicleList = [];

var spatCount = 0;
var mapCount = 0;
var map; // Google map
var gmarkers = [];
var batMobile = '../images/batmobile.png'; 
var markerDot = null;
var poly;

var latSep = null;
var longSep = null;
var seperation = null;
var lastUpdateTime = 0;



console.log("Host IP:" + document.location.hostname);
// Event listener from common.library 
addEventListener("newMessage", newMessageHandler, false);

// Receiver for unhandled messages from the common.library
function newMessageHandler(evt)
{
    var json_obj = JSON.parse(evt.detail.message);
//    console.log("newMessageHandler:" + evt.detail.message);
    if (json_obj.header.subtype === "SPAT-P") { processSpat(json_obj); }
    else if (json_obj.header.subtype === "MAP-P") { processMap(json_obj); }
    else if (json_obj.header.subtype === "RemoteVehicle") { handleTC(json_obj); }
    else if (json_obj.header.subtype === "Remote") { handleTC(json_obj); }
//    else if (json_obj.header.subtype === "BSM") { 
//        handleBSM(json_obj.payload); 
//    }
    else if (json_obj.header.subtype === "LocationMessage") { 
        handleLocation(json_obj.payload); 
    }
	else
	{
//		console.log(timeStamp(evt.detail.time) + "tmxUI Unknown Message: " + evt.detail.message);		
	}
    
}

function processSpat(jsonObj)
{
    spatCount++;
    document.getElementById("spat_count").innerHTML = spatCount;
    document.getElementById("spat_ts").innerHTML = jsonObj.header.timestamp;
    document.getElementById("spat_source").innerHTML = jsonObj.header.source;
//    document.getElementById("spat_payload").innerHTML = jsonObj.payload;
    $("#spat_payload").val(jsonObj.payload);
}

function processMap(jsonObj)
{
    mapCount++;
    document.getElementById("map_count").innerHTML = mapCount;
    document.getElementById("map_ts").innerHTML = jsonObj.header.timestamp;
    document.getElementById("map_source").innerHTML = jsonObj.header.source;
//    document.getElementById("map_payload").innerHTML = jsonObj.payload;
    $("map_payload").val(jsonObj.payload);
}


// Header
$(function(){
	$("[data-role='header']").toolbar();
});

$(document).ready(function () {
    $(".headbtn").mousedown(function () {
        $(".headbtn").removeClass('ui-btn-active');
        $(this).addClass('ui-btn-active');
    });

//    document.getElementById("identifier_label").innerHTML = "Type:" + radioSettings.Type + " ID:" + radioSettings.ID + " Mode:" + radioSettings.Mode;
});

var counter = 0;

function callProcessTC(jsonString)
{

    var json_obj = JSON.parse(jsonString);
    var json_payload = JSON.parse(json_obj.payload);
    processTC(json_payload);
    tcTableUpdate(json_payload);
}

function handleTC(json_obj)
{
//    console.log("Process TC Payload");
 //   console.log(json_obj);
    currentTime = new Date().getTime();
    if ((currentTime - lastUpdateTime) > 300)
    {
        lastUpdateTime = currentTime;
        processTC(json_obj.payload);
        tcTableUpdate(json_obj.payload);
    }

}

function handleBSM(json_obj)
{


    var found = false;
    // Look for an object that contains the same telemetry object
    for (var i = 0; i < bsmVehicleList.length; i++) {
        var tempObj = bsmVehicleList[i];
        if (tempObj.TemporaryId == json_obj.TemporaryId)
        {
            // Found Vehicle - update current
            found = true;
            bsmVehicleList[i] = json_obj;
        }

    }
    // If vehicle was not found add it to vector
    if (!found)
    {
        var newObj = json_obj;
        bsmVehicleList.push(newObj);
    }
    position = new google.maps.LatLng(parseFloat(json_obj.Latitude),parseFloat(json_obj.Longitude));
    map.panTo(position);

    var path = poly.getPath();
    if (path.getLength() >= 90) 
    {
//        path.pop();
        path.removeAt(0);
    }
    path.push(position);    

    updateBSMTable();
    updateMap();

}

function handleLocation(json_obj)
{
    /*

    var found = false;
    // Look for an object that contains the same telemetry object
    for (var i = 0; i < bsmVehicleList.length; i++) {
        var tempObj = bsmVehicleList[i];
        if (tempObj.TemporaryId == json_obj.TemporaryId)
        {
//            console.log(tempObj);
            // Found Vehicle - update current
            found = true;
            bsmVehicleList[i] = json_obj;
        }

    }
    // If vehicle was not found add it to vector
    if (!found)
    {
        var newObj = json_obj;
        bsmVehicleList.push(newObj);
    }
    position = new google.maps.LatLng(parseFloat(json_obj.Latitude),parseFloat(json_obj.Longitude));
    map.panTo(position);

    var path = poly.getPath();
    if (path.getLength() >= 90) 
    {
        path.removeAt(0);
    }
    path.push(position);    

    updateBSMTable();
    updateMap();
    */

}

function updateMap()
{
    var myLatLng;
    var found = false;

    for (var i = 0; i < bsmVehicleList.length; i++) {
        var tempObj = bsmVehicleList[i];
        for (var j = 0; j < gmarkers.length; j++) {
            var tempMark = gmarkers[j];
            if (tempObj.TemporaryId == tempMark.title)
            {
                position = new google.maps.LatLng(parseFloat(tempObj.Latitude),parseFloat(tempObj.Longitude));
                tempMark.setPosition(position);
//                console.log("Panning old " + tempObj.TemporaryId);
//                map.panTo(tempMark.getPosition());
//                tempMark.icon.rotation = parseFloat(tempObj.SteeringWheelAngle);
                found = true;
                break;
            } 
        }
        if (!found)
        {
            myLatLng = {lat: parseFloat(tempObj.Latitude), lng: parseFloat(tempObj.Longitude)};

            var marker = new google.maps.Marker({
                position: myLatLng,
                map: map,
                title: tempObj.TemporaryId,
                icon: markerDot
            });
            gmarkers.push(marker);
//            console.log("Panning new");
//            map.panTo(marker.getPosition());

        }
        found = false;
    }
}

function removeMarkers(){
    for(i=0; i<gmarkers.length; i++){
        gmarkers[i].setMap(null);
    }
}

function updateBSMTable()
{

    /*
                                                   msg.set_Altitude(_altitude);
    msg.set_Heading(_heading);
    msg.set_HorizontalDOP(_HDOP);
    msg.set_Latitude(_lat);
    msg.set_Longitude(_long);
    msg.set_NumSatellites(_numberSats);
    msg.set_Speed(_speed);

                                                <th class="heading">Latitude</th>
                                                <th class="heading">Longitude</th>
                                                <th class="heading">Elevation</th>
                                                <th class="heading">Speed</th>
                                                <th class="heading">Heading</th>
                                                <th class="heading">Satellites</th>
                                                <th class="heading">HDOP</th>
 */

/*
    $('#bsmtable').DataTable().clear();
    for (var i = 0; i < bsmVehicleList.length; i++) {
        var tempObj = bsmVehicleList[i];
        $('#bsmtable').DataTable().row.add([tempObj.Latitude, tempObj.Longitude,tempObj.Elevation_m,tempObj.Speed, tempObj.Heading, tempObj.NumSatellites, tempObj.HorizontalDOP]);

    }
    $('#bsmtable').DataTable().draw();
*/
}


function tcTableUpdate(jsonObj)
{
//    console.log("Process tcTabletUpdate");
//    console.log(jsonObj);


    if (jsonObj.VehicleID != null) $("#tcID").html(jsonObj.VehicleID);
    if ((jsonObj.LateralSeparation != null) && (jsonObj.LongitudinalSeparation != null))
    {

        latSep = parseFloat(jsonObj.LateralSeparation) / 1000;
        longSep = parseFloat(jsonObj.LongitudinalSeparation) / 1000;

        seperation = (Math.sqrt(Math.pow(latSep,2) + Math.pow(longSep,2))).toFixed(3);
        
        latSep = latSep.toFixed(3);
        longSep = longSep.toFixed(3);

        $("#tcSeperation").html(seperation.toString());
        $("#tcLatSep").html(latSep.toString());
        $("#tcLongSep").html(longSep.toString());

    }
    if (jsonObj.RSSI != null) $("#tcRSSI").html(jsonObj.RSSI);
    if (jsonObj.LongitudinalAcceleration != null) $("#tcLongAcc").html(jsonObj.LongitudinalAcceleration);
    if (jsonObj.Speed != null) $("#tcSpeed").html(jsonObj.Speed);
    if (jsonObj.Latitude != null) $("#tcLat").html(jsonObj.Latitude);
    if (jsonObj.Longitude != null) $("#tcLong").html(jsonObj.Longitude);
    if (jsonObj.LateralAcceleration != null) $("#tcLatAcc").html(jsonObj.LateralAcceleration);
    /*
    if (jsonObj.LastChanged != null) $("#tcChanged").html(jsonObj.LastChanged);
    if (jsonObj.LastReceived != null) $("#tcReceive").html(jsonObj.LastReceived);
    if (jsonObj.RxCount != null) $("#tcRX").html(jsonObj.RxCount);
    if (jsonObj.SeqNumber != null) $("#tcSeq").html(jsonObj.SeqNumber);
    if (jsonObj.ExpiryTime != null) $("#Expire").html(jsonObj.ExpiryTime);

    if (jsonObj.VectorLatDegrees != null) $("#tcLat").html(jsonObj.VectorLatDegrees);
    if (jsonObj.VectorLonDegrees != null) $("#tcLong").html(jsonObj.VectorLonDegrees);
    if (jsonObj.VectorElevation != null) $("#tcElevation").html(jsonObj.VectorElevation);
    if (jsonObj.VectorHeading != null) $("#tcHeading").html(jsonObj.VectorHeading);
    if (jsonObj.VectorCosHeading != null) $("#tcCosHeading").html(jsonObj.VectorCosHeading);
    if (jsonObj.VectorSinHeading != null) $("#tcSinHeading").html(jsonObj.VectorSinHeading);
    if (jsonObj.VectorSpeed != null) $("#tcSpeed").html(jsonObj.VectorSpeed);
    if (jsonObj.VectorAdvanceOK != null) $("#tcAdvanceOK").html(jsonObj.VectorAdvanceOK);

    if (jsonObj.VectorLatAcceleration != null) $("#tcLatAcc").html(jsonObj.VectorLatAcceleration);
    if (jsonObj.VectorLonAcceleration != null) $("#tcLongAcc").html(jsonObj.VectorLonAcceleration);
    if (jsonObj.VectorRadius != null) $("#tcRadius").html(jsonObj.VectorRadius);
    if (jsonObj.VectorNormalizedMinuteUnit != null) $("#tcNormalizedMinuteUnit").html(jsonObj.VectorNormalizedMinuteUnit);
    if (jsonObj.VectorSeparationX10 != null) $("#tcX10").html(jsonObj.VectorSeparationX10);
    if (jsonObj.VectorSeparationY10 != null) $("#tcY10").html(jsonObj.VectorSeparationY10);
    if (jsonObj.VectorSeparationE10 != null) $("#tcE10").html(jsonObj.VectorSeparationE10);
    if (jsonObj.VectorSeparationR10 != null) $("#tcR10").html(jsonObj.VectorSeparationR10);

    if (jsonObj.VectorSeparationV10 != null) $("#tcV10").html(jsonObj.VectorSeparationV10);
    if (jsonObj.VectorSeparationA10 != null) $("#tcA10").html(jsonObj.VectorSeparationA10);
    if (jsonObj.VectorSeparationB10 != null) $("#tcB10").html(jsonObj.VectorSeparationB10);
    if (jsonObj.VectorSeparationH10 != null) $("#tcH10").html(jsonObj.VectorSeparationH10);
    if (jsonObj.VectorSeparationRB10 != null) $("#tcRB10").html(jsonObj.VectorSeparationRB10);
    if (jsonObj.VectorSeparationT10 != null) $("#tcT10").html(jsonObj.VectorSeparationT10);
    */

    if (jsonObj.IsSameElevation) $("#sameElev").attr("data-state", "blue"); else $("#sameElev").attr("data-state", "black");
    if (jsonObj.IsDifferentElevation) $("#diffElev").attr("data-state", "blue"); else $("#diffElev").attr("data-state", "black");
    if (jsonObj.IsSimilarDirection) $("#simDir").attr("data-state", "blue"); else $("#simDir").attr("data-state", "black");
    if (jsonObj.IsOppositeDirection) $("#OppDir").attr("data-state", "blue"); else $("#OppDir").attr("data-state", "black");
    if (jsonObj.IsClosing) $("#Closing").attr("data-state", "blue"); else $("#Closing").attr("data-state", "black");
    if (jsonObj.IsReceding) $("#Receding").attr("data-state", "blue"); else $("#Receding").attr("data-state", "black");
    if (jsonObj.IsAhead) $("#Ahead").attr("data-state", "blue"); else $("#Ahead").attr("data-state", "black");
    if (jsonObj.IsBehind) $("#Behind").attr("data-state", "blue"); else $("#Behind").attr("data-state", "black");
    if (jsonObj.IsSameLane) $("#SameLane").attr("data-state", "blue"); else $("#SameLane").attr("data-state", "black");
    if (jsonObj.IsNearLane) $("#NearLane").attr("data-state", "blue"); else $("#NearLane").attr("data-state", "black");
    if (jsonObj.IsFarLane) $("#FarLane").attr("data-state", "blue"); else $("#FarLane").attr("data-state", "black");
    if (jsonObj.IsLeft) $("#Left").attr("data-state", "blue"); else $("#Left").attr("data-state", "black"); 
    if (jsonObj.IsRight) $("#Right").attr("data-state", "blue"); else $("#Right").attr("data-state", "black");
    if (jsonObj.IsXing) $("#Xing").attr("data-state", "blue"); else $("#Xing").attr("data-state", "black");
    if (jsonObj.IsXing) $("#LCXing").attr("data-state", "blue"); else $("#LCXing").attr("data-state", "black");
    if (jsonObj.IsXing) $("#LLXing").attr("data-state", "blue"); else $("#LLXing").attr("data-state", "black");
    if (jsonObj.IsXing) $("#CCXing").attr("data-state", "blue"); else $("#CCXing").attr("data-state", "black");

}



function processTC(jsonObj)
{
    /*

    {"VehicleID":"1042","LastChanged":"1464376140322","LastReceived":"1464376140322","IsBehind":"1","IsSameLane":"1"}


    /**
        VehicleID RxCount SeqNumber LastChanged LastReceived ExpiryTime IsSameElevation IsDifferentElevation
        IsSimilarDirection IsOppositeDirection IsClosing IsReceding IsAhead IsBehind IsSameLane IsNearLane
        IsFarLane IsLeft IsRight IsXing
    */
    resetAllActive();

    if (!jsonObj.IsDifferentElevation) {
//        console.log("IsSameElevation");
        if (jsonObj.IsSimilarDirection) {
//            console.log("IsSimilarDirection");

            if (jsonObj.IsAhead) {
//                console.log("IsAhead");
                if (jsonObj.IsSameLane) {
//                    console.log("IsSameLane");
                     $("#F").css( "background-color", "#32AFFF" );
                }
                else if (jsonObj.IsLeft) {
//                    console.log("IsLeft");
                    if (jsonObj.IsNearLane) {
//                        console.log("IsNearLane");
                        $("#FL").css( "background-color", "#32AFFF" );
                    }
                    else if (jsonObj.IsFarLane) {
//                        console.log("IsFarLane");
                        $("#FFL").css( "background-color", "#32AFFF" );
                    }
                }
                else if (jsonObj.IsRight) {
//                    console.log("IsRight");
                    if (jsonObj.IsNearLane) {
//                        console.log("IsNearLane");
                        $("#FR").css( "background-color", "#32AFFF" );
                    }
                    else if (jsonObj.IsFarLane) {
//                        console.log("IsFarLane");
                        $("#FFR").css( "background-color", "#32AFFF" );
                    }                    
                }
            }
            else if (jsonObj.IsBehind) {
//                console.log("IsBehind");
                if (jsonObj.IsSameLane) {
//                    console.log("IsSameLane");
                     $("#B").css( "background-color", "#32AFFF" );
                }
                else if (jsonObj.IsLeft) {
//                    console.log("IsLeft");
                    if (jsonObj.IsNearLane) {
//                        console.log("IsNearLane");
                        $("#BL").css( "background-color", "#32AFFF" );
                    }
                    else if (jsonObj.IsFarLane) {
//                        console.log("IsFarLane");
                        $("#BFL").css( "background-color", "#32AFFF" );
                    }
                }
                else if (jsonObj.IsRight) {
//                    console.log("IsRight");
                    if (jsonObj.IsNearLane) {
//                        console.log("IsNearLane");
                        $("#BR").css( "background-color", "#32AFFF" );
                    }
                    else if (jsonObj.IsFarLane) {
//                        console.log("IsFarLane");
                        $("#BFR").css( "background-color", "#32AFFF" );
                    }                    
                }
            }
            else 
            {
//                console.log("IsEven");
                if (jsonObj.IsSameLane) {
//                    console.log("IsSameLane");
                     $("#myVech").css( "background-color", "#32AFFF" );
                }
                else if (jsonObj.IsLeft) {
//                    console.log("IsLeft");
                    if (jsonObj.IsNearLane) {
//                        console.log("IsNearLane");
                        $("#SL").css( "background-color", "#32AFFF" );
                    }
                    else if (jsonObj.IsFarLane) {
//                        console.log("IsFarLane");
                        $("#SFL").css( "background-color", "#32AFFF" );
                    }
                }
                else if (jsonObj.IsRight) {
//                    console.log("IsRight");
                    if (jsonObj.IsNearLane) {
//                        console.log("IsNearLane");
                        $("#SR").css( "background-color", "#32AFFF" );
                    }
                    else if (jsonObj.IsFarLane) {
//                        console.log("IsFarLane");
                        $("#SFR").css( "background-color", "#32AFFF" );
                    }                    
                }                

            }
        }
    }

}

function resetAllActive()
{
     $("#FFL").css("background-color","#f9f9f9");
     $("#FL").css("background-color","#f9f9f9");
     $("#F").css("background-color","#f9f9f9");
     $("#FR").css("background-color","#f9f9f9");
     $("#FFR").css("background-color","#f9f9f9");
     $("#SFL").css("background-color","#f9f9f9");
     $("#SL").css("background-color","#f9f9f9");
     $("#myVech").css("background-color","#f9f9f9");
     $("#SR").css("background-color","#f9f9f9");
     $("#SFR").css("background-color","#f9f9f9");
     $("#BFL").css("background-color","#f9f9f9");
     $("#BL").css("background-color","#f9f9f9");
     $("#B").css("background-color","#f9f9f9");
     $("#BR").css("background-color","#f9f9f9");
     $("#BFR").css("background-color","#f9f9f9");
}

//--------- Map ------------------------------------------------


function initMap() {

    map = new google.maps.Map(document.getElementById('map'), {
          center: {lat: 39.98914969, lng: -83.020620300},
          zoom: 16
    });

/*
    markerDot = {
        url: "../images/indicatordotdarkblue.png", // url
        origin: new google.maps.Point(0,0), // origin
        anchor: new google.maps.Point(5, 5), // anchor
    };
*/
    markerDot = {
            url: '../images/car.svg',
            rotation: 10,
//            size: new google.maps.Size(30, 50),
//            origin: new google.maps.Point(0, 0),
//            anchor: new google.maps.Point(15, 25),
//            scale: 0.5,
//            rotation: 20

//            size: new google.maps.Size(71, 71),
//            origin: new google.maps.Point(0, 0),
            anchor: new google.maps.Point(0, 17.5),
            scaledSize: new google.maps.Size(100, 35)
                        
    };
    poly = new google.maps.Polyline({
      strokeColor: '#4169E1',
      strokeOpacity: 1.0,
      strokeWeight: 3
    });
    poly.setMap(map);    
  }
