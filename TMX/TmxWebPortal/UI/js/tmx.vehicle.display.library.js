console.log("Including tmx.vehicle.display.library.js")
var deviceName = "TMX";
var customLibraryJsVersion = "0.0.1";

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
var activeDirections = [];
var activeEPCWImage = '../images/E-PCW/E-PCW_base.png';
var baseEPCWImage = '../images/E-PCW/E-PCW_base.png';
var playEPCWAudio = false;
var baseImageActiveEPCW = false; 
var lastEPCWID = 0;
var VTRWActive = false;
var CSWActive = false;

//var VTRWSeverity = 0;
//var CSWSeverity = 0;
//var EEBLSeverity = 0;
//var FCWSeverity = 0;

var defaultRCVWTimout = 10000;
var RCVWTimerID = null;
var RCVWSeverity = 0;
var RCVWActive =false;
var activeRCVWImage = '../images/RCVW/00-base-art.png';
var baseRCVWImage = '../images/RCVW/00-base-art.png';
var RCVWInterval = null;
var baseImageActiveRCVW = false; 
var activeRCVWAlert = false;
var RCVWActiveEvent = 0;
var RCVWCurrentEvent = 0;
var currentRCVWSeverity = 0;
var RCVWBlinkInterval = 300;
var RCVWErrorText = "";

var EPCWInterval = null;
var currentAudio = -1;
var nextAudio = -1;
var defaultSoundTimeout = 5000;
var SoundTimerID = null;

var his = createHisMessageObject();
var hisAudio = createHisMessageObject();
var hisID = 0;
var hisAudioID = 0;

var cloakMode = false;
var screenNotification = true;
var soundNotification = true;

var appList = [];



function createAppStatusMessageObject(appType)
{
    var appStatusMessage = new Object();
    appStatusMessage.appType = appType;
    appStatusMessage.active = false;
    appStatusMessage.activeEvent = null;
    appStatusMessage.severity = 0;
    // Display timer for image toggle
    appStatusMessage.displayTimer = 300;
    // Timeout interval
    appStatusMessage.interval = null;
    appStatusMessage.imageActive = null;
    appStatusMessage.imageInactive = null;
    appStatusMessage.imageDisplayed = false;

    return appStatusMessage;
}

function getAppObject(appType)
{
    var appObject = null;
    var tempList = appList;
    // Look for object
    for (var j = 0; j < tempList.length; j++) {
        var tempObj = tempList[j];
        if (appType.toUpperCase() == tempObj.appType.toUpperCase())
        {
            appObject = tempObj;
            break;
        }
    } 
    return appObject;
}

// Event listener from common.library 
addEventListener("newMessage", newMessageHandler, false);
addEventListener("disconnect", disconnectHandler, false);
addEventListener("connect", connectHandler, false);

// Handle connect
function connectHandler()
{
    console.log("Connected Setting Background Image");
     $("#ActiveImage").attr('src', '../images/Common/Background.png');
}

function createAppStatusMessageObject(appType)
{
    var appStatusMessage = new Object();
    appStatusMessage.appType = appType;
    appStatusMessage.active = false;
    appStatusMessage.activeEvent = null;
    appStatusMessage.severity = 0;
    appStatusMessage.displayTimer = 300;


    return appStatusMessage;
}

