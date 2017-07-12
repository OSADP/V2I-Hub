var speechString = { string: "", hasBeenSpoken: false };
var volume = 1;

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

function speak(speachText) {
    speechString.string = speachText;
    speech = new SpeechSynthesisUtterance(speechString.string);
    var voicesList = window.speechSynthesis.getVoices();
//    console.log(voicesList);
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
    vol = 2 * (window.volume - 0.5);
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
            default:
                break;
        }
    }
    window.setPersistentCommonValuesFlag = true;
}

