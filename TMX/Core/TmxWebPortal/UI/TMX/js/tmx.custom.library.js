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

    /*
	var tokens = evt.detail.message.split(";");
	var msgType = tokens[2].toUpperCase();
	if (msgType === "VARIABLES") { TelemetryHandler(evt.detail.message); }
    else if (msgType === "EVENT") { EventHandler(evt.detail.message);}
    else if (msgType === "VERSION_REPLY") { VersionReplyHandler(evt.detail.message); }
    else if (msgType === "PREV_DATA") { PreviousDataHandler(evt.detail.message); }
	else if (msgType === "TIME") {  }
        */
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

    // Test timer for TC

  
    console.log("Setting Interval");

    setInterval(function() {   
        var jsonString = data[counter % data.length];
        // Remote Vehicle Test
//        callProcessTC(jsonString);
//        console.log("Message:" + jsonString + "\nCounter:" + counter + " Length:" + data.length + " modulus:" + counter % data.length);

        // BSM Test
        var bsmJsonString = bsmData[counter % bsmData.length];
//        handleBSM(JSON.parse(bsmJsonString));
//        console.log("Message:" + bsmJsonString + "\nCounter:" + counter + " Length:" + bsmData.length + " modulus:" + counter % bsmData.length);
        counter++;

    }, 500);

   


});

//test Data Array
var data =  [   
                '{"VehicleID":"1042","LastChanged":"1464376140322","LastReceived":"1464376140322","IsSameElevation":"1","IsSimilarDirection":"1","IsAhead":"1","IsLeft":"1","IsFarLane":"1"}',
                '{"VehicleID":"1042","LastChanged":"1464376140322","LastReceived":"1464376140322","IsSameElevation":"1","IsSimilarDirection":"1","IsAhead":"1","IsLeft":"1","IsNearLane":"1"}',
                '{"VehicleID":"1042","LastChanged":"1464376140323","LastReceived":"1464376140323","IsSameElevation":"1","IsSimilarDirection":"1","IsAhead":"1","IsSameLane":"1"}',
                '{"VehicleID":"1042","LastChanged":"1464376140324","LastReceived":"1464376140324","IsSameElevation":"1","IsSimilarDirection":"1","IsAhead":"1","IsRight":"1","IsNearLane":"1"}',
                '{"VehicleID":"1043","LastChanged":"1464376140325","LastReceived":"1464376140322","IsSameElevation":"1","IsSimilarDirection":"1","IsAhead":"1","IsRight":"1","IsFarLane":"1"}',
                '{"VehicleID":"1042","LastChanged":"1464376140322","LastReceived":"1464376140322","IsSameElevation":"1","IsSimilarDirection":"1","IsLeft":"1","IsFarLane":"1"}',
                '{"VehicleID":"1042","LastChanged":"1464376140322","LastReceived":"1464376140322","IsSameElevation":"1","IsSimilarDirection":"1","IsLeft":"1","IsNearLane":"1"}',
                '{"VehicleID":"1042","LastChanged":"1464376140323","LastReceived":"1464376140323","IsSameElevation":"1","IsSimilarDirection":"1","IsSameLane":"1"}',
                '{"VehicleID":"1042","LastChanged":"1464376140324","LastReceived":"1464376140324","IsSameElevation":"1","IsSimilarDirection":"1","IsRight":"1","IsNearLane":"1"}',
                '{"VehicleID":"1045","LastChanged":"1464376140325","LastReceived":"1464376140322","IsSameElevation":"1","IsSimilarDirection":"1","IsRight":"1","IsFarLane":"1"}',
                '{"VehicleID":"1042","LastChanged":"1464376140322","LastReceived":"1464376140322","IsSameElevation":"1","IsSimilarDirection":"1","IsBehind":"1","IsLeft":"1","IsFarLane":"1"}',
                '{"VehicleID":"1042","LastChanged":"1464376140322","LastReceived":"1464376140322","IsSameElevation":"1","IsSimilarDirection":"1","IsBehind":"1","IsLeft":"1","IsNearLane":"1"}',
                '{"VehicleID":"1042","LastChanged":"1464376140323","LastReceived":"1464376140323","IsSameElevation":"1","IsSimilarDirection":"1","IsBehind":"1","IsSameLane":"1"}',
                '{"VehicleID":"1048","LastChanged":"1464376140324","LastReceived":"1464376140324","IsSameElevation":"1","IsSimilarDirection":"1","IsBehind":"1","IsRight":"1","IsNearLane":"1"}',
                '{"VehicleID":"1042","LastChanged":"1464376140325","LastReceived":"1464376140322","IsSameElevation":"1","IsSimilarDirection":"1","IsBehind":"1","IsRight":"1","IsFarLane":"1"}'
            ];