// Receiver for unhandled messages from the common.library
function newMessageHandler(evt)
{
//    console.log(evt.detail.message);
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

// Handle disconnect
function disconnectHandler()
{
    console.log("Disconnected - Removing all images");
    clearEPCW();
    clearRCVW();
     $("#ActiveImage").attr('src', '../images/Common/Splash_Screen.png');
}
function  handleConfig(json_obj) 
{

    console.log("Config Key:" + json_obj.key + " Value:" + json_obj.value);
    if (json_obj.key == "NotificationMode")
    {
        console.log("Notification Mode:" + json_obj.value)
        if (json_obj.value.toLowerCase() == "none")
        {
            screenNotification = false;
            soundNotification = false;
            $("#ActiveImage").addClass('hidden');
            $("#CloakImage").removeClass('hidden');
        }
        else if (json_obj.value.toLowerCase() == "audible")
        {
            screenNotification = false;
            soundNotification = true;
            $("#ActiveImage").addClass('hidden');
            $("#CloakImage").removeClass('hidden');
        }
        else if (json_obj.value.toLowerCase() == "visual")
        {
            screenNotification = true;
            soundNotification = false;
            $("#ActiveImage").removeClass('hidden');
            $("#CloakImage").addClass('hidden');
        }
        else if (json_obj.value.toLowerCase() == "all")
        {
            screenNotification = true;
            soundNotification = true;
            $("#ActiveImage").removeClass('hidden');
            $("#CloakImage").addClass('hidden');
        }
    }
    else { console.log("Did not process " + json_obj.key); }
}

/*
This function handles all of the Application:Application messages and displays the appropriate 
Image.
*/
function handleAppMessage(json_obj)
{
    // Set audio to no sound
    nextAudio = -1;
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
                populateHisImageMessageObject(2, 0, 1);
                FCWSeverity = 1;
                // TODO Change
                nextAudio = 4;
            }            
            break;
        case "2":
            setTimedImage('../images/FCW/5-vehicle-ahead-warning-red.png', json_obj.CustomText);
            if (FCWSeverity < 2)
            {
                populateHisImageMessageObject(2, 0, 2);
                FCWSeverity = 2;
                nextAudio = 8;
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
            populateHisImageMessageObject(3, 0, 0);
            break;
        case "1":
        case "2":
            setTimedImage('../images/EEBL/4-eebl.png', json_obj.CustomText);
            if (EEBLSeverity < 2)
            {
                populateHisImageMessageObject(2, 0, 2);
                EEBLSeverity = 2;
                nextAudio = 25;
            }            
            break;
        default:
            clearImage();
            break;
        }
    }
    else if (json_obj.AppId == 6) // EPCW
