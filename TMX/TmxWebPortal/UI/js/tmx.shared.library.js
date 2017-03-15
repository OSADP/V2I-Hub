var speechString = { string: "", hasBeenSpoken: false };

var ackQueue = [];

function replaceUnderscoresWithSpaces(name) {
    return name.replace(/_/g, " ");
}

function closePopup() {
    bPopup = false;
    $('#popupalert').addClass('hidden');
    document.getElementById("popuptype").innerHTML = "";
    document.getElementById("popupdevice").innerHTML = "";
    document.getElementById("popupmsg").innerHTML = "";
    document.getElementById("popuptype").setAttribute('data-msgNum',"");
}

var speechTimeout = null;

var context;
var currentOsc, gainOsc, speech;
var audioTimer, soundActive = false, overallAudioCounter = 0;
/*
function playSoundBeeps(freq, timeDuration, numberOfBeeps, continuousFlag) {
	    createWave(freq);
	    currentOsc.start();
	    audioTimer = setInterval(function () {
	        if (window.overallAudioCounter < numberOfBeeps) {
	            window.soundActive = true;
	            if (!continuousFlag) {
	                if (window.overallAudioCounter % 2 == 0) {
	                    window.gainOsc.gain.value = -1;
	                } else {
	                    window.gainOsc.gain.value = beepVolumes();
	                }
	            } else {
	                window.gainOsc.gain.value = beepVolumes();
	            }
	            window.overallAudioCounter++;
	        } else {
	            stopWave();
	        }
	    }, timeDuration);
        playBeeps(freq, timeDuration, numberOfBeeps, continuousFlag);
}
*/
function playBeeps(freq, timeDuration, numberOfBeeps, continuousFlag) {
    createWave(freq);
    currentOsc.start();

    console.log("Starting Sound freq:" + freq + " duration:" + timeDuration + " beeps:" + numberOfBeeps + " cont:" + continuousFlag);

    audioTimer = setInterval(function () {
        if (window.overallAudioCounter < numberOfBeeps) {
            window.soundActive = true;
            if (!continuousFlag) {
                if (window.overallAudioCounter % 2 == 0) {
                    console.log("Window Gain = -1");
                    window.gainOsc.gain.value = -1;
                } else {
                    window.gainOsc.gain.value = beepVolumes();
                }
            } else {
                window.gainOsc.gain.value = beepVolumes();
            }
            window.overallAudioCounter++;
        } else {
            stopWave();
        }
    }, timeDuration);
}

function speak() {
    speech = new SpeechSynthesisUtterance(speechString.string);
    var voicesList = window.speechSynthesis.getVoices();
    speech.voice = voicesList[0];
    speech.lang = "en-US";
    speech.rate = 1;
    speech.pitch = 1;
    if (volume != 0) {
        speech.volume = 1;
    } else {
        speech.volume = volume;
    }
    // speechSynthesis.speak(speech);
    speechTimeout = setTimeout(function () {
        if (!speechString.hasBeenSpoken) {
            speechSynthesis.speak(speech);
            speechString.hasBeenSpoken = true;
        }
    }, 1000);

}

function cancelSpeech() {
    clearTimeout(speechTimeout);
    speechSynthesis.pause();
    speechSynthesis.cancel();
    speechString.hasBeenSpoken = false;
}

function beepVolumes() {
    var vol = window.volume;
//    if (vol >= 0.50) {
//        vol = 2 * (window.volume - 0.5) / 10.0;
//    } else {
        vol = 2 * (window.volume - 0.5);
//    }
    return vol;
}

function createWave(freq) {
    console.log("Creating Wave");
    if (window.soundActive) {
        stopWave();
    }
    window.gainOsc = context.createGain();
    var oscillator = context.createOscillator();
    oscillator.type = "sine";
    oscillator.frequency.value = freq;
    oscillator.connect(context.destination);
    window.currentOsc = oscillator;
    window.currentOsc.connect(window.gainOsc);
    window.gainOsc.connect(context.destination);
    window.gainOsc.gain.value = beepVolumes();
}