//            <coordinates>
//                ,,0 ,,0 ,,0 ,,0 ,,0 ,,0 ,,0 ,,0 ,,0 ,,0 ,,0 ,,0 ,,0 ,,0 ,,0 -83.02205895108328,39.9907274632118,0 -83.02303785779256,39.99079842285588,0 -83.02395714981115,39.99081143859025,0 -83.02481596365557,39.99084617275785,0 -83.02585841861341,39.99090406481287,0 -83.02600141667449,39.99082589338364,0 -83.02599778081188,39.99059264237849,0 -83.02595541978148,39.99001583651597,0 -83.02586150072767,39.98912928137244,0 -83.02578192341922,39.98838407361343,0 -83.02569178188931,39.98789048512617,0 -83.02553632356012,39.9878091786347,0 -83.02523856379507,39.98780335035207,0 -83.02466792830776,39.9877925974582,0 -83.02384729429053,39.98775691848701,0 -83.0228916334724,39.98769975634421,0 -83.02215530290727,39.98769404842613,0 -83.02136106608899,39.98767034877151,0 

var bsmData =   [
//                    '{"header":{"type":"Decoded","subtype":"BSM","source":"Decode and Forward","sourceId":30,"encoding":"json","timestamp":1465916272153,"flags":0},"payload":{"IsLocationValid":"1","IsElevationValid":"1","IsSpeedValid":"1","IsHeadingValid":"0","IsSteeringWheelAngleValid":"1","TemporaryId":"708530215","Latitude":"39.989149699999999","Longitude":"-83.020620300000004","Elevation_m":"210.199997","Speed_mph":"0.0447387248","SteeringWheelAngle":"-193.5","IsOutgoing":"0"}}'
                    '{"IsLocationValid":"1","IsElevationValid":"1","IsSpeedValid":"1","IsHeadingValid":"0","IsSteeringWheelAngleValid":"1","TemporaryId":"708530215","Latitude":"39.98766260124449","Longitude":"-83.02077016907667","Elevation_m":"210.199997","Speed_mph":"0","SteeringWheelAngle":"-193.5","IsOutgoing":"0"}',
//                    '{"IsLocationValid":"1","IsElevationValid":"1","IsSpeedValid":"1","IsHeadingValid":"0","IsSteeringWheelAngleValid":"1","TemporaryId":"708530216","Latitude":"39.98764788105873","Longitude":"-83.020620300000004","Elevation_m":"210.199997","Speed_mph":"0","SteeringWheelAngle":"-193.5","IsOutgoing":"0"}',
                    '{"IsLocationValid":"1","IsElevationValid":"1","IsSpeedValid":"1","IsHeadingValid":"0","IsSteeringWheelAngleValid":"1","TemporaryId":"708530215","Latitude":"39.98764788105873","Longitude":"-83.02013122282311","Elevation_m":"210.199997","Speed_mph":"1","SteeringWheelAngle":"-193.5","IsOutgoing":"0"}',
                    '{"IsLocationValid":"1","IsElevationValid":"1","IsSpeedValid":"1","IsHeadingValid":"0","IsSteeringWheelAngleValid":"1","TemporaryId":"708530215","Latitude":"39.98762261073677","Longitude":"-83.01965328461573","Elevation_m":"210.199997","Speed_mph":"2","SteeringWheelAngle":"-93.5","IsOutgoing":"0"}',
                    '{"IsLocationValid":"1","IsElevationValid":"1","IsSpeedValid":"1","IsHeadingValid":"0","IsSteeringWheelAngleValid":"1","TemporaryId":"708530215","Latitude":"39.98759635649678","Longitude":"-83.01910704416699","Elevation_m":"210.199997","Speed_mph":"3","SteeringWheelAngle":"-3.5","IsOutgoing":"0"}',
                    '{"IsLocationValid":"1","IsElevationValid":"1","IsSpeedValid":"1","IsHeadingValid":"0","IsSteeringWheelAngleValid":"1","TemporaryId":"708530215","Latitude":"39.98758300229493","Longitude":"-83.01864556713778","Elevation_m":"210.199997","Speed_mph":"4","SteeringWheelAngle":"193.5","IsOutgoing":"0"}',
                    '{"IsLocationValid":"1","IsElevationValid":"1","IsSpeedValid":"1","IsHeadingValid":"0","IsSteeringWheelAngleValid":"1","TemporaryId":"708530215","Latitude":"39.98754921191781","Longitude":"-83.01817156675632","Elevation_m":"210.199997","Speed_mph":"5","SteeringWheelAngle":"93.5","IsOutgoing":"0"}',
                    '{"IsLocationValid":"1","IsElevationValid":"1","IsSpeedValid":"1","IsHeadingValid":"0","IsSteeringWheelAngleValid":"1","TemporaryId":"708530215","Latitude":"39.98758243172519","Longitude":"-83.01785798719594","Elevation_m":"210.199997","Speed_mph":"6","SteeringWheelAngle":"3.5","IsOutgoing":"0"}',
                    '{"IsLocationValid":"1","IsElevationValid":"1","IsSpeedValid":"1","IsHeadingValid":"0","IsSteeringWheelAngleValid":"1","TemporaryId":"708530215","Latitude":"39.98825377667155","Longitude":"-83.01780589985268","Elevation_m":"210.199997","Speed_mph":"7","SteeringWheelAngle":"-3.5","IsOutgoing":"0"}',
                    '{"IsLocationValid":"1","IsElevationValid":"1","IsSpeedValid":"1","IsHeadingValid":"0","IsSteeringWheelAngleValid":"1","TemporaryId":"708530215","Latitude":"39.98885245294227","Longitude":"-83.01776325046242","Elevation_m":"210.199997","Speed_mph":"8","SteeringWheelAngle":"-93.5","IsOutgoing":"0"}',
                    '{"IsLocationValid":"1","IsElevationValid":"1","IsSpeedValid":"1","IsHeadingValid":"0","IsSteeringWheelAngleValid":"1","TemporaryId":"708530215","Latitude":"39.98968659447217","Longitude":"-83.01772119438205","Elevation_m":"210.199997","Speed_mph":"7","SteeringWheelAngle":"-193.5","IsOutgoing":"0"}',
                    '{"IsLocationValid":"1","IsElevationValid":"1","IsSpeedValid":"1","IsHeadingValid":"0","IsSteeringWheelAngleValid":"1","TemporaryId":"708530215","Latitude":"39.990291130747","Longitude":"-83.01766876451679","Elevation_m":"210.199997","Speed_mph":"7","SteeringWheelAngle":"-193.5","IsOutgoing":"0"}',
                    '{"IsLocationValid":"1","IsElevationValid":"1","IsSpeedValid":"1","IsHeadingValid":"0","IsSteeringWheelAngleValid":"1","TemporaryId":"708530215","Latitude":"39.99058064599134","Longitude":"-83.01769127097492","Elevation_m":"210.199997","Speed_mph":"7","SteeringWheelAngle":"-193.5","IsOutgoing":"0"}',
                    '{"IsLocationValid":"1","IsElevationValid":"1","IsSpeedValid":"1","IsHeadingValid":"0","IsSteeringWheelAngleValid":"1","TemporaryId":"708530215","Latitude":"39.99062210829456","Longitude":"-83.01855701729942","Elevation_m":"210.199997","Speed_mph":"7","SteeringWheelAngle":"-193.5","IsOutgoing":"0"}',
                    '{"IsLocationValid":"1","IsElevationValid":"1","IsSpeedValid":"1","IsHeadingValid":"0","IsSteeringWheelAngleValid":"1","TemporaryId":"708530215","Latitude":"39.9906681052687","Longitude":"-83.01987678638511","Elevation_m":"210.199997","Speed_mph":"7","SteeringWheelAngle":"-193.5","IsOutgoing":"0"}',
 
                    '{"IsLocationValid":"1","IsElevationValid":"1","IsSpeedValid":"1","IsHeadingValid":"0","IsSteeringWheelAngleValid":"1","TemporaryId":"708530217","Latitude":"39.98825377667155","Longitude":"-83.01780589985268","Elevation_m":"210.199997","Speed_mph":"7","SteeringWheelAngle":"-193.5","IsOutgoing":"0"}',
                    '{"IsLocationValid":"1","IsElevationValid":"1","IsSpeedValid":"1","IsHeadingValid":"0","IsSteeringWheelAngleValid":"1","TemporaryId":"708530217","Latitude":"39.98885245294227","Longitude":"-83.01776325046242","Elevation_m":"210.199997","Speed_mph":"8","SteeringWheelAngle":"-193.5","IsOutgoing":"0"}',
                    '{"IsLocationValid":"1","IsElevationValid":"1","IsSpeedValid":"1","IsHeadingValid":"0","IsSteeringWheelAngleValid":"1","TemporaryId":"708530217","Latitude":"39.98968659447217","Longitude":"-83.01772119438205","Elevation_m":"210.199997","Speed_mph":"7","SteeringWheelAngle":"-193.5","IsOutgoing":"0"}',
                    '{"IsLocationValid":"1","IsElevationValid":"1","IsSpeedValid":"1","IsHeadingValid":"0","IsSteeringWheelAngleValid":"1","TemporaryId":"708530217","Latitude":"39.990291130747","Longitude":"-83.01766876451679","Elevation_m":"210.199997","Speed_mph":"7","SteeringWheelAngle":"-193.5","IsOutgoing":"0"}',
                    '{"IsLocationValid":"1","IsElevationValid":"1","IsSpeedValid":"1","IsHeadingValid":"0","IsSteeringWheelAngleValid":"1","TemporaryId":"708530217","Latitude":"39.99058064599134","Longitude":"-83.01769127097492","Elevation_m":"210.199997","Speed_mph":"7","SteeringWheelAngle":"-193.5","IsOutgoing":"0"}',
                    '{"IsLocationValid":"1","IsElevationValid":"1","IsSpeedValid":"1","IsHeadingValid":"0","IsSteeringWheelAngleValid":"1","TemporaryId":"708530217","Latitude":"39.99062210829456","Longitude":"-83.01855701729942","Elevation_m":"210.199997","Speed_mph":"7","SteeringWheelAngle":"-193.5","IsOutgoing":"0"}',

                    '{"IsLocationValid":"1","IsElevationValid":"1","IsSpeedValid":"1","IsHeadingValid":"0","IsSteeringWheelAngleValid":"1","TemporaryId":"708530215","Latitude":"39.9906920081979","Longitude":"-83.02112522515054","Elevation_m":"210.199997","Speed_mph":"7","SteeringWheelAngle":"-193.5","IsOutgoing":"0"}'
                ];



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
    processTC(json_obj.payload);
    tcTableUpdate(json_obj.payload);
}

