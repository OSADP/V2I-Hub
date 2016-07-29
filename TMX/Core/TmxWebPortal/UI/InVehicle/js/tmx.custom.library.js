var deviceName = "TMX";
var customLibraryJsVersion = "0.0.1";

// Image timeout variables
var timerID = null;
var defaultTimout = 3000;

// EPCW variables
var EPCWActive = false;
var activeEPCWAlert = false;
var nearEPCWID = null;
var farEPCWID = null;
var leftEPCWID = null;
var rightEPCWID = null;
var nearEPCWSeverity = null;
var farEPCWSeverity = null;
var leftEPCWSeverity = null;
var rightEPCWSeverity = null;
var activeEPCWImage = '../images/E-PCW/3-base-art.png';
var baseImageActiveEPCW = false; 

var EPCWInterval = null;

// Event listener from common.library 
addEventListener("newMessage", newMessageHandler, false);

// Receiver for unhandled messages from the common.library
function newMessageHandler(evt)
{
//    console.log(evt.detail.message);
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
    if (json_obj.AppId == 2) // FCW
    {
        switch (json_obj.Severity) {
        case "0":
//            setTimedImage('../images/FCW/3-base-art.png', json_obj.CustomText);
            break;
        case "1":
            setTimedImage('../images/FCW/4-vehicle-ahead-alert-yellow.png', json_obj.CustomText);
            break;
        case "2":
            setTimedImage('../images/FCW/5-vehicle-ahead-warning-red.png', json_obj.CustomText);
            break;
        default:
            clearImage();
            break;
        }
    }
       if (json_obj.AppId == 3) // EEBL
    {
        switch (json_obj.Severity) {
        case "0":
            setTimedImage('../images/FCW/3-base-art.png', json_obj.CustomText);
            break;
        case "1":
        case "2":
            setTimedImage('../images/EEBL/EEBL.gif', json_obj.CustomText);
            break;
        default:
            clearImage();
            break;
        }
    }
    else if (json_obj.AppId == 6) // EPCW
    {
        stopTimer();
        switch (json_obj.EventCode) {
        case "3": // Entered Area
            clearImage();
            EPCWActive = true;
            $("#ActiveImage").attr('src', '../images/E-PCW/2-system-active.png');
            startTimer(defaultTimout);
            break;
        case "4": // Exited Area
            EPCWActive = false;
            clearEPCW();
            break;
        case "5": // Detected Area
            // No Action
            break;
        case "6": // Near side Pedestrian
            nearEPCWID = json_obj.EventID;
            nearEPCWSeverity = json_obj.Severity;
            displayEPCW();
            break;
        case "7": // Far side Pedestrian
            farEPCWID = json_obj.EventID;
            farEPCWSeverity = json_obj.Severity;
            displayEPCW();
            break;
        case "8": // Left side Pedestrian
            leftEPCWID = json_obj.EventID;
            leftEPCWSeverity = json_obj.Severity;
            displayEPCW();
            break;
        case "9": // Right side Pedestrian
            rightEPCWID = json_obj.EventID;
            rightEPCWSeverity = json_obj.Severity;
            displayEPCW();
            break;
        case "20": // Pedestrian alert complete 
            if (json_obj.EventID == nearEPCWID) nearEPCWID = null;
            else if (json_obj.EventID == farEPCWID) farEPCWID = null;
            else if (json_obj.EventID == leftEPCWID) leftEPCWID = null;
            else if (json_obj.EventID == rightEPCWID) rightEPCWID = null;
            displayEPCW();
            break;
        }
    }
    else if (json_obj.AppId == 7) // EVTRW
    {
       switch (json_obj.Severity) {
        case "0":
            setTimedImage('../images/blank.png', json_obj.CustomText);
            break;
        case "1":
            setTimedImage('../images/VTRW/4-vehicle-passing-left-yellow-arrow.png', json_obj.CustomText);
            break;
        case "2":
            setTimedImage('../images/VTRW/6-vehicle-crossing-path-red-arrow.png', json_obj.CustomText);
            break;
        default:
            clearImage();
            break;
        }
    }
}

function setTimedImage(image, displayText)
{
    stopTimer();
    $("#ActiveImage").attr('src', image);
    $("#bottomDiv").html(displayText);
    startTimer(defaultTimout);
}

function displayEPCW()
{
    if ((nearEPCWID!=null) || (farEPCWID!=null) || (leftEPCWID!=null) || (rightEPCWID!=null))
    {
        if ((nearEPCWID!=null) && (rightEPCWID!=null))
        {
            if ((nearEPCWSeverity == 2) && (rightEPCWSeverity == 2)) activeEPCWImage = '../images/E-PCW/8-pedestrian-double-red.png';
            else if ((nearEPCWSeverity == 2) && (rightEPCWSeverity == 1)) activeEPCWImage = '../images/E-PCW/9-pedestrian-red-yellow.png';
            else if ((nearEPCWSeverity == 1) && (rightEPCWSeverity == 2)) activeEPCWImage = '../images/E-PCW/10-pedestrian-red-yellow.png';
            else if ((nearEPCWSeverity == 1) && (rightEPCWSeverity == 1)) activeEPCWImage = '../images/E-PCW/11-pedestrian-double-yellow.png';
        }
        else if (nearEPCWID!=null)
        {
            if (nearEPCWSeverity == 1) activeEPCWImage = '../images/E-PCW/5-pedestrian-intent-to-cross-nearside-yellow.png';
            else if (nearEPCWSeverity == 2) activeEPCWImage = '../images/E-PCW/4-pedestrian-near-crosswalk-red.png';
        }
        else if (rightEPCWID!=null)
        {
            if (rightEPCWSeverity == 1) activeEPCWImage = '../images/E-PCW/7-pedestrian-intent-to-cross-rightside-yellow.png';
            else if (rightEPCWSeverity == 2) activeEPCWImage = '../images/E-PCW/6-pedestrian-in-rightside-crosswalk-red.png';            
        }
        if (!activeEPCWAlert)
        {
            activeEPCWAlert = true;
            startEPCWDisplay();
        }
    }
    else
    {
        stopEPCWDisplay();
        activeEPCWAlert = false;
    }
    console.log("Active Image:" + activeEPCWImage);

}

function toggleEPCWImage()
{
    if (baseImageActiveEPCW) $("#ActiveImage").attr('src', activeEPCWImage);
    else $("#ActiveImage").attr('src', '../images/E-PCW/3-base-art.png');
    baseImageActiveEPCW = !baseImageActiveEPCW;
}

function startEPCWDisplay()
{
    baseImageActiveEPCW = true;
    toggleEPCWImage();
    EPCWInterval = setInterval(toggleEPCWImage, 300);
}

function stopEPCWDisplay()
{
    clearInterval(EPCWInterval);
    EPCWInterval = null;
    activeEPCWImage = '../images/blank.png';
    $("#ActiveImage").attr('src', activeEPCWImage);
}

function clearEPCW()
{
    stopEPCWDisplay();
    nearEPCWID = null;
    farEPCWID = null;
    leftEPCWID = null;
    rightEPCWID = null;
    nearEPCWSeverity = null;
    farEPCWSeverity = null;
    leftEPCWSeverity = null;
    rightEPCWSeverity = null;
}

function startTimer(duration)
{
    timerID = setTimeout(clearImage, duration);
}

function stopTimer()
{
    if (timerID != null)
    {
        clearTimeout(timerID);
        timerID = null;
    }
}

function clearImage()
{
    stopTimer();
    $("#ActiveImage").attr('src', '../images/blank.png');
    $("#bottomDiv").html("");
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