function stopWave() {
    if (window.currentOsc != null) {
        overallAudioCounter = 0;
        window.soundActive = false;
        window.currentOsc.stop(0);
        window.currentOsc.disconnect();
        clearInterval(window.audioTimer);
    }
}

function setPersistentCommonValues() {
    for (var i = 0; i < persistentCommonMemory.length; i++) {
        window[persistentCommonMemory[i].tag] = persistentCommonMemory[i].value;
        switch (persistentCommonMemory[i].tag) {
            case "popupOption":
                switchPopupOption(persistentCommonMemory[i].value);
                break;
            case "volume":
                window[persistentCommonMemory[i].tag] = parseFloat(persistentCommonMemory[i].value);
                var volPercent = (100 * window[persistentCommonMemory[i].tag]).toFixed(0);
                var slider = document.getElementById("volumeslider");
                if (slider != null)
                    slider.setAttribute('value', volPercent);
                break;
            case "excomHtmlPageLink":
                excomHtmlPageLink = excomHtmlPageLink.replace("http://", "");
                break;
            case "mainHtmlPageLink":
                break;
            default:
                break;
        }
    }
    window.setPersistentCommonValuesFlag = true;
}

function sortPriorityLevel(a, b) {
    if (a == null || a == undefined || a.priority == null || a.priority == undefined || a.msg == null || a.msg == undefined) {
        return 1;
    } else if (b == null || b == undefined || b.priority == null || b.priority == undefined || b.msg == null || b.msg == undefined) {
        return -1;
    }

    var priorityA = a.priority.toLowerCase();
    var priorityB = b.priority.toLowerCase();
    if (priorityA > priorityB) {
        if (priorityA == "warning" && priorityB == "operator verify") {
            return -1;
        } else {
            return 1;
        }
    } else if (priorityA < priorityB) {
        if (priorityA == "operator verify" && priorityB == "warning") {
            return 1;
        } else {
            return -1;
        }
    }
 
    // priorityA == priorityB
    if (priorityA == "operator verify") {
        var aTokens = a.msg.split(";");
        var bTokens = b.msg.split(";");
        if (aTokens[4] <= bTokens[4]) {
            return -1;
        } else if (aTokens[4] > bTokens[4]) {
            return 1;
        }
    }
    return 0;
}