function handleBSM(json_obj)
{

//    console.log(jsonString);

//    var json_obj = JSON.parse(jsonString);
//    console.log("BSM Received");
//    console.log(json_obj);

    var found = false;
    // Look for an object that contains the same telemetry object
//    console.log("Length:" + bsmVehicleList.length)
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
    console.log("Path elements:" + path.getLength());
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

//    console.log(jsonString);

//    var json_obj = JSON.parse(jsonString);
//    console.log("BSM Received");
//    console.log(json_obj);

/*
    msg.set_Altitude(_altitude);
    msg.set_Heading(_heading);
    msg.set_HorizontalDOP(_HDOP);
    msg.set_Latitude(_lat);
    msg.set_Longitude(_long);
    msg.set_NumSatellites(_numberSats);
    msg.set_Speed(_speed);

*/

    var found = false;
    // Look for an object that contains the same telemetry object
//    console.log("Length:" + bsmVehicleList.length)
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
    console.log("Path elements:" + path.getLength());
    if (path.getLength() >= 90) 
    {
        path.removeAt(0);
    }
    path.push(position);    

    updateBSMTable();
    updateMap();

}


//var batMobile = '../images/batmobile.png';


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

    $('#bsmtable').DataTable().clear();
    for (var i = 0; i < bsmVehicleList.length; i++) {
        var tempObj = bsmVehicleList[i];
        $('#bsmtable').DataTable().row.add([tempObj.Latitude, tempObj.Longitude,tempObj.Elevation_m,tempObj.Speed, tempObj.Heading, tempObj.NumSatellites, tempObj.HorizontalDOP]);

    }
    $('#bsmtable').DataTable().draw();

}


