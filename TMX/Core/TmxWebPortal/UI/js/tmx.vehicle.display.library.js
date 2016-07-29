console.log("Including tmx.vehicle.display.library.js")
var deviceName = "TMX";
var customLibraryJsVersion = "0.0.1";

// Image timeout variables
var timerID = null;
var defaultTimout = 3000;

// EPCW variables
var EPCWTimerID = null;
var defaultEPCWTimout = 10000;
var EPCWActive = false;
var activeEPCWAlert = false;
var nearEPCWID = null;
var farEPCWID = null;
var leftEPCWID = null;
var rightEPCWID = null;
var nearEPCWSeverity = 0;
var farEPCWSeverity = 0;
var leftEPCWSeverity = 0;
var rightEPCWSeverity = 0;
var activeEPCWImage = '../images/E-PCW/3-base-art.png';
var baseImageActiveEPCW = false; 
var lastEPCWID = 0;

var VTRWSeverity = 0;
var EEBLSeverity = 0;
var FCWSeverity = 0;

var EPCWInterval = null;

// Event listener from common.library 
addEventListener("newMessage", newMessageHandler, false);
addEventListener("disconnect", disconnectHandler, false);

// Receiver for unhandled messages from the common.library
function newMessageHandler(evt)
{
    console.log(evt.detail.message);
    var json_obj = JSON.parse(evt.detail.message);
    if (json_obj.header.subtype === "Application") { handleAppMessage(json_obj.payload); }
    else if (json_obj.header.subtype === "Basic") { handleAppMessage(json_obj.payload); }
//    else { console.log(timeStamp(evt.detail.time) + " Unknown Message: " + evt.detail.message);	}
}