function checkIfEqualOrHigherAlreadyInQueue(x) {
    if (x == null || x == undefined || x.msg == null || x.msg == undefined) return;
    var msgTokens = x.msg.split(";");
    for (var i = 0; i < popupQueue.length; i++) {
        if (popupQueue[i] == null || popupQueue[i] == undefined) {
            popupQueue.splice(i, 1);
            i--;
        }else {
            if (popupQueue[i].msg != null && popupQueue[i].msg != undefined) {
                var tokens = popupQueue[i].msg.split(";");
                if (tokens[0] == msgTokens[0] && tokens[2] == msgTokens[2] && tokens[3] == msgTokens[3]) {
                    if (tokens[2] == "LIMIT") {
                        if (sortPriorityLevel(popupQueue[i], x) <= 0) {
                            return true;
                        }
                    } else if (tokens[2] == "OP_VERIFY") {
                        if (tokens[4] <= msgTokens[4] && tokens[5] == msgTokens[5]) {
                            if (device == "AP") {
                                if (tokens[6] == msgTokens[6]) {
                                    return true;
                                }
                            } else {
                                return true;
                            }
                        } else if ((device != "AP" && tokens[5] == msgTokens[5]) || (device == "AP" && tokens[6] == msgTokens[6])) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    var type, device, popupmsg, msgNum;
    type = window.document.getElementById("popuptype").innerHTML;
    device = window.document.getElementById("popupdevice").innerHTML;
    popupmsg = window.document.getElementById("popupmsg").innerHTML;
    msgNum = window.document.getElementById("popuptype").getAttribute('data-msgNum');
    if (device == msgTokens[0] && type.toLowerCase() == x.priority) {
        if (type == "DANGER" || type == "LIMIT") {
            if (popupmsg.substring(0, popupmsg.indexOf(":")) == msgTokens[3]) {
                return true;
            }
        } else if (type == "Operator Verify" || type == "Error Verify") {
            if ((device != "AP" && msgNum <= msgTokens[4] && popupmsg == msgTokens[5]) ||
                (device == "AP" && msgNum <= msgTokens[4] && popupmsg == msgTokens[6])) {
                return true;
            } else if ((device != "AP" && popupmsg == msgTokens[5]) ||
                (device == "AP" && popupmsg == msgTokens[6])) {

            }
        }
    }
    popupQueue.sort(function (a, b) { return sortPriorityLevel(a, b); });
    return false;
}

function generateMessageFromPopup() {
    var type = window.document.getElementById("popuptype").innerHTML.toLowerCase();
    var device = window.document.getElementById("popupdevice").innerHTML;
    var message = window.document.getElementById("popupmsg").innerHTML;
    var msgNum = window.document.getElementById("popuptype").getAttribute('data-msgNum');

    var onlyNeededInfo = "";
    if (device != null && device != undefined && device != "") {
        var sensor = "";
        if (type != null && type != undefined && type != "") {
            if (type == "operator verify") {
                onlyNeededInfo = device + ";0;OP_VERIFY;VERIFY;" + msgNum + ";";
                if (device == "AP") {
                    onlyNeededInfo = "ADVISORY;";
                }
                onlyNeededInfo += message;
            } else if (type == "error verify") {
                onlyNeededInfo = device + ";0;OP_VERIFY;ERROR_VERIFY;" + msgNum + ";";
                if (device == "AP") {
                    onlyNeededInfo = "ERROR;";
                }
                onlyNeededInfo += message;
            } else if (type == "danger" || type == "warning") {
                var text = "";
                if (window.document.getElementById('popupalert').className.indexOf('dangerpopup') != -1) {
                    text += "RED_";
                } else if (window.document.getElementById('popupalert').className.indexOf('warningpopup') != -1) {
                    text += "YELLOW_";
                }

                if (message.toLowerCase().indexOf("upper") != -1) {
                    text += "LIMIT_HIGH";
                } else if (message.toLowerCase().indexOf("lower") != -1) {
                    text += "LIMIT_LOW";
                }
                sensor = message.substring(0, message.indexOf(":"));
                onlyNeededInfo = device + ";0;LIMIT;" + sensor + ";" + text + ";" + "ACK_REQUEST";
            }
        }
    }
    return { msg: onlyNeededInfo, type: type};
}

function checkPopupWithClearLimit(msg) {
    var tokens = msg.split(";");
    var device = window.document.getElementById("popupdevice").innerHTML;
    var message = window.document.getElementById("popupmsg").innerHTML;
    var sensor = message.substring(0, message.indexOf(":"));

    if (device == tokens[0] && sensor == tokens[3] && !$('#popupalert').hasClass('hidden')) {
        closePopup();
        //showPopupMessage();
    }
}

function savePopup(message) {
    if (message != null && message != undefined && message.msg.length > 0) {
//        console.log("savePopup popupQueue.push ->" + message + "<-")
        popupQueue.unshift(message);
    }
}

function swapForHigherPriorityLevel(x) {
    if (x.msg == null || x.msg == undefined) return;

    if (popupOption == 'beeps') {
        stopWave();
    } else {
        cancelSpeech();
    }
    var oldMessage = generateMessageFromPopup();

    var newMessageTokens = x.msg.split(";");
    if (oldMessage != undefined && oldMessage != null && oldMessage.length > 0) {
        var oldMessageTokens = oldMessage.msg.split(";");

        if (oldMessageTokens[3] != newMessageTokens[3]) {
//            console.log("swapForHigherPriorityLevel popupQueue.push ->" + oldMessage + "<-")
            popupQueue.push(oldMessage);
        }
    }
    popupQueue.unshift(x);
    popupQueue.sort(function (a, b) { return sortPriorityLevel(a, b); });
    showPopupMessage();
}

function cleanUpPopupQueue(device)
{
    if (device == null || device == undefined) {
        return;
    }
    for (var i = 0; i < popupQueue.length; i++) {
        if (popupQueue[i] == null || popupQueue[i] == undefined) {
            popupQueue.splice(i, 1);
            i--;
        } else {
            var msg = popupQueue[i].msg;
            if (msg != null && msg != undefined) {
                var tokens = msg.split(";");
                if (tokens[0] != null && tokens[0] != undefined) {
                    if (tokens[0].toLowerCase() == device.toLowerCase()) {
//                        console.log("cleanUpPopupQueue Removing Message - " + popupQueue[i].msg);
                        popupQueue.splice(i, 1);
                        i--;
                    }
                }
            }
        }
    }
}

function checkAckQueue(msg) {
    var bAlreadyInQueue = false;
    var tokens = msg.split(";");
    for (var i = 0; i < ackQueue.length; i++) {
        if (ackQueue[i].msg != null && ackQueue[i].msg != undefined) {
            var ackTokens = ackQueue[i].msg.split(";");
            if (ackTokens[0] == tokens[0] && ackTokens[2] == tokens[2]) {
                if (ackTokens[2] == "OP_VERIFY" && ackTokens[4] == tokens[4]) {
                    bAlreadyInQueue = true;
                    break;
                }
            }
        }
    }
    if (!bAlreadyInQueue) {
        ackQueue.push( { msg: msg, check : false} );
    } 
}

cleanUpAckQueueInterval = setInterval( function() {
    for (var i = 0; i < ackQueue.length; i++) {
        if (ackQueue[i].check) {
//            console.log("cleanUpAckQueueInterval Removing Message - " + ackQueue[i].msg);
            ackQueue.splice(i, 1);
            i--;
        } else {
            ackQueue[i].check = true;
        }
    }
 }, 30000);

function sortAlertAndVerifyMessages(msg) {
    if (msg != undefined && msg != null && msg.length > 0) {
//        console.log("sortAlertAndVerifyMessages Start ->" + msg + "<-");
//        printPopupQueue();


        var tokens = msg.split(";");
        var removeItemStatus = false;
        if (tokens[0].toLowerCase() == "device" || tokens[0] == "" || tokens[1].toLowerCase() == "type" || tokens[1] == "") return;

        if (tokens.length > 3 && tokens[2] == "OP_VERIFY") {
            if (tokens[3] == "HAS_BEEN_ACKED") {
                if (!hasBeenAckedOpVerify(tokens[0], tokens[4])) {
                    checkAckQueue(msg);
                    return;
                } else {
                    popupQueue.sort(function (a, b) { return sortPriorityLevel(a, b); });
                    return;
                }
            } else {
                for (var i = 0; i < ackQueue.length; i++) {
                    if (ackQueue[i].msg != null && ackQueue[i].msg != undefined) {
                        var ackTokens = ackQueue[i].msg.split(";");
                        if (ackTokens[0] == tokens[0] && ackTokens[2] == tokens[2] && tokens[4] == ackTokens[4]) {
//                            console.log("sortAlertAndVerifyMessages Removing Message - " + ackQueue[i].msg);
                            ackQueue.splice(i, 1);
                            return;
                        }
                    }
                }
            }
            if (tokens[3] == "ERROR_VERIFY") {
                x = { msg: msg, priority: "error verify" };
                if ((window.document.getElementById('popupalert').className.indexOf('opverifypopup') != -1 && window.document.getElementById('popuptype') != "Error Verify") || (window.document.getElementById('popupalert').className.indexOf('warningpopup') != -1)) {
                    swapForHigherPriorityLevel(x);
                    return;
                }
            } else {
                x = { msg: msg, priority: "operator verify" };
            }

            var cStatus = checkIfEqualOrHigherAlreadyInQueue(x);
            if (cStatus != null && cStatus != undefined && cStatus) {
                removeItemStatus = removeOlderFromQueue(x);
            }
        } else if (tokens[2] == "LIMIT") {
           if (tokens[4] == "YELLOW_LIMIT_HIGH" || tokens[4] == "YELLOW_LIMIT_LOW" || tokens[4].toLowerCase() == "warning") {
               if (tokens[5] == "HAS_BEEN_ACKED") {
                	if (!hasBeenAckedAlert(tokens[0],tokens[3],tokens[4])) {
                        ackQueue.push( { msg: msg, check: false } );
                        return;
                    }
                } else {
                    for (var i = 0; i < ackQueue.length; i++) {
                        if (ackQueue[i].msg != null && ackQueue[i].msg != undefined) {
                            var ackTokens = ackQueue[i].msg.split(";");
                            if (ackTokens[0] == tokens[0] && ackTokens[2] == tokens[2] && tokens[4] == ackTokens[4] && tokens[5] == ackTokens[5]) {
//                                console.log("sortAlertAndVerifyMessages token > 5 Removing Message - " + ackQueue[i].msg);
                                ackQueue.splice(i, 1);
                                return;
                            }
                        }
                    }
                }
                var x = { msg: msg, priority: "warning" };

                var cStatus = checkIfEqualOrHigherAlreadyInQueue(x);
                if (cStatus != null && cStatus != undefined && cStatus) {
                    return;
                }

                if (window.document.getElementById('popupalert').className.indexOf('opverifypopup') != -1) {
                    swapForHigherPriorityLevel(x);
                    removeItemStatus = removeOlderFromQueue(x);
                    return;
                }
            } else if (tokens[4] == "RED_LIMIT_HIGH" || tokens[4] == "RED_LIMIT_LOW" || tokens[4].toLowerCase() == "danger") {

	           if (tokens[5] == "HAS_BEEN_ACKED") {
                	if (!hasBeenAckedAlert(tokens[0],tokens[3],tokens[4])) {
                            ackQueue.push( { msg: msg, check: false });
                            return;
                    }
            	} else {
	               for (var i = 0; i < ackQueue.length; i++) {
	                   if (ackQueue[i].msg != null && ackQueue[i].msg != undefined) {
	                       var ackTokens = ackQueue[i].msg.split(";");
	                       if (ackTokens[0] == tokens[0] && ackTokens[2] == tokens[2] && tokens[4] == ackTokens[4] && tokens[5] == ackTokens[5]) {
//                               console.log("sortAlertAndVerifyMessages RLH RLL DANGER Removing Message - " + ackQueue[i].msg);
	                           ackQueue.splice(i, 1);
	                           return;
	                       }
	                   }
                    }
                }
                var x = { msg: msg, priority: "danger" };

                var cStatus = checkIfEqualOrHigherAlreadyInQueue(x);
                if (cStatus != null && cStatus != undefined && cStatus) {
                    return;
                }
                if (window.document.getElementById('popupalert').className.indexOf('dangerpopup') == -1) {
                    swapForHigherPriorityLevel(x);
                    removeStatusItem = removeOlderFromQueue(x);
                    return;
                }
            }
            else if (tokens[4] == "CLEAR_LIMIT")
            {
                // Clear Limit code
                for (var i = 0; i < popupQueue.length; i++) {
                    if (popupQueue[i].msg != null && popupQueue[i].msg != undefined) {
                        var popupTokens = popupQueue[i].msg.split(";");
                        if (tokens[0] == popupTokens[0] && tokens[2] == popupTokens[2] && tokens[3] == popupTokens[3]) {
                            popupQueue.splice(i, 1);
                            return;
                        }
                    }
                }
                checkPopupWithClearLimit(msg);
                return;
            }
        }
        if (x != null && x != undefined) {
            popupQueue.push(x);
        }
        popupQueue.sort(function (a, b) { return sortPriorityLevel(a, b); });
//        console.log("sortAlertAndVerifyMessages End")
//        printPopupQueue();
        if (popupQueue.length > 0 && document.getElementById("popupdevice").innerHTML == "") {
            showPopupMessage();
        }
    }
}

function removeDeviceMessagesFromQueue(device)
{
    if (device == null || device == undefined || device == "") {
        return;
    }
    for (var i = 0; i < popupQueue.length; i++){
        var tokens = popupQueue[i].msg.split(";");
        if (tokens[0] == device) {
//            console.log("removeDeviceMessagesFromQueue Removing Message - " + popupQueue[i].msg);
            popupQueue.splice(i,1);
            i--;
            
        }
    }

    if (window.document.getElementById("popupdevice").innerHTML == device) {
        closePopup();
    }
}

function removeFromQueue(x) {
    if (x.msg == null || x.msg == undefined) return;
    var newMsgTokens = x.msg.split(";");
    var removeItemStatus = false;
    for (var i = 0; i < popupQueue.length; i++) {
        if (popupQueue[i] == null || popupQueue[i] == undefined) {
            popupQueue.splice(i, 1);
            i--;
        }else {
            if (popupQueue[i].msg != null && popupQueue[i].msg != undefined) {
                var tokens = popupQueue[i].msg.split(";");
                if (tokens[0] == newMsgTokens[0] && tokens[2] == newMsgTokens[2]) {
                    if (tokens[2] == "OP_VERIFY" && tokens[4] != newMsgTokens[4]) continue;
//                    console.log("removeFromQueue Removing Message - " + popupQueue[i].msg);
                    popupQueue.splice(i, 1);
                    removeStatus = true;
                    break;
                }
            }
        }
    }
    return removeItemStatus;
}

function removeOlderFromQueue(x) {
    if (x.msg == null || x.msg == undefined) return;
    var newMsgTokens = x.msg.split(";");
    var removeItemStatus = false;
    for (var i = 0; i < popupQueue.length; i++) {
        if (popupQueue[i] == null || popupQueue[i] == undefined) {
            popupQueue.splice(i, 1);
            i--;
        }else {
            if (popupQueue[i].msg != null && popupQueue[i].msg != undefined) {
                var tokens = popupQueue[i].msg.split(";");
                if (tokens[0] == newMsgTokens[0] && tokens[2] == newMsgTokens[2] && tokens[3] == newMsgTokens[3]) {
                    if (tokens[2] == "LIMIT") {
                        if (sortPriorityLevel(popupQueue[i], x) > 0 || (sortPriorityLevel(popupQueue[i], x) <= 0 && tokens[1] < newMsgTokens[1])) {
//                            console.log("removeOlderFromQueue LIMIT Removing Message - " + popupQueue[i].msg);
                            popupQueue.splice(i, 1);
                            removeItemStatus = true;
                            break;
                        }
                    } else if (tokens[2] == "OP_VERIFY") {
                        if ((tokens[4] == newMsgTokens[4] || tokens[4] < newMsgTokens[4]) && tokens[5] == newMsgTokens[5]) {
//                            console.log("removeOlderFromQueue OP_VERIFY Removing Message - " + popupQueue[i].msg);
                            popupQueue.splice(i, 1);
                            removeItemStatus = true;
                            break;
                        }
                    }
                }
            }
        }
    }
    return removeItemStatus;
}

function hasBeenAckedOpVerify(device, msgNumber) {
    //var removeItemStatus = removeFromQueue(x);
    var removeItemStatus = false;
    for (var i = 0; i < popupQueue.length; i++) {
        
        if (popupQueue[i] == null || popupQueue[i] == undefined) {
            popupQueue.splice(i, 1);
            i--;
        } else {
            if (popupQueue[i].msg != null && popupQueue[i].msg != undefined) {
                var tokens = popupQueue[i].msg.split(";");
                if (tokens[0] == device && tokens[2] == "OP_VERIFY") {
                    if (tokens[4] != msgNumber) continue;
//                    console.log("hasBeenAckedOpVerify Removing Message - " + popupQueue[i].msg);
                    popupQueue.splice(i, 1);
                    removeItemStatus = true;
                    break;
                }
            }
        }
    }

    var dev = window.document.getElementById("popupdevice");
    var type = document.getElementById("popuptype");
    var msgNum = type.getAttribute("data-msgnum");
    if (dev.innerHTML == device && type.innerHTML.toLowerCase().indexOf("verify") != -1 && msgNum == msgNumber) {
        bPopup = false;
        if (popupOption == 'beeps') {
            stopWave();
        } else if (popupOption == 'voice') {
            cancelSpeech();
        }
        removeItemStatus = true;
        closePopup();
        showPopupMessage();
    }
    return removeItemStatus;
}

function hasBeenAckedAlert(device, sensor, alertType) {
    var removeItemStatus = false;
    for (var i = 0; i < popupQueue.length; i++) {
        if (popupQueue[i] == null || popupQueue[i] == undefined) {
            popupQueue.splice(i, 1);
            i--;
        } else {
            if (popupQueue[i].msg != null && popupQueue[i].msg != undefined) {
                var tokens = popupQueue[i].msg.split(";");
                if (tokens[0].toLowerCase() == device.toLowerCase() && tokens[3].toLowerCase() == sensor.toLowerCase() && alertType.toLowerCase() == tokens[4].toLowerCase()) {
//                    console.log("hasBeenAckedAlert Removing Message - " + popupQueue[i].msg);
                    popupQueue.splice(i, 1);
                    removeItemStatus = true;
                    break;
                }
            }
        }
    }

    var dev = window.document.getElementById("popupdevice").innerHTML;
    var type = document.getElementById("popuptype").innerHTML;
    var msg = document.getElementById("popupmsg").innerHTML;
    var alertTokens = msg.split(":");
    // If sensors are the same, close popup
    if (dev.toLowerCase() == device.toLowerCase() && sensor.toLowerCase() == alertTokens[0].toLowerCase()) {
        bPopup = false;
        if (popupOption == 'beeps') {
            stopWave();
        } else if (popupOption == 'voice') {
            cancelSpeech();
        }
        removeItemStatus = true;
        closePopup();
        showPopupMessage();
    }
    return removeItemStatus;
}

/**
*   Generate and execute both visual and audio versions of operator verify message. 
*   Prepare the outgoing message to be sent when the user confirms the popup. 
*   @private
*   @param {string} msg - Operator verify message to be displayed
**/
function showVerifyMessage(msg) {
    if (msg == null || msg == undefined) return;
    var tokens = msg.split(";");
    if (tokens[3] == "VERIFY" || tokens[3] == "ERROR_VERIFY") {
        var string = "";
        bPopup = true;
        if (tokens.length > 5) {
            if (tokens[0] == "AP") {
                string = tokens[6];
            } else {
                string = tokens[5];
            }
        }

        if ((tokens[3] != "ERROR_VERIFY" && displayPopup(tokens[0], "Operator Verify", tokens[4], string)) ||
         (tokens[3] == "ERROR_VERIFY" && displayPopup(tokens[0], "Error Verify", tokens[4], string))) {
            // Say it
            if (popupOption == 'voice') {
                speechString.string = string;
                speechString.hasBeenSpoken = false;
                if (!speechSynthesis.speaking) {
                    console.log("Speaking Sound " + string);
                    speak();
                }
                console.log("Talking:" + string + " - " + window.document.baseURI);
            } else if (popupOption == 'beeps') {
                if (tokens[3] != "ERROR_VERIFY") {
                    console.log("Playing Sound VERIFY");
//                    playBeeps(900, 5, 200, true);
                    playBeeps(900, 400, 1, true);
                } else {
                    console.log("Playing Sound ERROR_VERIFY");
                    playBeeps(3000, 250, 4, false);
                }
            }
            window.document.getElementById('popupwindow').click();
            

            // playWave(900, 2);
        }
    }
}
/**
*   Generate and execute both visual and audio versions of the alert message if the alert is an "ACK_REQUEST."
*   Prepare the outgoing message to be sent when the user confirms the popup.
*   @private
*   @param {string} msg - Alert message to be displayed
**/
function showAlertMessage(msg) {
    if (msg == null || msg == undefined) return;
    var tokens = msg.split(";");
    if (tokens.length < 6) {
        return;
    }

    if (tokens[5] == "ACK_REQUEST") {
        var speakString = "";
        var string = "";
        var type = "";
        bPopup = true;
        var sensor = replaceUnderscoresWithSpaces(tokens[3]);
        switch (tokens[4]) {
            case "RED_LIMIT_HIGH":
                string = "Exceeded Upper Danger Limit.";
                type = "DANGER";
                speakString = "Danger the upper limit has been reached for sensor " + sensor;
                break;
            case "RED_LIMIT_LOW":
                string = "Exceeded Lower Danger Limit.";
                type = "DANGER";
                speakString = "Danger the lower limit has been reached for sensor " + sensor;
                break;
            case "YELLOW_LIMIT_HIGH":
                string = "Exceeded Upper Warning Limit.";
                type = "WARNING";
                speakString = "Warning the upper limit has been reached for sensor " + sensor;
                break;
            case "YELLOW_LIMIT_LOW":
                string = "Exceeded Lower Warning Limit.";
                type = "WARNING";
                speakString = "Warning the lower limit has been reached for sensor " + sensor;
                break;
            default:
                break;
        }

        if (displayPopup(tokens[0], type, tokens[1], tokens[3] + ": " + string)) {
            console.log("Starting Sound");
            if (popupOption == 'voice') {
                console.log("Talking:" + speakString + " - " + window.document.baseURI);
                speechString.string = speakString;
                speechString.hasBeenSpoken = false;
                if (!speechSynthesis.speaking) {
                    speak();
                }
            } else if (popupOption == 'beeps') {
                if (window.document.getElementById("popuptype").innerHTML == "WARNING") {
                    console.log("Playing Sound WARNING");
                    playBeeps(1100, 300, 4, false);
                } else if (window.document.getElementById("popuptype").innerHTML == "DANGER") {
                    console.log("Playing Sound DANGER");
                    playBeeps(3500, 250, 4, false);
                }
            }
            window.document.getElementById('popupwindow').click();
        }
    }
}

function displayPopup(device, type, msgNum, msg) {
    if (device == null || device == undefined || device.length <= 0 || device.toLowerCase() == "device" || type == null || type == undefined || type.length <= 0 || type.toLowerCase() == "type") return false;

    $('#popupCancel').addClass('hidden');
    window.document.getElementById('popupdevice').innerHTML = device;
    window.document.getElementById('popuptype').innerHTML = type;
    window.document.getElementById('popuptype').setAttribute('data-msgNum', msgNum);
    if (type.toUpperCase() == "WARNING") {
        window.document.getElementById('popupalert').setAttribute('class', 'warningpopup');
    } else if (type.toUpperCase() == "DANGER") {
        window.document.getElementById('popupalert').setAttribute('class', 'dangerpopup');
    } else {
        if (type == "Operator Verify") {
            window.document.getElementById('popupalert').setAttribute('class', 'opverifypopup');
        } else {
            window.document.getElementById('popupalert').setAttribute('class', 'dangerpopup');
        }
    }

    window.document.getElementById('popupmsg').innerHTML = msg;
    $('#popupalert').removeClass('hidden');
    bPopup = true;

    return true;
}