//    else if (json_obj.AppId == tmx.messages.appmessage.ApplicationTypes.EPCW.value) // EPCW
    {
        stopTimer();
        restartEPCWTimeout();
        playEPCWAudio = false;
        switch (json_obj.EventCode) {
        case "3": // Entered Area
            clearImage();
            EPCWActive = true;
            populateHisImageMessageObject(6, 3, 0);
            setImage('../images/E-PCW/E-PCW_base.png', "");
            currentAudio = -1;
            break;
        case "4": // Exited Area
            EPCWActive = false;
            populateHisImageMessageObject(6, 4, 0);
            stopEPCWTimer();
            clearEPCW();
            currentAudio = -1;
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
                if (nearEPCWSeverity != 0)  
                {
                    playEPCWAudio = true;
                }
                displayEPCW();
//                if (nearEPCWSeverity == 1)  nextAudio = 0; //  playAudio(0);
//                else if (nearEPCWSeverity == 2)  nextAudio = 1; //playAudio(1);
            }
            break;
        case "7": // Far side Pedestrian
            farEPCWID = json_obj.EventID;
            astEPCWID = json_obj.EventID;
            if (farEPCWSeverity != json_obj.Severity)
            {
                farEPCWSeverity = json_obj.Severity;
                if (farEPCWSeverity != 0)  playEPCWAudio = true;
                displayEPCW();
//                if (nearEPCWSeverity == 1)  nextAudio = 0; //  playAudio(0);
//                else if (nearEPCWSeverity == 2)  nextAudio = 1; //playAudio(1);
            }
            break;
        case "8": // Left side Pedestrian
            leftEPCWID = json_obj.EventID;
            astEPCWID = json_obj.EventID;
            if (leftEPCWSeverity != json_obj.Severity)
            {
                leftEPCWSeverity = json_obj.Severity;
                if (leftEPCWSeverity != 0)  playEPCWAudio = true;
                displayEPCW();
//                if (leftEPCWSeverity == 1)  nextAudio = 0; //  playAudio(0);
//                else if (leftEPCWSeverity == 2)  nextAudio = 1; //playAudio(1);
            }
            break;
        case "9": // Right side Pedestrian
            rightEPCWID = json_obj.EventID;
            lastEPCWID = json_obj.EventID;
            if (rightEPCWSeverity != json_obj.Severity)
            {
                rightEPCWSeverity = json_obj.Severity;
                if (rightEPCWSeverity != 0)  playEPCWAudio = true;
                displayEPCW();
//                if (rightEPCWSeverity == 1) nextAudio = 2; 
//                else if (rightEPCWSeverity == 2) nextAudio = 3; 
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
                populateHisImageMessageObject(6, 21, 0);
                setImage('../images/E-PCW/E-PCW_base.png', "");

            }
            // No Action
            break;
        }
    }
    else if (json_obj.AppId == 7) // EVTRW
    {
        switch (json_obj.EventCode) {
        case "1": // Entered Area
            clearImage();
            VTRWActive = true;
            populateHisImageMessageObject(7, 1, 0);
            setImage('../images/VTRW/VTRW_base.png', "");
            commonBackgroundImage = "../images/VTRW/VTRW_base.png";

            break;
        case "2": // Exited Area
        	VTRWActive = false;
        	VTRWSeverity = 0;
            clearImage();
            break;
        case "13":  // Ahead Left
        	if (VTRWActive && json_obj.Severity == "0") 
        	{
            	VTRWSeverity = 0;
                populateHisImageMessageObject(7, 13, 0);
                setImage('../images/VTRW/VTRW_base.png', "");
                break;        		
        	}
        case "17":	// Behind Left
        	if (VTRWActive && json_obj.Severity == "1")
        	{
        		setTimedImage('../images/VTRW/VTRW_caution_left_2.png', json_obj.CustomText);
        		if (VTRWSeverity < 1)
        		{
        			VTRWSeverity = 1;
                    nextAudio = 6;
        		}
        	}
            break;
        case "12": // Vehicle ahead
        	if (VTRWActive && json_obj.Severity == "2")
        	{
        		setTimedImage('../images/VTRW/VTRW_warning_left_1.png', json_obj.CustomText);
        		if (VTRWSeverity < 2)
        		{
        			VTRWSeverity = 2;
                    nextAudio = 7;
        		}
        	}
        	break;
       default:
    	   	break;
        }
    }
    else if (json_obj.AppId == 1) // CSW
    {
        console.log("CSW Event Code " + json_obj.EventCode);
        switch (json_obj.EventCode) {
        case "0": // Alert
            switch (json_obj.Severity) {
                case "0": // Alert
                    setTimedImage('../images/CSW/CSW_info.png', json_obj.CustomText);
                break;
                case "1": // Alert
                setTimedImage('../images/CSW/CSW_alert.png', json_obj.CustomText);
                if (CSWSeverity != "1")
                {
                    if (CSWSeverity < 1)
                    {
                        CSWSeverity = 1;
                        currentAudio = -1;
                        nextAudio = 25;
                    }
                }

                break;
                case "2": // Alert
                setTimedImage('../images/CSW/CSW_warning.png', json_obj.CustomText);
                if (CSWSeverity != "2")
                {
                    if (CSWSeverity < 2)
                    {
                        CSWSeverity = 2;
                        currentAudio = -1;
                        nextAudio = 8;
                    }
                }
                break;
            }
            break;
        case "1": // Entered Area
            if (!CSWActive)
            {
                clearImage();
                CSWActive = true;
                populateHisImageMessageObject(1, 0, 0);
                setImage('../images/CSW/CSW_base.png', "");
                commonBackgroundImage = "../images/CSW/CSW_base.png";
            }
            break;
        case "2": // Exited Area
            CSWActive = false;
            CSWSeverity = 0;
            commonBackgroundImage = "../images/Common/Background.png";
            clearImage();
            break;
       default:
            break;
        }
    }
    else if (json_obj.AppId == 10) // RCVW
    {
        // Check if RCVW object exists, if not create
        var tempObject = getAppObject("RCVW");
        if (tempObject == null)
        {

        }
        switch (json_obj.EventCode) {
        case "1": // Entered Area
            clearImage();
            RCVWActive = true;
            populateHisImageMessageObject(10, 1, 0);
            setImage('../images/RCVW/01-intersection-active.png', "");
            break;
        case "2": // Exited Area
            RCVWActive = false;
            RCVWSeverity = 0;
            clearRCVW();
            stopAudio();
            break;
        case "0":  // None
            if (RCVWActive) 
            {
                currentRCVWSeverity = RCVWSeverity;
                RCVWSeverity = Number(json_obj.Severity);
                displayRCVW();
            }
            break;
        case "23":  // Error
            RCVWCurrentEvent = 23;
            currentRCVWSeverity = RCVWSeverity;
            RCVWSeverity = Number(json_obj.Severity);
            RCVWErrorText = json_obj.CustomText;



            displayRCVW();
            if (currentRCVWSeverity < RCVWSeverity)
            {
                console.log("Playing sound");
                if (RCVWSeverity == 1) nextAudio = 9;
                if (RCVWSeverity == 2) nextAudio = 8;
            }     
            RCVWActiveEvent = RCVWCurrentEvent;
        break;
        case "24":  // Warning
            if (RCVWActive) 
            {
                RCVWCurrentEvent = 24;
                currentRCVWSeverity = RCVWSeverity;
                RCVWSeverity = Number(json_obj.Severity);
                displayRCVW();
                RCVWActiveEvent = RCVWCurrentEvent;
            }
            break;
        case "25":  // Danger
            if (RCVWActive) 
            {
                RCVWCurrentEvent = 25;
                currentRCVWSeverity = RCVWSeverity;
                RCVWSeverity = Number(json_obj.Severity);
                displayRCVW();
                RCVWActiveEvent = RCVWCurrentEvent;
            }
            break;            
       default:
            console.log("Invalid Event Code for RCVW - " + json_obj.EventCode);
            break;
        }
    }
    // If audio needs to be changed play audio
    if (nextAudio != -1) playAudio(nextAudio);
}