// Handle disconnect
function disconnectHandler()
{
    console.log("Disconnected - Removing all images");
    clearEPCW();
}
/*
This function handles all of the Application:Application messages and displays the appropriate 
Image.
*/
function handleAppMessage(json_obj)
{


//    console.log(json_obj);
    if (json_obj.AppId == 2) // FCW
    {
        switch (json_obj.Severity) {
        case "0":
//            setTimedImage('../images/FCW/3-base-art.png', json_obj.CustomText);
            break;
        case "1":
            setTimedImage('../images/FCW/4-vehicle-ahead-alert-yellow.png', json_obj.CustomText);
            if (FCWSeverity < 1)
            {
                FCWSeverity = 1;
                playAudio(4);
            }            
            break;
        case "2":
            setTimedImage('../images/FCW/5-vehicle-ahead-warning-red.png', json_obj.CustomText);
            if (FCWSeverity < 2)
            {
                FCWSeverity = 2;
                playAudio(5);
            }            
            break;
        default:
            clearImage();
            break;
        }
    }
    else if (json_obj.AppId == 3) // EEBL
    {
        switch (json_obj.Severity) {
        case "0":
            setTimedImage('../images/FCW/3-base-art.png', json_obj.CustomText);
            if (EEBLSeverity < 1)
            {
                EEBLSeverity = 1;
            }            

            break;
        case "1":
        case "2":
            setTimedImage('../images/EEBL/4-eebl.png', json_obj.CustomText);
            if (EEBLSeverity < 2)
            {
                EEBLSeverity = 2;
                playAudio(8);
            }            
            break;
        default:
            clearImage();
            break;
        }
    }
    else if (json_obj.AppId == 6) // EPCW
    {
        stopTimer();
        restartEPCWTimeout();
        switch (json_obj.EventCode) {
        case "3": // Entered Area
            clearImage();
            EPCWActive = true;
            $("#ActiveImage").attr('src', '../images/E-PCW/2-system-active.png');
//            startTimer(defaultTimout);
            break;
        case "4": // Exited Area
            EPCWActive = false;
            stopEPCWTimer();
            clearEPCW();
            break;
        case "5": // Detected Area
            // No Action
            break;
        case "6": // Near side Pedestrian
            nearEPCWID = json_obj.EventID;
            lastEPCWID = json_obj.EventID;
            if (nearEPCWSeverity != json_obj.Severity)
            {
                nearEPCWSeverity = json_obj.Severity;
                displayEPCW();
                if (nearEPCWSeverity == 1)  playAudio(0);
                else if (nearEPCWSeverity == 2)  playAudio(1);
            }
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
            lastEPCWID = json_obj.EventID;
            if (rightEPCWSeverity != json_obj.Severity)
            {
                rightEPCWSeverity = json_obj.Severity;
                displayEPCW();
                if (rightEPCWSeverity == 1) playAudio(2); 
                else if (rightEPCWSeverity == 2) playAudio(3); 
            }
            break;
        case "20": // Pedestrian alert complete 
            if (lastEPCWID == json_obj.EventID) stopAudio();
            if (json_obj.EventID == nearEPCWID) 
            {
                nearEPCWID = null;
                nearEPCWSeverity = 0;
            }
            else if (json_obj.EventID == farEPCWID) 
            {
                farEPCWID = null;
                farEPCWSeverity = 0;
            }
            else if (json_obj.EventID == leftEPCWID) 
            {
                leftEPCWID = null;
                leftEPCWSeverity = 0;
            }
            else if (json_obj.EventID == rightEPCWID) 
            {
                rightEPCWID = null;
                rightEPCWSeverity = 0;
            }
            displayEPCW();
            break;
        case "21": // Detected Area
            if (!EPCWActive) 
            {
                clearImage();
                EPCWActive = true;
                $("#ActiveImage").attr('src', '../images/E-PCW/2-system-active.png');
            }
            // No Action
            break;
        }
    }
    else if (json_obj.AppId == 7) // EVTRW
    {
       switch (json_obj.Severity) {
        case "0":
            clearImage();
            break;
        case "1":
            setTimedImage('../images/VTRW/4-vehicle-passing-left-yellow-arrow.png', json_obj.CustomText);
            if (VTRWSeverity < 1)
            {
                VTRWSeverity = 1;
                playAudio(6);
            }
            break;
        case "2":
            setTimedImage('../images/VTRW/6-vehicle-crossing-path-red-arrow.png', json_obj.CustomText);
            if (VTRWSeverity < 2)
            {
                VTRWSeverity = 2;
                playAudio(7);
            }
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
 //   $("#bottomDiv").html(displayText);
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
    activeEPCWImage = '../images/E-PCW/2-system-active.png';
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
    clearImage();
}

function startTimer(duration)
{
    timerID = setTimeout(clearImage, duration);
}

function startEPCWTimer(duration)
{
    EPCWTimerID = setTimeout(EPCWTimeout, duration);
}

function restartEPCWTimeout()
{
    stopEPCWTimer();
    startEPCWTimer(defaultEPCWTimout);
}

function stopEPCWTimer()
{
    if (EPCWTimerID != null)
    {
        clearTimeout(EPCWTimerID);
        EPCWTimerID = null;
    }    
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
//    $("#bottomDiv").html("");
    VTRWSeverity = 0;
}

function EPCWTimeout()
{
    console.log("EPCW Timerout Clearing");
    clearEPCW();
}

// Sounds -----------------------

var audioFiles = [
    "../sounds/Caution_Peddestrian_Ahead.mp3",
    "../sounds/Warning_Pedestrian_Ahead.mp3",
    "../sounds/Caution_Peddestrian_On_Right.mp3",
    "../sounds/Warning_Peddestrian_On_Right.mp3",
    "../sounds/Caution_Vehicle_Ahead-Audio.mp3",
    "../sounds/Warning_Vehicle_Ahead-Track 2.mp3",
    "../sounds/Caution_Vehicle_On_Left.mp3",
    "../sounds/Caution_Vehicle_Crossing_Path.mp3",
    "../sounds/FCW.wav"
];
    
function preloadAudio(url) {
    var audio = new Audio();
    audio.src = url;
}

var player = document.getElementById('player');
function playAudio(index) {
    stopAudio();
    player.src = audioFiles[index];
    player.play();
}

function stopAudio()
{
    player.pause();
    player.currentTime = 0;
}

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

//    $(".soundFiles").trigger('load');
});