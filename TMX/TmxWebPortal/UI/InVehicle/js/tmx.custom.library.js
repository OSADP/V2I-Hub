var deviceName = "InVehicle";
var customLibraryJsVersion = "0.0.1";


// Event listener from common.library 
addEventListener("newMessage", newMessageHandler, false);

// Receiver for unhandled messages from the common.library
function newMessageHandler(evt)
{
    console.log(evt.detail.message);
    var json_obj = JSON.parse(evt.detail.message);
    if (json_obj.header.subtype === "Application") { handleAppMessage(json_obj.payload); }
    else if (json_obj.header.subtype === "Basic") { handleAppMessage(json_obj.payload); }
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


$(function(){
    $("[data-role='header']").toolbar();
});

$(document).ready(function () {
    $(".headbtn").mousedown(function () {
        $(".headbtn").removeClass('ui-btn-active');
        $(this).addClass('ui-btn-active');
    });

});