function tcTableUpdate(jsonObj)
{
//    console.log("Process tcTabletUpdate");
//    console.log(jsonObj);

    if (jsonObj.VehicleID != null) $("#tcID").html(jsonObj.VehicleID);
    if (jsonObj.LateralSeparation != null) $("#tcLatSep").html(jsonObj.LateralSeparation);
    if (jsonObj.LongitudinalSeparation != null) $("#tcLongSep").html(jsonObj.LongitudinalSeparation);
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

    if (jsonObj.IsSameElevation) {
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


    markerDot = {
 //       url: "../images/cv_icon_64x32.png",
        url: "../images/indicatordotdarkblue.png", // url
//        scaledSize: new google.maps.Size(30, 50), // scaled size
        origin: new google.maps.Point(0,0), // origin
        anchor: new google.maps.Point(5, 5), // anchor
//        anchor: new google.maps.Point(8, 16), // anchor
//        scaledSize: new google.maps.Size(16, 32),


    };

        poly = new google.maps.Polyline({
//
          strokeColor: '#4169E1',
          strokeOpacity: 1.0,
          strokeWeight: 3
        });
        poly.setMap(map);    

/*
    batMobile = {
            url: '../images/red_racer.svg',
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

    batMobile = {
        path: google.maps.SymbolPath.BACKWARD_CLOSED_ARROW,
        scale: 5,
        strokeWeight:2,
        strokeColor:"#B40404",
        rotation: 25
    };



/*
    batMobile = {
        url: "../images/batmobile.png", // url
        scaledSize: new google.maps.Size(30, 50), // scaled size
        origin: new google.maps.Point(0,0), // origin
        anchor: new google.maps.Point(15, 25), // anchor
        rotation: 45

    };
*/

  }
