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
var TSPWActive = false;
var CSWActive = false;

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
var nextAudio = -1;

var cloakMode = false;

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
     $("#ActiveImage").attr('src', commonBackgroundImage);
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

//    console.log("Message: " + evt.detail.message);
    var json_obj = JSON.parse(evt.detail.message);
    if (json_obj.header.subtype === "Application") { handleAppMessage(json_obj.payload); }
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
    else if (json_obj.header.subtype === "ModuleStatus") 
    {
        handleStatusMessage(json_obj.payload);
    }
//    else { console.log(timeStamp(evt.detail.time) + " Unknown Message: " + evt.detail.message);	}
}

// Handle disconnect
function disconnectHandler()
{
    console.log("Disconnected - Removing all images");
    clearEPCW();
    stopEPCWTimer();
    clearRCVW();
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
    // Do nothing if cloak, maintenance or degraded mode active
    if ((!screenNotification && !soundNotification) || (MaintenanceMode) || (OperationalModeDegraded)) return;

    // Set audio to no sound
    nextAudio = -1;
    if (json_obj.AppId == 2) // FCW
    {
        switch (json_obj.Severity) {
        case "0":
//            setTimedImage('../images/FCW/3-base-art.png', json_obj.CustomText);
            break;
        case "1":
            if (screenNotification) {
                setTimedImage('../images/FCW/4-vehicle-ahead-alert-yellow.png', json_obj.CustomText);
            }
            if (FCWSeverity < 1)
            {
                if (screenNotification) {
                    populateHisImageMessageObject(2, json_obj.Id, json_obj.Severity);
                }
                FCWSeverity = 1;
                // TODO Change
                nextAudio = 4;
            }            
            break;
        case "2":
            if (screenNotification) {
                setTimedImage('../images/FCW/5-vehicle-ahead-warning-red.png', json_obj.CustomText);
            }
            if (FCWSeverity < 2)
            {
                if (screenNotification) {
                    populateHisImageMessageObject(2, json_obj.Id, json_obj.Severity);
                }
                FCWSeverity = 2;
                nextAudio = 8;
            }            
            break;
            default:
                if (screenNotification) {
                    clearImage();
                }
            break;
        }
    }
    else if (json_obj.AppId == 3) // EEBL
    {
        switch (json_obj.Severity) {
        case "0":
            if (screenNotification) {
                setTimedImage('../images/FCW/3-base-art.png', json_obj.CustomText);
                populateHisImageMessageObject(3, json_obj.Id, json_obj.Severity);
            }
            break;
        case "1":
        case "2":
            if (screenNotification) {
                setTimedImage('../images/EEBL/4-eebl.png', json_obj.CustomText);
            }
            if (EEBLSeverity < 2)
            {
                if (screenNotification) {
                    populateHisImageMessageObject(2, json_obj.Id, json_obj.Severity);
                }
                EEBLSeverity = 2;
                nextAudio = 25;
            }            
            break;
        default:
            if (screenNotification) {
                clearImage();
            }
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
            EPCWActive = true;
            if (screenNotification) {
                clearImage();
                populateHisImageMessageObject(6, json_obj.Id, json_obj.Severity);
                setImage('../images/E-PCW/E-PCW_base.png', "");
            }
//            console.log("Entered EPCW currentAudio=-1");
            currentAudio = -1;
            break;
        case "4": // Exited Area
            EPCWActive = false;
            stopEPCWTimer();
            if (screenNotification) {
                populateHisImageMessageObject(6, json_obj.Id, json_obj.Severity);
            }
            clearEPCW();
//            console.log("Exited EPCW currentAudio=-1");
            currentAudio = -1;
            break;
        case "5": // Detected Area
            // No Action
            break;
        case "6": // Near side Pedestrian
            if (screenNotification) {
                populateHisImageMessageObject(6, json_obj.Id, json_obj.Severity);
            }
            nearEPCWID = json_obj.EventID;
            lastEPCWID = json_obj.EventID;
            if (nearEPCWSeverity != json_obj.Severity)
            {
                nearEPCWSeverity = json_obj.Severity;
                if (nearEPCWSeverity != 0)  
                {
                    playEPCWAudio = true;
                }
                if (screenNotification) {
                    displayEPCW();
                }
            }
            break;
        case "7": // Far side Pedestrian
            if (screenNotification) {
                populateHisImageMessageObject(6, json_obj.Id, json_obj.Severity);
            }
            farEPCWID = json_obj.EventID;
            lastEPCWID = json_obj.EventID;
            if (farEPCWSeverity != json_obj.Severity)
            {
                farEPCWSeverity = json_obj.Severity;
                if (farEPCWSeverity != 0) playEPCWAudio = true;
                if (screenNotification) {
                    displayEPCW();
                }
            }
            break;
        case "8": // Left side Pedestrian
            if (screenNotification) {
                populateHisImageMessageObject(6, json_obj.Id, json_obj.Severity);
            }
            leftEPCWID = json_obj.EventID;
            lastEPCWID = json_obj.EventID;
            if (leftEPCWSeverity != json_obj.Severity)
            {
                leftEPCWSeverity = json_obj.Severity;
                if (leftEPCWSeverity != 0)  playEPCWAudio = true;
                if (screenNotification) {
                    displayEPCW();
                }
            }
            break;
        case "9": // Right side Pedestrian
            if (screenNotification) {
                populateHisImageMessageObject(6, json_obj.Id, json_obj.Severity);
            }
            rightEPCWID = json_obj.EventID;
            lastEPCWID = json_obj.EventID;
            if (rightEPCWSeverity != json_obj.Severity)
            {
                rightEPCWSeverity = json_obj.Severity;
                if (rightEPCWSeverity != 0) playEPCWAudio = true;
                if (screenNotification) {
                    displayEPCW();
                }
            }
            break;
        case "20": // Pedestrian alert complete 
            if (screenNotification) {
                populateHisImageMessageObject(6, json_obj.Id, json_obj.Severity);
            }
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
            if (screenNotification) {
                displayEPCW();
            }
            break;
        case "21": // Detected Area
            if (!EPCWActive) 
            {
                EPCWActive = true;
                if (screenNotification) {
                    clearImage();
                    populateHisImageMessageObject(6, json_obj.Id, json_obj.Severity);
                    setImage('../images/E-PCW/E-PCW_base.png', "");
                }
            }
            // No Action
            break;
        }
    }
    else if (json_obj.AppId == 7) // EVTRW
    {
        switch (json_obj.EventCode) {
        case "1": // Entered Area
            VTRWActive = true;
            if (screenNotification) {
                clearImage();
                populateHisImageMessageObject(7, json_obj.Id, json_obj.Severity);
                commonBackgroundImage = "../images/VTRW/VTRW_base.png";
                clearImage();
            }
 //           setImage('../images/VTRW/VTRW_base.png', "");
            break;
        case "2": // Exited Area
        	VTRWActive = false;
        	VTRWSeverity = 0;
        	if (screenNotification) {
        	    populateHisImageMessageObject(7, json_obj.Id, json_obj.Severity);
        	    commonBackgroundImage = "../images/Common/Background.png";
        	    clearImage();
        	}
            break;
        case "13":  // Ahead Left
        	if (VTRWActive && json_obj.Severity == "0") 
        	{
        	    VTRWSeverity = 0;
        	    if (screenNotification) {
        	        populateHisImageMessageObject(7, json_obj.Id, json_obj.Severity);
        	        setImage('../images/VTRW/VTRW_base.png', "");
        	    }
                break;        		
        	}
        case "17":	// Behind Left
        	if (VTRWActive && json_obj.Severity == "1")
        	{
        	    if (screenNotification) {
        	        populateHisImageMessageObject(7, json_obj.Id, json_obj.Severity);
        	        setTimedImage('../images/VTRW/VTRW_caution_left_2.png', json_obj.CustomText);
        	    }
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
        	    if (screenNotification) {
        	        populateHisImageMessageObject(7, json_obj.Id, json_obj.Severity);
        	        setTimedImage('../images/VTRW/VTRW_warning_left_1.png', json_obj.CustomText);
        	    }
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
        // TODO: Need to add a display function
        console.log("CSW Event Code " + json_obj.EventCode);
        switch (json_obj.EventCode) {
        case "0": // Alert
            switch (json_obj.Severity) {
            case "0": // Alert
                if (screenNotification) {
                    setTimedImage('../images/CSW/CSW_info.png', json_obj.CustomText);
                }
                break;
            case "1": // Alert
                if (screenNotification) {
                    setTimedImage('../images/CSW/CSW_alert.png', json_obj.CustomText);
                }
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
                if (screenNotification) {
                    setTimedImage('../images/CSW/CSW_warning.png', json_obj.CustomText);
                }
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
                CSWActive = true;
                if (screenNotification) {
                    clearImage();
                    populateHisImageMessageObject(1, json_obj.Id, json_obj.Severity);
                    setImage('../images/CSW/CSW_base.png', "");
                }
            }
            break;
        case "2": // Exited Area
            CSWActive = false;
            CSWSeverity = 0;
            if (screenNotification) {
                clearImage();
            }
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
            RCVWActive = true;
            if (screenNotification) {
                clearImage();
                populateHisImageMessageObject(10, json_obj.Id, json_obj.Severity);
                setImage('../images/RCVW/01-intersection-active.png', "");
            }
            break;
        case "2": // Exited Area
            RCVWActive = false;
            RCVWSeverity = 0;
            clearRCVW();
            stopAudio();
            break;
        case "0":  // None
            // Do nothing
            break;
        case "23":  // Error
            RCVWCurrentEvent = 23;
            currentRCVWSeverity = RCVWSeverity;
            RCVWSeverity = Number(json_obj.Severity);
            RCVWErrorText = json_obj.CustomText;
            
            if (screenNotification) {
                displayRCVW();
            }
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
                if (screenNotification) {
                    displayRCVW();
                }
                RCVWActiveEvent = RCVWCurrentEvent;
            }
            break;
        case "25":  // Danger
            if (RCVWActive) 
            {
                RCVWCurrentEvent = 25;
                currentRCVWSeverity = RCVWSeverity;
                RCVWSeverity = Number(json_obj.Severity);
                if (screenNotification) {
                    displayRCVW();
                }
                RCVWActiveEvent = RCVWCurrentEvent;
            }
            break;            
       default:
            console.log("Invalid Event Code for RCVW - " + json_obj.EventCode);
            break;
        }
    }
    else if (json_obj.AppId == 8 || // TSPW
             json_obj.AppId == 13)  // TSPWPOV
    {
    	TspwHandleAlert(json_obj);
    }
    
    // If audio needs to be changed play audio
    if (nextAudio != -1 && soundNotification) playAudio(nextAudio, audioFiles);
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
        his.payload.Id = generateUUID();
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
                    $("#NearDirectionImage").attr('src', "../images/E-PCW/E-PCW_caution_near.png");
                    activeEPCWImage = "../images/E-PCW/E-PCW_caution_near.png"
                } else if (nearEPCWSeverity == 2) {
                    $("#NearDirectionImage").attr('src', "../images/E-PCW/E-PCW_warning_near.png");
                    activeEPCWImage = "../images/E-PCW/E-PCW_warning_near.png"
                }
            }
        }
        if (rightEPCWID != null) {
            if (rightEPCWSeverity >= 1 && rightEPCWSeverity <= 2) {
                activeDirections.push("RightDirectionImage");
                if (rightEPCWSeverity == 1) {
                    $("#RightDirectionImage").attr('src', "../images/E-PCW/E-PCW_caution_right.png");
                    activeEPCWImage = "../images/E-PCW/E-PCW_caution_right.png"
                } else if (rightEPCWSeverity == 2) {
                    $("#RightDirectionImage").attr('src', "../images/E-PCW/E-PCW_warning_right.png");
                    activeEPCWImage = "../images/E-PCW/E-PCW_warning_right.png"
                }
            }
        }
        if (farEPCWID != null) {
            if (farEPCWSeverity >= 1 && farEPCWSeverity <= 2) {
                activeDirections.push("FarDirectionImage");
                if (farEPCWSeverity == 1) {
                    $("#FarDirectionImage").attr('src', "../images/E-PCW/E-PCW_caution_far.png");
                    activeEPCWImage = "../images/E-PCW/E-PCW_caution_far.png"
                } else if (farEPCWSeverity == 2) {
                    $("#FarDirectionImage").attr('src', "../images/E-PCW/E-PCW_warning_far.png");
                    activeEPCWImage = "../images/E-PCW/E-PCW_warning_far.png"
                }
            }
        }
        if (leftEPCWID != null) {
            if (leftEPCWSeverity >= 1 && leftEPCWSeverity <= 2) {
                activeDirections.push("LeftDirectionImage");
                if (leftEPCWSeverity == 1) {
                    $("#LeftDirectionImage").attr('src', "../images/E-PCW/E-PCW_caution_left.png");
                    activeEPCWImage = "../images/E-PCW/E-PCW_caution_left.png"
                } else if (leftEPCWSeverity == 2) {
                    $("#LeftDirectionImage").attr('src', "../images/E-PCW/E-PCW_warning_left.png");
                    activeEPCWImage = "../images/E-PCW/E-PCW_warning_left.png"
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

//        console.log("Play audio:" + playEPCWAudio + " index:" + audioPlayIndex);
        if ((playEPCWAudio) && (audioPlayIndex != -1)) {
            playAudio(audioPlayIndex, audioFiles);
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

function toggleEPCWImage()
{
    $("[id$=DirectionImage]").addClass("hidden");

    if ((screenNotification) && (!MaintenanceMode) && (!OperationalModeDegraded))
    {
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
    if (screenNotification) {
        $("#ActiveImage").attr('src', activeEPCWImage);
    }
    $("[id$=DirectionImage]").addClass("hidden");
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
    if (screenNotification) {
        clearImage();
        his.payload.HISPreState = activeEPCWImage;
        his.payload.HISPostState = commonBackgroundImage;
        sendCommand("TEST;" + JSON.stringify(his));
    }
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


// ------- EPCW ----------------------------------------------------------
// ------- RCVW ----------------------------------------------------------
function toggleRCVWImage()
{

    if ((!screenNotification && !soundNotification) && (!MaintenanceMode) && (!OperationalModeDegraded))
    {
        if (baseImageActiveRCVW) $("#ActiveImage").attr('src', activeRCVWImage);
        else 
        {
            $("#ActiveImage").attr('src', baseRCVWImage);
        }
        baseImageActiveRCVW = !baseImageActiveRCVW;
    }
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
    if (screenNotification) {
        $("#ActiveImage").attr('src', activeRCVWImage);
    }
}

function clearRCVW()
{
    stopRCVWDisplay();
    if (screenNotification) {
        clearImage();
        his.payload.HISPreState = activeRCVWImage;
        his.payload.HISPostState = commonBackgroundImage;
        sendCommand("TEST;" + JSON.stringify(his));
    }
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

// ------- TSPW Start ----------------------------------------------------
var tspwPedestrianTimout_ms = 2000;
var tspwValidSeverity = {"1": true, "2": true};

var tspwZoneInfoTable = {
	"10": { 		// event Code
		zoneName: "OnCurbsidePed",
		timerId: null,
		currentAudIdx: -1,
		files: {
		"1":{   	// severity
				imgFile: ["../images/TVO/05-inform-zone2.png", ""],
				audFileIdx: [27, -1]
			},
		"2":{
				imgFile: ["../images/TVO/05-inform-zone2.png", ""], // no warning image present
				audFileIdx: [27, -1]
			}}},		
	"32": {
		zoneName: "InRoadwayFwdCenterPed",
		timerId: null,
		currentAudIdx: -1,
		files: {
		"1":{
				imgFile: ["../images/TVO/03-inform-zone5.png", "../images/POV/02-inform.png"],
				audFileIdx: [28, 28]
			},
		"2":{
				imgFile: ["../images/TVO/06-warning-zone5.png", "../images/POV/04-warning-road.png"],
				audFileIdx: [29, 29]
			}}},
	"33": {
		zoneName: "InRoadwayFwdCurbPed",
		timerId: null,
		currentAudIdx: -1,
		files: {
		"1":{
				imgFile: ["../images/TVO/07-warning-zone3.png", ""], // no inform image present
				audFileIdx: [29, -1] // using warning audio
			},
		"2":{
				imgFile: ["../images/TVO/07-warning-zone3.png", ""],
				audFileIdx: [29, -1]
			}}},
	"34": {
		zoneName: "InRoadwayRearCenterPed",
		timerId: null,
		currentAudIdx: -1,
		files: {
		"1":{
				imgFile: ["../images/TVO/04-inform-zone6.png", "../images/POV/03-warning.png"],
				audFileIdx: [28, 29]
			},
		"2":{
				imgFile: ["../images/TVO/09-warning-zone6.png", "../images/POV/03-warning.png"],
				audFileIdx: [29, 29]
			}}},
	"35": {
		zoneName: "InRoadwayRearCurbPed",
		timerId: null,
		currentAudIdx: -1,
		files: {
		"1":{
				imgFile: ["../images/TVO/08-warning-zone4.png", "../images/POV/03-warning.png"],  // no inform image present
				audFileIdx: [29, 29]// using warning audio
			},
		"2":{
				imgFile: ["../images/TVO/08-warning-zone4.png", "../images/POV/03-warning.png"],
				audFileIdx: [29, 29]
			}}}
};

function TspwHandleAlert(json_obj)
{
    switch (json_obj.EventCode) {
    case "3": // Entered Area
        clearImage();
        populateHisImageMessageObject(json_obj.AppId, json_obj.Id, json_obj.Severity);
        TspwDisplayBase(json_obj);
        TspwClearAllZones();
        console.log("Entered TSPW");
        break;
    case "4": // Exited Area
        populateHisImageMessageObject(json_obj.AppId, json_obj.Id, json_obj.Severity);
    	TspwClearAllZones();
        clearImage();
        console.log("Exited TSPW");
        break;
    case "5": // Detected Area
        // No Action
        break;
    case "20": // EventComplete
    case "21": // InArea
    	// No pedestrian alerts while on the map.
    	TspwClearAllZones();
        console.log("Cleared All TSPW Zones");
    	break;
    case "10": // OnCurbsidePed
    case "32": // InRoadwayFwdCenterPed
    case "33": // InRoadwayFwdCurbPed
    case "34": // InRoadwayRearCenterPed
    case "35": // InRoadwayRearCurbPed
        TspwDisplayZone(json_obj);
    	break;
	}
}

function TspwDisplayBase(json_obj)
{
	var isPov = (json_obj.AppId == "13");
	
	if (isPov)
    	setImage('../images/POV/01-base.png', json_obj.CustomText);
	else
    	setImage('../images/TVO/02-base.png', json_obj.CustomText);
}

function TspwDisplayZone(json_obj)
{
	var evCode = json_obj.EventCode;
	var sev = json_obj.Severity;
	var zoneObj = tspwZoneInfoTable[evCode];
    var povIdx = (json_obj.AppId == "8")? 0 : 1;
//	var povIdx = json_obj.EventID;
	
	if (sev in tspwValidSeverity) {
		var audFileIdx = zoneObj.files[sev].audFileIdx[povIdx];
		if (audFileIdx > TspwGetAudioSeverity()) {
			// Play new higher severity audio
			playAudio(audFileIdx, audioFiles);
		}
		zoneObj.currentAudIdx = audFileIdx;
		
		var zone = $("#" + zoneObj.zoneName + "ZoneImage");
		
		var imgFile = zoneObj.files[sev].imgFile[povIdx];

		// Make sure Base image is set
        TspwDisplayBase(json_obj);
		
		if (imgFile) {
		    zone.attr('src', imgFile);
			zone.removeClass("hidden");
		    
		    // Reset zone display timer.
		    clearTimeout(zoneObj.timerId);
		    zoneObj.timerId = setTimeout(TspwClearZone, tspwPedestrianTimout_ms, evCode);
		    console.log("Set Image for : " + zoneObj.zoneName);
		}
		
	    // TODO: remove below - test only
//	    TspwDisplayAllZones("2");
   	}
	else {
	    console.log("Received invalid alert severity " + sev + " for TSPW display event: " + evCode);
	}
}

function TspwClearZone(evCode)
{
	var zoneObj = tspwZoneInfoTable[evCode];
    var zone = $("#" + zoneObj.zoneName + "ZoneImage");
    zone.addClass("hidden");
    clearTimeout(zoneObj.timerId);
    zoneObj.timerId = null;
    zoneObj.currentAudIdx = -1;
}

function TspwClearAllZones()
{
	Object.keys(tspwZoneInfoTable).forEach(function(key,index) {
	    // key: the name of the object key
	    // index: the ordinal position of the key within the object
		TspwClearZone(key, "0");
	});
    $("#bottomDiv").html("");
}

function TspwGetAudioSeverity() {
	var audIdx = -1;
	Object.keys(tspwZoneInfoTable).forEach(function(evCode,index) {
		// Return highest severity audio that was played for any of the zones
		if (audIdx < tspwZoneInfoTable[evCode].currentAudIdx)
			audIdx = tspwZoneInfoTable[evCode].currentAudIdx;
	});
	return audIdx;
}

//TODO: remove below - test only
function TspwDisplayAllZones(sev)
{
	Object.keys(tspwZoneInfoTable).forEach(function(evCode,index) {
		var zoneObj = tspwZoneInfoTable[evCode];
		if (sev in tspwValidSeverity) {
			var zone = $("#" + zoneObj.zoneName + "ZoneImage");		
			zone.removeClass("hidden");
		    zone.attr('src', zoneObj.files[sev].imgFile);
		}
	});
}
//------- TSPW End -------------------------------------------------------


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
    "../sounds/EEBL_warning.wav",
 // -----------------------------------------------------    
    "../sounds/TSPW/TSPW_Caution\ Curb.mp3",		// 27
    "../sounds/TSPW/TSPW_Caution\ in\ Road.mp3",           
    "../sounds/TSPW/TSPW_Warning\ in\ Road.mp3"
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
