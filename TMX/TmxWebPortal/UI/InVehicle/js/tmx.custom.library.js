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

$(function(){
    $("[data-role='header']").toolbar();
});

$(document).ready(function () {
    $(".headbtn").mousedown(function () {
        $(".headbtn").removeClass('ui-btn-active');
        $(this).addClass('ui-btn-active');
    });

});