function setImage(image, displayText)
{

    if ($("#ActiveImage").attr('src') != image)
    {
        his.payload.HISPreState = $("#ActiveImage").attr('src');
        his.payload.HISPostState = image;
        sendCommand("TEST;" + JSON.stringify(his));       
       $("#ActiveImage").attr('src', image);
    }
    $("#bottomDiv").html(displayText);

}


function displayRCVW()
{
    var currentActiveRCVWImage = activeRCVWImage;
    var currentRCVWBlinkInterval = RCVWBlinkInterval;


//    if (RCVWActiveEvent != RCVWCurrentEvent)
//    {
        
        switch (RCVWSeverity) {
        case 0:
            stopRCVWDisplay();
            document.getElementById("bottomDiv").innerHTML = "";
            $("#ActiveImage").attr('src', '../images/RCVW/01-intersection-active.png');
            break;
        case 1:
            document.getElementById("bottomDiv").innerHTML = "";
            switch (RCVWCurrentEvent) {
                case 23:
                    stopRCVWDisplay();
                    document.getElementById("bottomDiv").innerHTML = RCVWErrorText;
                    activeRCVWImage = '../images/RCVW/02-system-error.png';
                    RCVWBlinkInterval = -1;
                    break;
                case 24:
                    activeRCVWImage = '../images/RCVW/04-rr-crossing-alert.png';
                    baseRCVWImage = '../images/RCVW/03-rr-crossing-base.png';
                    RCVWBlinkInterval = 500;
//                    restartRCVWDisplay();
                    break;
                case 25:
                    activeRCVWImage = '../images/RCVW/06-hri-warning-alert.png';
                    baseRCVWImage = '../images/RCVW/05-hri-warning-base.png';
                    RCVWBlinkInterval = 500;
//                    restartRCVWDisplay();
                    break;
            }
           
            if (!activeRCVWAlert)
            {
                activeRCVWAlert = true;
                console.log("Starting Display");
                if (RCVWBlinkInterval == -1)
                {
                    $("#ActiveImage").attr('src', activeRCVWImage);
                }
                else {
                    startRCVWDisplay();
                }
            }
            else if (currentRCVWBlinkInterval != RCVWBlinkInterval)
            {
                restartRCVWDisplay();
            }
            break;
        case 2:
            document.getElementById("bottomDiv").innerHTML = "";
            switch (RCVWCurrentEvent) {
                case 23:
                    stopRCVWDisplay();
                    RCVWBlinkInterval = -1;
                    document.getElementById("bottomDiv").innerHTML = RCVWErrorText;
                    activeRCVWImage = '../images/RCVW/02-system-error.png';
                    break;
                case 24:
                    activeRCVWImage = '../images/RCVW/04-rr-crossing-alert.png';
                    baseRCVWImage = '../images/RCVW/03-rr-crossing-base.png';
                    RCVWBlinkInterval = 200;
//                    restartRCVWDisplay();
                    break;
                case 25:
                    activeRCVWImage = '../images/RCVW/06-hri-warning-alert.png';
                    baseRCVWImage = '../images/RCVW/05-hri-warning-base.png';
                    RCVWBlinkInterval = 200;
//                    restartRCVWDisplay();                    
                    break;
            }
           
            if (!activeRCVWAlert)
            {
                activeRCVWAlert = true;
                console.log("Starting Display");
                if (RCVWBlinkInterval == -1)
                {
                    $("#ActiveImage").attr('src', activeRCVWImage);
                }
                else {
                    startRCVWDisplay();
                }
            }
            else if (currentRCVWBlinkInterval != RCVWBlinkInterval)
            {
                restartRCVWDisplay();
            }
            break;
        }
//    }
//    else
//    {

//    }

    if ((currentActiveRCVWImage != activeRCVWImage) || (currentRCVWSeverity != RCVWSeverity))
    {
        console.log("Playing sound");
        if (RCVWSeverity == 1) nextAudio = 9;
        if (RCVWSeverity == 2) nextAudio = 8;

    }

    if (currentActiveRCVWImage != activeRCVWImage)
    {
            his.payload.HISPreState = currentActiveRCVWImage;
            his.payload.HISPostState = activeRCVWImage;
            sendCommand("TEST;" + JSON.stringify(his));
    }


}


