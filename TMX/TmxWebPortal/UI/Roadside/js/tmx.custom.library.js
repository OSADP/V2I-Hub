var deviceName = "InVehicle";
var customLibraryJsVersion = "0.0.1";


// Event listener from common.library 
addEventListener("newMessage", newMessageHandler, false);

// Receiver for unhandled messages from the common.library
function newMessageHandler(evt)
{
	//console.log("IN MEssage handler");
    //console.log(evt.detail.message);
    var json_obj = JSON.parse(evt.detail.message);
    if (json_obj.header.subtype === "Application") { handleAppMessage(json_obj.payload); }
    else if (json_obj.header.subtype === "Basic") { handleAppMessage(json_obj.payload); }
    else if (json_obj.header.type === "__config") { 
        // Need to handle 
        if (json_obj.payload != undefined)
            handleConfig(json_obj.payload[0]); 
    }    
//    else { console.log(timeStamp(evt.detail.time) + " Unknown Message: " + evt.detail.message);	}
}
/*
This function handles all of the Application:Application messages and displays the appropriate 
Image.
*/
function handleAppMessage(json_obj)
{

    console.log(json_obj);

}

function handleConfig(json_obj) {

	if (json_obj.key == "OperationMode") {
		if (json_obj.value == "MaintMode") {
			console.log("In MaintMode");
	        screenNotification = false;
	        soundNotification = false;
	        $("#ActiveImage").addClass('hidden');
	        $("#CloakImage").addClass('hidden');    		
		} else {
			console.log("Not In MaintMode");
	        screenNotification = true;
	        soundNotification = true;
	        $("#ActiveImage").removeClass('hidden');
	        $("#CloakImage").addClass('hidden');    		
		}
	}
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