function displayEPCW()
{
    var audioPlayIndex = -1;
    var currentActiveEPCWImage = activeEPCWImage;
    activeDirections = [];
    if ((nearEPCWID!=null) || (farEPCWID!=null) || (leftEPCWID!=null) || (rightEPCWID!=null)) {
        if (nearEPCWID != null) {
            if (nearEPCWSeverity >= 1 && nearEPCWSeverity <= 2) {
                activeDirections.push("NearDirectionImage");
                if (nearEPCWSeverity == 1) {
                    if (baseEPCWImage.indexOf("warning") != -1) {
                        baseEPCWImage = '../images/E-PCW/E-PCW_base.png';
                    }
                    $("#NearDirectionImage").attr('src', "../images/E-PCW/E-PCW_caution_near.png");
                } else if (nearEPCWSeverity == 2) {
                    baseEPCWImage = '../images/E-PCW/E-PCW_base.png';
                    $("#NearDirectionImage").attr('src', "../images/E-PCW/E-PCW_warning_near.png");
                }
            }
        }
        if (rightEPCWID != null) {
            if (rightEPCWSeverity >= 1 && rightEPCWSeverity <= 2) {
                activeDirections.push("RightDirectionImage");
                if (rightEPCWSeverity == 1) {
                    if (baseEPCWImage.indexOf("warning") != -1) {
                        baseEPCWImage = '../images/E-PCW/E-PCW_base.png';
                    }
                    $("#RightDirectionImage").attr('src', "../images/E-PCW/E-PCW_caution_right.png");
                } else if (rightEPCWSeverity == 2) {
                    baseEPCWImage = '../images/E-PCW/E-PCW_base.png';
                    $("#RightDirectionImage").attr('src', "../images/E-PCW/E-PCW_warning_right.png");
                }
            }
        }
        if (farEPCWID != null) {
            if (farEPCWSeverity >= 1 && farEPCWSeverity <= 2) {
                activeDirections.push("FarDirectionImage");
                if (farEPCWSeverity == 1) {
                    if (baseEPCWImage.indexOf("warning") != -1) {
                        baseEPCWImage = '../images/E-PCW/E-PCW_base.png';
                    }
                    $("#FarDirectionImage").attr('src', "../images/E-PCW/E-PCW_caution_far.png");
                } else if (farEPCWSeverity == 2) {
                    baseEPCWImage = '../images/E-PCW/E-PCW_base.png';
                    $("#FarDirectionImage").attr('src', "../images/E-PCW/E-PCW_warning_far.png");
                }
            }
        }
        if (leftEPCWID != null) {
            if (leftEPCWSeverity >= 1 && leftEPCWSeverity <= 2) {
                activeDirections.push("LeftDirectionImage");
                if (leftEPCWSeverity == 1) {
                    if (baseEPCWImage.indexOf("warning") != -1) {
                        baseEPCWImage = '../images/E-PCW/E-PCW_base.png';
                    }
                    $("#LeftDirectionImage").attr('src', "../images/E-PCW/E-PCW_caution_left.png");
                } else if (leftEPCWSeverity == 2) {
                    baseEPCWImage = '../images/E-PCW/E-PCW_base.png';
                    $("#LeftDirectionImage").attr('src', "../images/E-PCW/E-PCW_warning_left.png");
                }
            }
        }

        if ((nearEPCWID!=null) && ((rightEPCWID!=null) || (leftEPCWID!=null) || (farEPCWID!=null))) {
            if (rightEPCWID!=null) {
                if ((nearEPCWSeverity == 1) && (rightEPCWSeverity == 1)) {
                    audioPlayIndex = 15;
                }
                else {
                    if ((nearEPCWSeverity == 1) && (rightEPCWSeverity == 2)) audioPlayIndex = 23;           // No Sound File
                    else if ((nearEPCWSeverity == 2) && (rightEPCWSeverity == 1)) audioPlayIndex = 19;      // no Sound File
                    else if ((nearEPCWSeverity == 2) && (rightEPCWSeverity == 2)) audioPlayIndex = 22;      
                }
            }
            else if (leftEPCWID!=null){
                if ((nearEPCWSeverity == 1) && (leftEPCWID == 1)) {
                    audioPlayIndex = 14;
                }
                else {
                    if ((nearEPCWSeverity == 1) && (leftEPCWID == 2)) audioPlayIndex = 18;              // No Sound File
                    else if ((nearEPCWSeverity == 2) && (leftEPCWID == 1)) audioPlayIndex = 19;         // No Sound File
                    else if ((nearEPCWSeverity == 2) && (leftEPCWID == 2)) audioPlayIndex = 21;         // No Sound File
                }
            }
            else if (farEPCWID!=null) {
                if ((nearEPCWSeverity == 1) && (farEPCWID == 1)) {
                    audioPlayIndex = 13;
                }
                else {
                    if ((nearEPCWSeverity == 1) && (farEPCWID == 2)) audioPlayIndex = 17;               // No sound File
                    else if ((nearEPCWSeverity == 2) && (farEPCWID == 1)) audioPlayIndex = 20;  
                    else if ((nearEPCWSeverity == 2) && (farEPCWID == 2)) audioPlayIndex = 22;          
                }
            }
        }
        else if (nearEPCWID!=null) {
            if (nearEPCWSeverity == 1) {
                audioPlayIndex = 12;
            }
            else if (nearEPCWSeverity == 2) {
                audioPlayIndex = 19;
            }
        }
        else if (rightEPCWID!=null) {
            if (rightEPCWSeverity == 1) {
                audioPlayIndex = 16;
            }
            else if (rightEPCWSeverity == 2) {      
                audioPlayIndex = 23;
            }
        }
        else if (farEPCWID!=null) {
            if (farEPCWSeverity == 1) {     
                audioPlayIndex = 10;
            }
            else if (farEPCWSeverity == 2) {
                audioPlayIndex = 17;
            }
        }
        else if (leftEPCWID!=null) {
            if (leftEPCWSeverity == 1) {
                audioPlayIndex = 11;
            }
            else if (leftEPCWSeverity == 2) {
                audioPlayIndex = 18;
            }
        }

        console.log("Play audio:" + playEPCWAudio + " index:" + audioPlayIndex);
        if ((playEPCWAudio) && (audioPlayIndex != -1)) {
            playAudio(audioPlayIndex);
        }

        if (!activeEPCWAlert) {
            activeEPCWAlert = true;
            startEPCWDisplay();
        }
        if (currentActiveEPCWImage != activeEPCWImage) {
        	his.payload.HISPreState = currentActiveEPCWImage;
            his.payload.HISPostState = activeEPCWImage;
            sendCommand("TEST;" + JSON.stringify(his));
        }

        $("#ActiveImage").attr('src', baseEPCWImage);
    }
    else {
        if (activeEPCWAlert == true) {
            his.payload.HISPreState = currentActiveEPCWImage;
            his.payload.HISPostState = '../images/E-PCW/E-PCW_base.png';
            sendCommand("TEST;" + JSON.stringify(his));
        }
        stopEPCWDisplay();
        currentAudio = -1;
        activeEPCWAlert = false;
    }
}

/*
function displayStatus()
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


}
*/

function toggleEPCWImage()
{
    $("[id$=DirectionImage]").addClass("hidden");

    for (var i = 0; i < activeDirections.length; i++) {
        var direction = $("#" + activeDirections[i]);
        if (baseImageActiveEPCW) {
           direction.removeClass("hidden");
        } else {
           direction.addClass("hidden");
        }
    }
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
    activeEPCWImage = '../images/E-PCW/E-PCW_base.png';
    $("#ActiveImage").attr('src', activeEPCWImage);
    $("#LeftDirectionImage").addClass("hidden");
    $("#RightDirectionImage").addClass("hidden");
    $("#NearDirectionImage").addClass("hidden");
    $("#FarDirectionImage").addClass("hidden");
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
    $("#LeftDirectionImage").addClass("hidden");
    $("#RightDirectionImage").addClass("hidden");
    $("#NearDirectionImage").addClass("hidden");
    $("#FarDirectionImage").addClass("hidden");
    his.payload.HISPreState = activeEPCWImage;
    his.payload.HISPostState = '../images/Common/Background.png';
    sendCommand("TEST;" + JSON.stringify(his));
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
function EPCWTimeout()
{
    console.log("EPCW Timerout Clearing");
    clearEPCW();
}

function startSoundTimer(duration)
{
    SoundTimerID = setTimeout(SoundTimeout, duration);
}

function restartSoundTimeout()
{
    stopSoundTimer();
    startSoundTimer(defaultSoundTimeout);
}

function stopSoundTimer()
{
    if (SoundTimerID != null)
    {
        clearTimeout(SoundTimerID);
        SoundTimerID = null;
    }    
}
function SoundTimeout()
{
    console.log("Sound Timerout Clearing");
    currentAudio = -1;
}

// ------- EPCW ----------------------------------------------------------
// ------- RCVW ----------------------------------------------------------
function toggleRCVWImage()
{
    if (baseImageActiveRCVW) $("#ActiveImage").attr('src', activeRCVWImage);
    else 
    {
        $("#ActiveImage").attr('src', baseRCVWImage);
    }
    baseImageActiveRCVW = !baseImageActiveRCVW;
}

function startRCVWDisplay()
{
    baseImageActiveRCVW = true;
    toggleRCVWImage();
    RCVWInterval = setInterval(toggleRCVWImage, RCVWBlinkInterval);
}

function restartRCVWDisplay()
{
    clearInterval(RCVWInterval);
    RCVWInterval = null;
    RCVWInterval = setInterval(toggleRCVWImage, RCVWBlinkInterval);
}

function stopRCVWDisplay()
{
    stopRCVWWTimer();
    clearInterval(RCVWInterval);
    RCVWInterval = null;
    activeRCVWImage = '../images/RCVW/01-intersection-active.png';
    activeRCVWAlert = false;
    $("#ActiveImage").attr('src', activeRCVWImage);
}

function clearRCVW()
{
    stopRCVWDisplay();
    clearImage();
    his.payload.HISPreState = activeRCVWImage;
    his.payload.HISPostState = '../images/Common/Background.png';
    sendCommand("TEST;" + JSON.stringify(his));
}

function startRCVWTimer(duration)
{
    RCVWTimerID = setTimeout(RCVWTimeout, duration);
}

function restartRCVWTimeout()
{
    stopRCVWTimer();
    startRCVWTimer(defaultRCVWTimout);
}

function stopRCVWWTimer()
{
    if (RCVWTimerID != null)
    {
        clearTimeout(RCVWTimerID);
        RCVWTimerID = null;
    }    
}
function RCVWTimeout()
{
    console.log("RCVW Timerout Clearing");
    clearRCVW();
}

// ------- RCVW ----------------------------------------------------------



// Sounds -----------------------

var audioFiles = [
    "../sounds/Caution_Peddestrian_Ahead.mp3",      // 0
    "../sounds/Warning_Pedestrian_Ahead.mp3",
    "../sounds/Caution_Peddestrian_On_Right.mp3",
    "../sounds/Warning_Peddestrian_On_Right.mp3",
    "../sounds/Caution_Vehicle_Ahead-Audio.mp3",
    "../sounds/Warning_Vehicle_Ahead-Track 2.mp3",
    "../sounds/Caution_Vehicle_On_Left.mp3",
    "../sounds/Caution_Vehicle_Crossing_Path.mp3",
    "../sounds/FCW.wav",
    "../sounds/Train_Honk.wav",                     // 9
// -----------------------------------------------------    
    "../sounds/E-PCW/E-PCW_caution_far.mp3",        // 10
    "../sounds/E-PCW/E-PCW_caution_left.mp3",
    "../sounds/E-PCW/E-PCW_caution_near.mp3",
    "../sounds/E-PCW/E-PCW_caution_near_caution_far.mp3",
    "../sounds/E-PCW/E-PCW_caution_near_caution_left.mp3",
    "../sounds/E-PCW/E-PCW_caution_near_caution_right.mp3", //15
    "../sounds/E-PCW/E-PCW_caution_right.mp3",
    "../sounds/E-PCW/E-PCW_warning_far.mp3",
    "../sounds/E-PCW/E-PCW_warning_left.mp3",
    "../sounds/E-PCW/E-PCW_warning_near.mp3",

    "../sounds/E-PCW/E-PCW_warning_near_caution_far.mp3",       // 20
    "../sounds/E-PCW/E-PCW_warning_near_warning_left.mp3",
    "../sounds/E-PCW/E-PCW_warning_near_warning_right.mp3",
    "../sounds/E-PCW/E-PCW_warning_right.mp3",
    "../sounds/E-PCW/E-PCW_warning_near_warning_far.mp3",
    "../sounds/EEBL.wav",
    "../sounds/EEBL_warning.wav"
];
    
function preloadAudio(url) {
    var audio = new Audio();
    audio.src = url;
}

var player = document.getElementById('player');
function playAudio(index) {
    console.log("Playing Audio index:" + index + " Current Audio:" + currentAudio);
    if (soundNotification)
    {
        if (index != currentAudio)
        {
            currentAudio = index;
            stopAudio();
            player.src = audioFiles[index];
            player.play();

            restartSoundTimeout(defaultSoundTimeout);
            hisAudio.payload.HISPostState = audioFiles[index];
            sendCommand("TEST;" + JSON.stringify(hisAudio));
        }
    }
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
