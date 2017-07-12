console.log("including tmx.components.library.js");
var componentLibraryJsVersion = "0.0.2";

// Gauge -----------------------------------------------------------------------------
Gauge = function (component) {
    var node = null;
    var startnum;
    var meterreading;
    var endnum;
    var label;

    var obj = this;
    obj.start = 0;
    obj.end = 5000;
    obj.meterreading = 0;
    obj.label = "";

    obj.node = component;

    if (obj.node.getAttribute("data-startValue") != null || obj.node.getAttribute("data-startValue") != undefined || obj.node.getAttribute("data-startValue") != "---") {
        obj.start = obj.node.getAttribute("data-startValue");
    }
    if (obj.node.getAttribute("data-endValue") != null || obj.node.getAttribute("data-endValue") != undefined || obj.node.getAttribute("data-endValue") != "---") {
        obj.end = obj.node.getAttribute("data-endValue");
    }
    if (obj.node.getAttribute("data-value") != null || obj.node.getAttribute("data-value") != undefined || obj.node.getAttribute("data-value") != "---") {
        obj.meterreading = obj.node.getAttribute("data-value");
    }
    if (obj.node.getAttribute("data-label") != null || obj.node.getAttribute("data-label") != undefined ) {
        obj.label = obj.node.getAttribute("data-label");
    }

    obj.node.innerHTML += (
        "<figure-gauge>" +
            "<div class=\"cssgauge\"><div class=\"meter-cssgauge\"></div></div>" +
        "</figure-gauge>" +
        "<div class=\"gaugetext ongauge\">" +
        "<div>" + obj.label + "</div>" +
        "<div class=\"meterreading\">" + obj.meterreading + "</div>" + "</div>" +
        "<div class=\"startnumber gaugetext belowgauge\">" + obj.start + "</div>" +
        "<div class=\"endnumber gaugetext belowgauge\">" + obj.end + "</div>"
    );
    obj.node.addEventListener('mousedown', function() {
        obj.onmousedown(event);
    }, true);

    if ((obj.node.getAttribute("data-device") != null) && (obj.node.getAttribute("data-sensor") != null))
    {
        if (!checkTelemetryHandlerForElement(obj.node)) {
            registerObject(createTelemObject(obj, obj.node.getAttribute("data-device"), obj.node.getAttribute("data-sensor"), updateGauge, updateGageLimit, updateGaugeNoData));
        }
    }
}


Gauge.prototype.refresh = function (val) {
    if (isNaN(val)) {
        this.node.querySelector('.meterreading').innerHTML = "---";
        this.node.querySelector('.meter-cssgauge').style.transform = "rotate(0deg)";
        return;
    }
    var displayNumber = parseFloat(val);
    if (displayNumber != parseFloat(this.node.querySelector('.meterreading').innerHTML))
    {
        if ((val == "---") || (val == "reset_display"))
        {
            this.node.querySelector('.meterreading').innerHTML = "---";
            displayNumber = 0;
        }
        else if (displayNumber == NaN) 
        {
            this.node.querySelector('.meterreading').innerHTML = "NaN";
            displayNumber = 0;
        }
        else
        {
            displayVal = displayNumber;
            this.node.querySelector('.meterreading').innerHTML = displayNumber;
        }
        displayNumber = (displayNumber / this.end) * 180;
        if (displayNumber > 180) displayNumber = 180;
        this.node.querySelector('.meter-cssgauge').style.transform = "rotate(" + displayNumber + "deg)";
    }
};

Gauge.prototype.getValue = function () {
    return this.node.querySelector('.meterreading').innerHTML;
};

Gauge.prototype.onmousedown = function (event) {
    if (document.getElementById('popupMenu') != null && document.getElementById('popupMenu') != undefined) {
        var popup = this.node.getAttribute('data-nopopup');
        if (popup == "true") {
            return;
        }
        $("#popupMenuHeader").html(this.node.getAttribute("data-label")).enhanceWithin();
        $("#popupMenu").attr("data-device", this.node.getAttribute("data-device"));
        $("#popupMenu").attr("data-sensor", this.node.getAttribute("data-sensor"));
        $("#popupMenu").attr("data-minValue", this.node.getAttribute("data-minValue"));
        $("#popupMenu").attr("data-maxValue", this.node.getAttribute("data-maxValue"));
        var y_distance = event.y;
        if (y_distance >= 0.65 * screen.height) { y_distance *= 0.8; }
        $("#popupMenu").popup("open", { x: event.x, y: y_distance });
    }
};


Gauge.prototype.limit = function (newVal) {
    if ((newVal == "RED_LIMIT_HIGH") || (newVal == "RED_LIMIT_LOW")) {
        this.node.querySelector('.meter-cssgauge').style.background = "#F00";
    }
    else if ((newVal == "YELLOW_LIMIT_HIGH") || (newVal == "YELLOW_LIMIT_LOW")) {
        this.node.querySelector('.meter-cssgauge').style.background = "#FF0";
    }
    else {
        this.node.querySelector('.meter-cssgauge').style.background = "#178C19";
    }
};

// Linear Meter ----------------------------------------------------------------------
LinearMeter = function (component) {
    var node = null;
    var max;
    var min;
    var sections;

    var obj = this;

    obj.max = 100;
    obj.min = 0;
    obj.sections = 4;

    obj.node = component;
    var alarmIncludedFlag = obj.node.getAttribute("data-alarmState") != null && obj.node.getAttribute("data-alarmState") != undefined;

    if (obj.node.getAttribute("data-minValue") != null && obj.node.getAttribute("data-minValue") != undefined && obj.node.getAttribute("data-minValue") != "") {
        obj.min = parseFloat(obj.node.getAttribute("data-minValue"));
    }
    if (obj.node.getAttribute("data-maxValue") != null && obj.node.getAttribute("data-maxValue") != undefined && obj.node.getAttribute("data-maxValue") != "") {
        obj.max = parseFloat(obj.node.getAttribute("data-maxValue"));
    }
    if (obj.node.getAttribute("data-sections") != null && obj.node.getAttribute("data-sections") != undefined && obj.node.getAttribute("data-sections") != "") {
        obj.sections = parseFloat(obj.node.getAttribute("data-sections"));
    }

    obj.node.innerHTML += (
			"<div class='telem-meter-label'>" + obj.node.getAttribute("data-label"));

    obj.node.innerHTML += ( "</div>" +
			"<div class='mtrread'>" +
				"<div>" +
					"<div class='metermarks'></div>" +
                 "</div>" +
	             "<div class='meter green nostripes'>" +
	            	"<span style='width: 0%' class='progress'></span>" +
	            "</div>" +
	        	"<div class='numdivcontainer'></div>" +
	        "</div>" +
	        "<div class='telem-meter-value'>---</div>" +
            "<div class='telem-meter-units'>" + obj.node.getAttribute("data-units") + "</div>"
    );

    this.node.querySelector('.progress').style.background = "#178C19";
    // Change to use dynamic data-xxx value
    for (var a = obj.sections; a > 0; a--) {
        var divValue = (obj.max - (a * (obj.max - obj.min)) / obj.sections);
        //if ((divValue - Math.floor(divValue)) != divValue) {
        divValue = divValue.toFixed(1);
        //}

        if (a == obj.sections) {
            this.node.querySelector('.numdivcontainer').innerHTML += "<div class='firstnumdiv'>" + divValue + "</div>";
        } else {
            this.node.querySelector('.numdivcontainer').innerHTML += "<div class='numdiv'>" + divValue + "</div>";
        }
    }
    this.node.querySelector('.numdivcontainer').innerHTML += "<div class='lastnumdiv'>" + obj.max.toFixed(1) + "</div>";
    // Change to use dynamic data-xxx value
    for (var m = 0; m < obj.sections; m++) {
        this.node.querySelector('.metermarks').innerHTML += "<div class='mark'></div>";
    }

    obj.node.addEventListener('mousedown', function () {
        obj.onmousedown(event);
    }, true);

    if ((obj.node.getAttribute("data-device") != null) && (obj.node.getAttribute("data-sensor") != null)) {
        if (!checkTelemetryHandlerForElement(obj.node)) {
            registerObject(createTelemObject(obj, obj.node.getAttribute("data-device"), obj.node.getAttribute("data-sensor"), updateCustom, updateCustomLimit, updateCustomNoData));

            if (alarmIncludedFlag && !checkAlarmHandlerForElement(obj.node)) {
                registerAlarmObject(createAlarmObject(obj, obj.node.getAttribute("data-device"), obj.node.getAttribute("data-sensor"), updateAlarm, updateCustomNoData, updateMode));
            }
        }
    }
};

LinearMeter.prototype.refresh = function (val) {
    if (isNaN(val)) {
        this.node.querySelector('.telem-meter-value').innerHTML = "---";
        this.node.querySelector('.progress').style.width = "0%";
        return;
    }
    val = parseFloat(val);
    var decimals = this.node.getAttribute("data-decimals");
    var displayNumber = val;
    if (decimals != null && decimals != undefined) {
        var displayNumber = val.toFixed(decimals);
    } 
    if (displayNumber != parseFloat(this.node.querySelector('.telem-meter-value').innerHTML))
    {
        if ((val == "---") || (val == "reset_display")) {
            this.node.querySelector('.telem-meter-value').innerHTML = "---";
            displayNumber = 0;
        }
        else if (displayNumber == NaN) {
            this.node.querySelector('.telem-meter-value').innerHTML = "NaN";
            displayNumber = 0;
        }
        else {
            displayVal = displayNumber;
            this.node.querySelector('.telem-meter-value').innerHTML = displayNumber;
        }
        displayNumber = ((displayNumber - this.min) / (this.max-this.min)) * 100;
        if (displayNumber > 100) displayNumber = 100;
        else if (displayNumber <= 0.5) {
            displayNumber = 0.5;
        }
        this.node.querySelector('.progress').style.width = displayNumber + "%";
    }

};

LinearMeter.prototype.refreshAlarm = function (val) {
    if (val == "DISABLED" || val == "0") {
        this.node.querySelector('.telem-meter-label').style.color = "orange";
        this.node.setAttribute("data-alarmstate", "disabled");
    } else if (val == "ENABLED" || val == "1") {
        this.node.querySelector('.telem-meter-label').style.color = "#FFFFFF";
        this.node.setAttribute("data-alarmstate", "enabled");
    }
};

LinearMeter.prototype.refreshMode = function (val) {
    this.node.setAttribute('data-mode', val);
};

LinearMeter.prototype.getValue = function() 
{
	return this.node.querySelector('.telem-meter-value').innerHTML;
};

LinearMeter.prototype.onmousedown = function (event) {
    if (document.getElementById('popupMenu') != null && document.getElementById('popupMenu') != undefined) {
        var popup = this.node.getAttribute('data-nopopup');
        if (popup == "true") {
            return;
        }
        $("#popupMenuHeader").html(this.node.getAttribute("data-label")).enhanceWithin();
        $("#popupMenu").attr("data-device", this.node.getAttribute("data-device"));
        $("#popupMenu").attr("data-sensor", this.node.getAttribute("data-sensor"));
        $("#popupMenu").attr("data-minValue", this.node.getAttribute("data-minValue"));
        $("#popupMenu").attr("data-maxValue", this.node.getAttribute("data-maxValue"));

        if (this.node.getAttribute('data-alarmState') == "enabled") {
            document.getElementById('popupTelemAlarm').innerHTML = "Disable Alarm";
        } else if (this.node.getAttribute('data-alarmState') == "disabled") {
            document.getElementById('popupTelemAlarm').innerHTML = "Enable Alarm";
        }
        var mode = this.node.getAttribute('data-mode');
        if (mode != null && mode != undefined) {
            if (this.node.getAttribute('data-mode') == "SW_MANUAL") {
                document.getElementById('popupMenuManual').innerHTML = "Set Automatic";
            } else {
                document.getElementById('popupMenuManual').innerHTML = "Set Manual";
            }
        }
        var y_distance = event.y;
        if (y_distance >= 0.65 * screen.height) { y_distance *= 0.8; }
        $("#popupMenu").popup("open", { x: event.x, y: y_distance });
    }
};


LinearMeter.prototype.limit = function(newVal) 
{
	if ((newVal == "RED_LIMIT_HIGH") || (newVal == "RED_LIMIT_LOW"))
	{
		this.node.querySelector('.progress').style.background = "#F00";
	}
	else if ((newVal == "YELLOW_LIMIT_HIGH") || (newVal == "YELLOW_LIMIT_LOW"))
	{
		this.node.querySelector('.progress').style.background = "#FF0";
	}
	else
	{
        this.node.querySelector('.progress').style.background = "#178C19";
	}
};

LinearMeter.prototype.name = function (name, replacement) {
    var replacing = this.node.querySelector('.telem-meter-label');
    if (replacing != null && replacing != undefined) {
        this.node.setAttribute("data-label", replacement);
        replacing.innerHTML = replacement;
    }
};
LinearMeter.prototype.units = function (val) {
    var units = this.node.querySelector('.templateUnits');
    if (units != null && units != undefined) {
        units.innerHTML = val;
    }
};
// -----  End Linear Meter --------------------------------------------------------------
// Vertical Meter ----------------------------------------------------------------------
VerticalMeter = function (component) {
    var node = null;
    var max;
    var min;
    var sections;

    var obj = this;

    obj.max = 100;
    obj.min = 0;
    obj.sections = 4;


    obj.node = component;
    if (obj.node.getAttribute("data-minValue") != null && obj.node.getAttribute("data-minValue") != undefined && obj.node.getAttribute("data-minValue") != "") {
        obj.min = parseFloat(obj.node.getAttribute("data-minValue"));
    }
    if (obj.node.getAttribute("data-maxValue") != null && obj.node.getAttribute("data-maxValue") != undefined && obj.node.getAttribute("data-maxValue") != "") {
        obj.max = parseFloat(obj.node.getAttribute("data-maxValue"));
    }
    if (obj.node.getAttribute("data-sections") != null && obj.node.getAttribute("data-sections") != undefined && obj.node.getAttribute("data-sections") != "") {
        obj.sections = parseFloat(obj.node.getAttribute("data-sections"));
    }
    var alarmIncludedFlag = obj.node.getAttribute("data-alarmState") != null && obj.node.getAttribute("data-alarmState") != undefined;

    obj.node.innerHTML += (
            "<div class='verticalmtrread'>" +
                "<div class='verticalmeter'>" +
                    "<span class='progress'></span>" +
                 "</div>" +
                 "<div class='verticalmetermarks'></div>" +
                 "<div class='verticalnumdivcontainer'></div>" +
            "</div>" +
        "<div class='center'><div class='telem-vertmeter-value inline'></div>" + 
        "<div class='telem-vertmeter-units inline'>" + obj.node.getAttribute("data-units") + "</div></div>" + 
        "<div class='telem-vertmeter-label'>" + obj.node.getAttribute("data-label") + "</div>"
        
    );

    this.node.querySelector('.progress').style.background = "#178C19";
    // Change to use dynamic data-xxx value
    for (var a = 0; a <= obj.sections; a++) {
        var divValue = (obj.max - (a * (obj.max - obj.min)) / obj.sections);
        //if ((divValue - Math.floor(divValue)) != obj.min) {
        divValue = divValue.toFixed(1);
        //}
        this.node.querySelector('.verticalnumdivcontainer').innerHTML += "<div class='verticalnumdiv'>" + divValue + "</div>";
    }
    // Change to use dynamic data-xxx value
    for (var m = 0; m < obj.sections; m++) {
        this.node.querySelector('.verticalmetermarks').innerHTML += "<div class='verticalmark' ></div>";
    }

    obj.node.addEventListener('mousedown', function () {
        obj.onmousedown(event);
    }, true);

    obj.node.querySelector('.progress').style.height = "0%";
    obj.node.querySelector('.telem-vertmeter-value').innerHTML = "---";

    if ((obj.node.getAttribute("data-device") != null) && (obj.node.getAttribute("data-sensor") != null)) {
        if (!checkTelemetryHandlerForElement(obj.node)) {
            registerObject(createTelemObject(obj, obj.node.getAttribute("data-device"), obj.node.getAttribute("data-sensor"), updateCustom, updateCustomLimit, updateCustomNoData));
            if (alarmIncludedFlag && !checkAlarmHandlerForElement(obj.node)) {
                registerAlarmObject(createAlarmObject(obj, obj.node.getAttribute("data-device"), obj.node.getAttribute("data-sensor"), updateAlarm, updateCustomNoData, updateMode));
            }
        }
    }
};

VerticalMeter.prototype.refresh = function (val) {
    if (isNaN(val) || val == "") {
        this.node.querySelector('.telem-vertmeter-value').innerHTML = "---";
        this.node.querySelector('.progress').style.height = "0%";
        return;
    }
    val = parseFloat(val);
    var decimals = this.node.getAttribute("data-decimals");
    var displayNumber = val;
    if (decimals != null && decimals != undefined) {
        displayNumber = val.toFixed(decimals);
    }
    if (displayNumber != parseFloat(this.node.querySelector('.telem-vertmeter-value').innerHTML))
    {
        if ((val == "---") || (val == "reset_display")) {
            this.node.querySelector('.telem-vertmeter-value').innerHTML = "---";
            displayNumber = 0;
        }
        else if (displayNumber == NaN) {
            this.node.querySelector('.telem-vertmeter-value').innerHTML = "NaN";
            displayNumber = 0;
        }
        else {
            displayVal = displayNumber;
            this.node.querySelector('.telem-vertmeter-value').innerHTML = displayNumber;
        }
        displayNumber = ((displayNumber-this.min) / (this.max-this.min)) * 100;
        if (displayNumber > 100) displayNumber = 100;
        else if (displayNumber <= 0.5) {
            displayNumber = 0.5;
        }
        this.node.querySelector('.progress').style.height = displayNumber + "%";
    }
};

VerticalMeter.prototype.refreshAlarm = function (val) {
    if (val == "DISABLED" || val == "0") {
        this.node.querySelector('.telem-vertmeter-label').style.color = "orange";
        this.node.setAttribute("data-alarmstate", "disabled");
    } else if (val == "ENABLED" || val == "1") {
        this.node.querySelector('.telem-vertmeter-label').style.color = "#FFFFFF";
        this.node.setAttribute("data-alarmstate", "enabled");
    }
};

VerticalMeter.prototype.refreshMode = function (val) {
    this.node.setAttribute('data-mode', val);
};

VerticalMeter.prototype.onmousedown = function (event) {
    if (document.getElementById('popupMenu') != null && document.getElementById('popupMenu') != undefined) {
        var popup = this.node.getAttribute('data-nopopup');
        if (popup == "true") {
            return;
        }
        $("#popupMenuHeader").html(this.node.getAttribute("data-label")).enhanceWithin();
        $("#popupMenu").attr("data-device", this.node.getAttribute("data-device"));
        $("#popupMenu").attr("data-sensor", this.node.getAttribute("data-sensor"));
        $("#popupMenu").attr("data-minValue", this.node.getAttribute("data-minValue"));
        $("#popupMenu").attr("data-maxValue", this.node.getAttribute("data-maxValue"));

        if (this.node.getAttribute('data-alarmState') == "enabled") {
            document.getElementById('popupTelemAlarm').innerHTML = "Disable Alarm";
        } else if (this.node.getAttribute('data-alarmState') == "disabled") {
            document.getElementById('popupTelemAlarm').innerHTML = "Enable Alarm";
        }
        var mode = this.node.getAttribute('data-mode');
        if (mode != null && mode != undefined) {
            if (this.node.getAttribute('data-mode') == "SW_MANUAL") {
                document.getElementById('popupMenuManual').innerHTML = "Set Automatic";
            } else {
                document.getElementById('popupMenuManual').innerHTML = "Set Manual";
            }
        }

        var y_distance = event.y;
        if (y_distance >= 0.65 * screen.height) { y_distance *= 0.8;
        }
        $("#popupMenu").popup("open", { x: event.x, y: y_distance });
    }
};


VerticalMeter.prototype.getValue = function() 
{
	return this.node.querySelector('.telem-vertmeter-value').innerHTML;
};

VerticalMeter.prototype.limit = function(newVal) 
{
	if ((newVal == "RED_LIMIT_HIGH") || (newVal == "RED_LIMIT_LOW"))
	{
		this.node.querySelector('.progress').style.background = "#F00";
	}
	else if ((newVal == "YELLOW_LIMIT_HIGH") || (newVal == "YELLOW_LIMIT_LOW"))
	{
		this.node.querySelector('.progress').style.background = "#FF0";
	}
	else
	{
        this.node.querySelector('.progress').style.background = "#178C19";
	}
};

VerticalMeter.prototype.name = function (name, replacement) {
    var replacing = this.node.querySelector('.telem-vertmeter-label');
    if (replacing != null && replacing != undefined) {
        this.node.setAttribute("data-label", replacement);
        replacing.innerHTML = replacement;
    }
}
VerticalMeter.prototype.units = function (val) {
    var units = this.node.querySelector;
    if (units != null && units != undefined) {
        units.innerHTML = val;
    }
};
// -----  End Vertical Meter --------------------------------------------------------------
// Telemetry display ----------------------------------------------------------------------
TelemDisplay = function (component) {
    var node = null;
    var obj = this;
    var currentVal;
    var currentAlarm;
    var currentMode;

    obj.node = component;
    var newHTML = "";
    var alarmIncludedFlag = obj.node.getAttribute("data-alarmState") != null && obj.node.getAttribute("data-alarmState") != undefined;

    if ((obj.node.getAttribute("data-label") != "") && (obj.node.getAttribute("data-label") != null)) {
        newHTML += "<span class='templateLabel'>"; //+ "</span>";
        newHTML += obj.node.getAttribute("data-label") + "</span>";
    }

    newHTML += "<div class='templateValue'>---</div>";
    if ((obj.node.getAttribute("data-units") != "") && (obj.node.getAttribute("data-units") != null)) {
        newHTML += "<span class='templateUnits'>" + obj.node.getAttribute("data-units") + "</span>";
    }
    obj.node.innerHTML += (newHTML);

    obj.node.addEventListener('mousedown', function () {
        obj.onMouseDown(event);
    }, true);
    if ((obj.node.getAttribute("data-device") != null) && (obj.node.getAttribute("data-sensor") != null)) {
        if (!checkTelemetryHandlerForElement(obj.node)) {
            registerObject(createTelemObject(obj, obj.node.getAttribute("data-device"), obj.node.getAttribute("data-sensor"), updateCustom, updateCustomLimit, updateCustomNoData));
            if (alarmIncludedFlag && !checkAlarmHandlerForElement(obj.node)) {
                registerAlarmObject(createAlarmObject(obj, obj.node.getAttribute("data-device"), obj.node.getAttribute("data-sensor"), updateAlarm, updateCustomNoData, updateMode));
            }
        }
    }
};

TelemDisplay.prototype.onMouseDown = function (event) {
    if (document.getElementById('popupMenu') != null && document.getElementById('popupMenu') != undefined) {
        var popup = this.node.getAttribute('data-nopopup');
        if (popup == "true") {
            return;
        }
        $("#popupMenuHeader").html(this.node.getAttribute("data-label")).enhanceWithin();
        $("#popupMenu").attr("data-device", this.node.getAttribute("data-device"));
        $("#popupMenu").attr("data-sensor", this.node.getAttribute("data-sensor"));
        $("#popupMenu").attr("data-minValue", this.node.getAttribute("data-minValue"));
        $("#popupMenu").attr("data-maxValue", this.node.getAttribute("data-maxValue"));

        if (this.node.getAttribute('data-alarmState') == "enabled") {
            document.getElementById('popupTelemAlarm').innerHTML = "Disable Alarm";
        } else if (this.node.getAttribute('data-alarmState') == "disabled") {
            document.getElementById('popupTelemAlarm').innerHTML = "Enable Alarm";
        }
        var mode = this.node.getAttribute('data-mode');
        if (mode != null && mode != undefined) {
            if (this.node.getAttribute('data-mode') == "SW_MANUAL") {
                document.getElementById('popupMenuManual').innerHTML = "Set Automatic";
            } else {
                document.getElementById('popupMenuManual').innerHTML = "Set Manual";
            }
        }
        var y_distance = event.y;
        if (y_distance >= 0.65 * screen.height) { y_distance *= 0.8; }
        $('#popupMenu').popup();
        $("#popupMenu").popup("open", { x: event.x, y: y_distance });
    }
};


/*
TelemDisplay.mousedown = function() {
  console.log( "Handler for .mousedown() called." );
};
*/
TelemDisplay.prototype.refresh = function (val) {
    currentVal = val;

    var state = this.node.getAttribute("data-state");
    if (state != null && state != undefined) {
        if (state == "disabled") {
            return;
        }
    }

    if (val == "reset_display") val = "---";
    if (!isNaN(val)) {
        val = parseFloat(val);
        var decimals = this.node.getAttribute("data-decimals");
        if (decimals != null && decimals != undefined) {
            val = val.toFixed(decimals);
        }
    }
    if (this.node.querySelector('.templateValue').innerHTML != val) {
        this.node.querySelector('.templateValue').innerHTML = val;
    }
};

TelemDisplay.prototype.refreshAlarm = function(val) {
    if (val == "DISABLED" || val == "0") {
        this.node.querySelector('.templateLabel').style.color = "orange";
        this.node.setAttribute("data-alarmstate", "disabled");
    } else if (val == "ENABLED" || val == "1") {
        this.node.querySelector('.templateLabel').style.color = "#FFFFFF";
        this.node.setAttribute("data-alarmstate", "enabled");
    }
};

TelemDisplay.prototype.refreshMode = function(val) {
    this.node.setAttribute('data-mode', val);
	//if (this.node.querySelector('.templateValue').innerHTML != val)
	//	this.node.querySelector('.templateValue').innerHTML = val; 
};

TelemDisplay.prototype.limit = function(newVal) 
{
	if ((newVal == "RED_LIMIT_HIGH") || (newVal == "RED_LIMIT_LOW"))
	{
		this.node.querySelector('.templateValue').style.background = "#ff0000";
		this.node.querySelector('.templateValue').style.color = "#2e2e2e";
	}
	else if ((newVal == "YELLOW_LIMIT_HIGH") || (newVal == "YELLOW_LIMIT_LOW"))
	{
		this.node.querySelector('.templateValue').style.background = "#ffff00";
		this.node.querySelector('.templateValue').style.color = "#2e2e2e";
	}
	else
	{
		this.node.querySelector('.templateValue').style.background = "#2e2e2e";
		this.node.querySelector('.templateValue').style.color = "#f9f9f9";
	}
};

TelemDisplay.prototype.name = function (name, replacement) {
    var replacing = this.node.querySelector('.templateLabel');
    if (replacing != null && replacing != undefined) {
        this.node.setAttribute("data-label", replacement);
        replacing.innerHTML = replacement;
    }
};
TelemDisplay.prototype.units = function (val) {
    var units = this.node.querySelector('.templateUnits');
    if (units != null && units != undefined) {
        units.innerHTML = val;
    }
};
// -----  End Telemetry Display --------------------------------------------------------------

LedBarDisplay = function (component) {
    var node;
    var type;
    var auto;
    var limitActive = false;
    var obj = this;
    obj.node = component;

    var tempAuto = obj.node.getAttribute("data-led-type");
    if (tempAuto != null) {
        obj.auto = tempAuto;
    }
    else
        obj.auto = true;

    var tempAuto = obj.node.getAttribute("data-led-type");

    if (obj.node.getAttribute("data-state") == null)
        obj.node.setAttribute("data-state", "disabled");

    var tempType = obj.node.getAttribute("data-led-type");
    if (tempType != null) { obj.type = tempType.toLowerCase(); }
    else { obj.type = "ledbar"; }

    if ((obj.node.getAttribute("data-device") != null) && (obj.node.getAttribute("data-sensor") != null)) {
        if (!checkTelemetryHandlerForElement(obj.node)) {
            registerObject(createTelemObject(obj, obj.node.getAttribute("data-device"), obj.node.getAttribute("data-sensor"), updateCustom, updateCustomLimit, updateCustomNoData));
        }
    }
};

LedBarDisplay.prototype.refresh = function (val) {
    var value = val.toLowerCase();
    this.node.value = value;
    var prevValue = this.node.getAttribute("data-state");
    if (this.type == "ledbar") {
        if ((value == "1") || (value == "true") || (value == "on")) {
            if (this.node.getAttribute("data-state") != "green")
                this.node.setAttribute("data-state", "green");
        }
        else if ((value == "0") || (value == "false") || (value == "off") || (value == "t_off") || (value == "toff")) {
            if (this.node.getAttribute("data-state") != "off") {
                this.node.setAttribute("data-state", "off");
            }
        }
        else if ((value == "t_on") || (value == "ton")){
            if (this.node.getAttribute("data-state") != "yellow")
                this.node.setAttribute("data-state", "yellow");
        }
        else if (value == "---") {
            if (this.node.getAttribute("data-state") != "disabled")
                this.node.setAttribute("data-state", "disabled");
        }
        else {
            if (this.node.getAttribute("data-state") != "disabled")
                this.node.setAttribute("data-state", "disabled");
        }
    }
    else if (this.type == "module") {
        if (value == "---") {
            if (this.node.getAttribute("data-state") != "off")
                this.node.setAttribute("data-state", "off");
        }
        else if ((!this.limitActive) && (value != "reset_display")) {
            if (this.node.getAttribute("data-state") != "green")
                this.node.setAttribute("data-state", "green");
        }
        else if (value == "reset_display") {
            if (this.node.getAttribute("data-state") != "disabled")
                this.node.setAttribute("data-state", "disabled");
        }
    }
    /*
    if (this.node.getAttribute('data-updatebtn') != null && this.node.getAttribute('data-updatebtn') != undefined && prevValue != this.node.getAttribute("data-state")) {
        var btn_id = this.node.getAttribute('data-updatebtn');
        if (btn_id != null && btn_id != undefined) {
            var x = this.node.getAttribute('data-state');
            if (this.node.getAttribute('data-state') == 'off' || this.node.getAttribute('data-state') == 'disabled') {
                document.getElementById(btn_id).setAttribute('disabled', 'disabled');
            } else {
                document.getElementById(btn_id).removeAttribute('disabled');
            }
        }
    } 
    */
};
LedBarDisplay.prototype.getValue = function () {
    return this.node.value;
};

LedBarDisplay.prototype.limit = function (newVal) 
{
	if (this.type == "module") {
		if ((newVal == "RED_LIMIT_HIGH") || (newVal == "RED_LIMIT_LOW"))
		{
			this.limitActive = true;
			if (this.node.getAttribute("data-state") != "red")
	                this.node.setAttribute("data-state", "red");
		}
		else if ((newVal == "YELLOW_LIMIT_HIGH") || (newVal == "YELLOW_LIMIT_LOW"))
		{
			this.limitActive = true;
			if (this.node.getAttribute("data-state") != "yellow")
	                this.node.setAttribute("data-state", "yellow");
		}
		else
		{
			this.limitActive = false;
			this.refresh(this.value);
		} 
	}
};
LedBarDisplay.prototype.name = function (name, replacement) { 
    var replacing = null;
    if (replacing != null && replacing != undefined) {
        this.node.setAttribute("data-label", replacement);
        replacing.innerHTML = replacement;
    }
};
LedBarDisplay.prototype.units = function (val) { };

//----------------End LedBarDisplay-------------------------------

LedDisplay = function (component) {
    var node;
    var type;
    var auto;
    var obj = this;
    obj.node = component;

    var tempAuto = obj.node.getAttribute("data-led-auto");
    if (tempAuto != null) {
        obj.auto = tempAuto;
    }
    else
        obj.auto = true;

    if (obj.node.getAttribute("data-state") == null)
        obj.node.setAttribute("data-state", "disabled");

    var alarmIncludedFlag = (obj.node.getAttribute("data-alarmState") != null && obj.node.getAttribute("data-alarmState") != undefined);

    var tempType = obj.node.getAttribute("data-led-type");
    if (tempType != null) { obj.type = tempType.toLowerCase(); }
    else { obj.type = "led"; }
    if ((obj.node.getAttribute("data-device") != null) && (obj.node.getAttribute("data-sensor") != null)) {
        if ((obj.auto)) {
            if (alarmIncludedFlag && !checkAlarmHandlerForElement(obj.node)) {
                registerAlarmObject(createAlarmObject(obj, obj.node.getAttribute("data-device"), obj.node.getAttribute("data-sensor"), updateAlarm, updateCustomNoData, updateMode));
            } else if (!checkTelemetryHandlerForElement(obj.node)) {
                registerObject(createTelemObject(obj, obj.node.getAttribute("data-device"), obj.node.getAttribute("data-sensor"), updateCustom, updateCustomLimit, updateCustomNoData));
            }
        }
    }
};

LedDisplay.prototype.refresh = function (val) {
    value = val.toLowerCase();
    var prevValue = this.node.getAttribute("data-state");
    if (this.type == "led") {
        //		console.log("Refresh Value:" + value);
        if ((value == 1) || (value == "true") || (value == "on")) {
            if (this.node.getAttribute("data-state") != "green")
                this.node.setAttribute("data-state", "green");
        }
        else if ((value == "0") || (value == "false")) {
            if (this.node.getAttribute("data-state") != "red")
            //			if (this.node.getAttribute("data-state") != "red" && this.node.getAttribute("data-state") != "disabled")
                this.node.setAttribute("data-state", "red");
        }
        else if (value == "---" || value == "off") {
            if (this.node.getAttribute("data-state") != "off")
                this.node.setAttribute("data-state", "off");
        }
        else {
            if (this.node.getAttribute("data-state") != "disabled")
                this.node.setAttribute("data-state", "disabled");
        }
    }
    else if (this.type == "warning") {
        if ((value == "1") || (value == "true") || parseInt(val,16) != 0) {
            if (this.node.getAttribute("data-state") != "yellow")
                this.node.setAttribute("data-state", "yellow");
        }
        else if ((value == "0") || (value == "false")) {
            if (this.node.getAttribute("data-state") != "off")
                this.node.setAttribute("data-state", "off");
        }
        else {
            if (this.node.getAttribute("data-state") != "disabled")
                this.node.setAttribute("data-state", "disabled");
        }
    }
    else if (this.type == "fault") {
        if ((value == "1") || (value == "true") || parseInt(val, 16) != 0) {
            if (this.node.getAttribute("data-state") != "red")
                this.node.setAttribute("data-state", "red");
        }
        else if ((value == "0") || (value == "false")) {
            if (this.node.getAttribute("data-state") != "off")
                this.node.setAttribute("data-state", "off");
        }
        else {
            if (this.node.getAttribute("data-state") != "disabled")
                this.node.setAttribute("data-state", "disabled");
        }
    }

    if (this.node.getAttribute('data-updateBtn') != null && this.node.getAttribute('data-updateBtn') != undefined && prevValue != this.node.getAttribute("data-state")) {
        var btn_ids = this.node.getAttribute('data-updateBtn').split(",");
        if (btn_ids != null && btn_ids != undefined) {
            var state = this.node.getAttribute('data-state');
            for (var i = 0; i < btn_ids.length; i++) {
                var btn = document.getElementById(btn_ids[i]);
                if (state == 'off' || state == 'disabled') {
                    btn.setAttribute('disabled', 'disabled');
                } else {
                    btn.removeAttribute('disabled');
                }
            }
        }
    }

    var associatedHigherLevel = this.node.getAttribute('data-higherLevel');
    if (associatedHigherLevel != null && associatedHigherLevel != undefined) {
        var tokens = associatedHigherLevel.split(",");
        for (var i = 0; i < tokens.length; i++) {
            var led = document.getElementById(tokens[i]);
            if (led == null || led == undefined) continue;
            var currentState = led.getAttribute("data-state");

            var type = tokens[i].substring(0, tokens[i].indexOf("-"));
            var state = 1;
            var leds = $('.' + type + "leds");
            for (var k = 0; k < leds.length; k++) {
                if (leds[k].className.indexOf("led ") != -1) {
                    state = state && (leds[k].getAttribute('data-state') == "green") ? 1 : 0;
                }
            }
            if (currentState == "green") {
                state = currentState && state;
            }
            led.setAttribute("data-state", (state == 1) ? "green" : "red");
        }
    }

    var associatedBitLevel = this.node.getAttribute('data-bit');
    if (associatedBitLevel != null && associatedBitLevel != undefined) {
        var tokens = associatedBitLevel.split(";");
        for (var i = 0; i < tokens.length; i++) {
            var hex = parseInt(val, 16);
            var state = (hex >> tokens[i]) & 1;
            this.node.setAttribute("data-state", (state == 1) ? "green" : "red");
        }
    }
};

LedDisplay.prototype.refreshAlarm = function (val) {
    value = val.toLowerCase();
    if (this.type == "led") {
        //		console.log("Refresh Value:" + value);
        if ((value == 1) || (value == "true") || (value == "on")) {
            if (this.node.getAttribute("data-state") != "green")
                this.node.setAttribute("data-state", "green");
        }
        else if ((value == "0") || (value == "false")) {
            if (this.node.getAttribute("data-state") != "red")
            //			if (this.node.getAttribute("data-state") != "red" && this.node.getAttribute("data-state") != "disabled")
                this.node.setAttribute("data-state", "red");
        }
        else if (value == "---" || value == "off") {
            if (this.node.getAttribute("data-state") != "off")
                this.node.setAttribute("data-state", "off");
        }
        else {
            if (this.node.getAttribute("data-state") != "disabled")
                this.node.setAttribute("data-state", "disabled");
        }
    }
    else if (this.type == "warning") {
        if ((value == "1") || (value == "true") || parseInt(val, 16) != 0) {
            if (this.node.getAttribute("data-state") != "yellow")
                this.node.setAttribute("data-state", "yellow");
        }
        else if ((value == "0") || (value == "false")) {
            if (this.node.getAttribute("data-state") != "off")
                this.node.setAttribute("data-state", "off");
        }
        else {
            if (this.node.getAttribute("data-state") != "disabled")
                this.node.setAttribute("data-state", "disabled");
        }
    }
    else if (this.type == "fault") {
        if ((value == "1") || (value == "true") || parseInt(val, 16) != 0) {
            if (this.node.getAttribute("data-state") != "red")
                this.node.setAttribute("data-state", "red");
        }
        else if ((value == "0") || (value == "false")) {
            if (this.node.getAttribute("data-state") != "off")
                this.node.setAttribute("data-state", "off");
        }
        else {
            if (this.node.getAttribute("data-state") != "disabled")
                this.node.setAttribute("data-state", "disabled");
        }
    }
};

LedDisplay.prototype.refreshMode = function (val) {
    this.node.setAttribute('data-mode', val);
};


LedDisplay.prototype.getValue = function() 
{
	return "";
};

LedDisplay.prototype.limit = function(newVal) 
{
};

LedDisplay.prototype.name = function(name, replacement) {
    var replacing = null;
    if (replacing != null && replacing != undefined) {
        this.node.setAttribute("data-label", replacement);
        replacing.innerHTML = replacement;
    }
};
LedDisplay.prototype.units = function(val) {};
//------------------------------------------------------------------------------------------------------------------
InverseLedDisplay = function (component) {
    var node;
    var type;
    var auto;
    var obj = this;
    obj.node = component;

    var tempAuto = obj.node.getAttribute("data-led-auto");
    if (tempAuto != null) {
        obj.auto = tempAuto;
    }
    else
        obj.auto = true;

    if (obj.node.getAttribute("data-state") == null)
        obj.node.setAttribute("data-state", "disabled");

    var alarmIncludedFlag = (obj.node.getAttribute("data-alarmState") != null && obj.node.getAttribute("data-alarmState") != undefined);

    var tempType = obj.node.getAttribute("data-led-type");
    if (tempType != null) { obj.type = tempType.toLowerCase(); }
    else { obj.type = "led"; }
    if ((obj.node.getAttribute("data-device") != null) && (obj.node.getAttribute("data-sensor") != null)) {
        if ((obj.auto)) {
            if (alarmIncludedFlag && !checkAlarmHandlerForElement(obj.node)) {
                registerAlarmObject(createAlarmObject(obj, obj.node.getAttribute("data-device"), obj.node.getAttribute("data-sensor"), updateAlarm, updateCustomNoData, updateMode));
            } else if (!checkTelemetryHandlerForElement(obj.node)) {
                registerObject(createTelemObject(obj, obj.node.getAttribute("data-device"), obj.node.getAttribute("data-sensor"), updateCustom, updateCustomLimit, updateCustomNoData));
            }
        }
    }
};

InverseLedDisplay.prototype.refresh = function (val) {
    value = val.toLowerCase();
    var prevValue = this.node.getAttribute("data-state");
        //		console.log("Refresh Value:" + value);
    if ((value == 0) || (value == "false") || (value == "off")) {
        if (this.node.getAttribute("data-state") != "green")
            this.node.setAttribute("data-state", "green");
    }
    else if (value == 1 || (value == "true") || (value == "on")) {
        if (this.node.getAttribute("data-state") != "red")
                //			if (this.node.getAttribute("data-state") != "red" && this.node.getAttribute("data-state") != "disabled")
            this.node.setAttribute("data-state", "red");
    }
};

InverseLedDisplay.prototype.refreshAlarm = function (val) {
    value = val.toLowerCase();
    if ((value == 0) || (value == "false") || (value == "off")) {
        if (this.node.getAttribute("data-state") != "green")
            this.node.setAttribute("data-state", "green");
    }
    else if (value == 1 || (value == "true") || (value == "on")) {
        if (this.node.getAttribute("data-state") != "red")
            //			if (this.node.getAttribute("data-state") != "red" && this.node.getAttribute("data-state") != "disabled")
            this.node.setAttribute("data-state", "red");
    }
};

InverseLedDisplay.prototype.refreshMode = function (val) {
    this.node.setAttribute('data-mode', val);
};


InverseLedDisplay.prototype.getValue = function () {
    return "";
};
InverseLedDisplay.prototype.limit = function (newVal) { };
InverseLedDisplay.prototype.name = function (name, replacement) {
    var replacing = null;
    if (replacing != null && replacing != undefined) {
        this.node.setAttribute("data-label", replacement);
        replacing.innerHTML = replacement;
    }
};
InverseLedDisplay.prototype.units = function (val) { };
//------------------------------------------------------------------------------------------------------------------

// Power Button display ----------------------------------------------------------------------
PowerButton = function (component, num) {
    var node = null;
    var obj = this;
    var currentVal = 1;
    var buttonID = null;
    obj.buttonID = "powerButton" + num;
    obj.node = component;

    obj.node.innerHTML += (
    	"<div class='inline-block '>" +
    //  			"<div class='selectbutton'>" +
            "<div>" +
  				"<button id= 'powerButton" + num + "' class='ui-btn ui-corner-all ui-shadow ui-btn-none height-4em width-10em' onmousedown='sendPowerCommand(\"" + obj.node.getAttribute("data-target") + "\",\"" + obj.node.getAttribute("data-output") + "\", this.id)'>" + obj.node.getAttribute("data-label") + "</button>" +
	  		"</div>" +
	  		"<div class='ledbar' data-state='disabled' data-value='---'' data-units='' data-sensor='" + obj.node.getAttribute("data-sensor") + "' data-device='" + obj.node.getAttribute("data-device") + "'>" +
  		"</div>"
   	);
   // console.log("<button id= 'powerButton" + num + "' class='ui-btn ui-corner-all ui-shadow ui-btn-none height-4em' onmousedown='sendPowerCommand(\"" + obj.node.getAttribute("data-target") + "\"," + obj.node.getAttribute("data-output") + ", this.id)'>" + obj.node.getAttribute("data-label") + "</button>");


    if ((obj.node.getAttribute("data-device") != null) && (obj.node.getAttribute("data-sensor") != null)) {
        if (!checkTelemetryHandlerForElement(obj.node)) {
            registerObject(createTelemObject(obj, obj.node.getAttribute("data-device"), obj.node.getAttribute("data-sensor"), updateCustom, updateCustomLimit, updateCustomNoData));
        }
    }
};

PowerButton.prototype.refresh = function(val) 
{
	document.getElementById(this.buttonID).setAttribute("data-state", val);
};

PowerButton.prototype.limit = function(newVal) 
{
};

PowerButton.prototype.name = function(name, replacement) {
    var replacing = null; 
    if (replacing != null && replacing != undefined) {
        this.node.setAttribute("data-label", replacement);
        replacing.innerHTML = replacement;
    }
};
PowerButton.prototype.units = function(val) { };
// -----------------------------------------------------------------------------
// Telemetry display ----------------------------------------------------------------------
tcDisplay = function (component) {
    var node = null;
    var obj = this;
    var currentVal;
    var currentAlarm;
    var currentMode;

    obj.node = component;
    var newHTML = "";
    var alarmIncludedFlag = obj.node.getAttribute("data-alarmState") != null && obj.node.getAttribute("data-alarmState") != undefined;

    if ((obj.node.getAttribute("data-label") != "") && (obj.node.getAttribute("data-label") != null)) {
        newHTML += "<span class='templateLabel'>"; //+ "</span>";
        newHTML += obj.node.getAttribute("data-label") + "</span>";
    }

    newHTML += "<div class='templateValue'>---</div>";
    if ((obj.node.getAttribute("data-units") != "") && (obj.node.getAttribute("data-units") != null)) {
        newHTML += "<span class='templateUnits'>" + obj.node.getAttribute("data-units") + "</span>";
    }
    obj.node.innerHTML += (newHTML);

    obj.node.addEventListener('mousedown', function () {
        obj.onMouseDown(event);
    }, true);
    if ((obj.node.getAttribute("data-device") != null) && (obj.node.getAttribute("data-sensor") != null)) {
        if (!checkTelemetryHandlerForElement(obj.node)) {
            registerObject(createTelemObject(obj, obj.node.getAttribute("data-device"), obj.node.getAttribute("data-sensor"), updateCustom, updateCustomLimit, updateCustomNoData));
            if (alarmIncludedFlag && !checkAlarmHandlerForElement(obj.node)) {
                registerAlarmObject(createAlarmObject(obj, obj.node.getAttribute("data-device"), obj.node.getAttribute("data-sensor"), updateAlarm, updateCustomNoData, updateMode));
            }
        }
    }
};

tcDisplay.prototype.onMouseDown = function (event) {
    if (document.getElementById('popupMenu') != null && document.getElementById('popupMenu') != undefined) {
        var popup = this.node.getAttribute('data-nopopup');
        if (popup == "true") {
            return;
        }
        $("#popupMenuHeader").html(this.node.getAttribute("data-label")).enhanceWithin();
        $("#popupMenu").attr("data-device", this.node.getAttribute("data-device"));
        $("#popupMenu").attr("data-sensor", this.node.getAttribute("data-sensor"));
        $("#popupMenu").attr("data-minValue", this.node.getAttribute("data-minValue"));
        $("#popupMenu").attr("data-maxValue", this.node.getAttribute("data-maxValue"));

        if (this.node.getAttribute('data-alarmState') == "enabled") {
            document.getElementById('popupTelemAlarm').innerHTML = "Disable Alarm";
        } else if (this.node.getAttribute('data-alarmState') == "disabled") {
            document.getElementById('popupTelemAlarm').innerHTML = "Enable Alarm";
        }
        var mode = this.node.getAttribute('data-mode');
        if (mode != null && mode != undefined) {
            if (this.node.getAttribute('data-mode') == "SW_MANUAL") {
                document.getElementById('popupMenuManual').innerHTML = "Set Automatic";
            } else {
                document.getElementById('popupMenuManual').innerHTML = "Set Manual";
            }
        }
        var y_distance = event.y;
        if (y_distance >= 0.65 * screen.height) { y_distance *= 0.8; }
        $('#popupMenu').popup();
        $("#popupMenu").popup("open", { x: event.x, y: y_distance });
    }
};


/*
tcDisplay.mousedown = function() {
  console.log( "Handler for .mousedown() called." );
};
*/
tcDisplay.prototype.refresh = function (val) {
    currentVal = val;

    var state = this.node.getAttribute("data-state");
    if (state != null && state != undefined) {
        if (state == "disabled") {
            return;
        }
    }

    if (val == "reset_display") val = "---";
    if (!isNaN(val)) {
        val = parseFloat(val);
        var decimals = this.node.getAttribute("data-decimals");
        if (decimals != null && decimals != undefined) {
            val = val.toFixed(decimals);
        }
    }
    if (this.node.querySelector('.templateValue').innerHTML != val) {
        this.node.querySelector('.templateValue').innerHTML = val;
    }
};

tcDisplay.prototype.refreshAlarm = function(val) {
    if (val == "DISABLED" || val == "0") {
        this.node.querySelector('.templateLabel').style.color = "orange";
        this.node.setAttribute("data-alarmstate", "disabled");
    } else if (val == "ENABLED" || val == "1") {
        this.node.querySelector('.templateLabel').style.color = "#FFFFFF";
        this.node.setAttribute("data-alarmstate", "enabled");
    }
};

tcDisplay.prototype.refreshMode = function(val) {
    this.node.setAttribute('data-mode', val);
    //if (this.node.querySelector('.templateValue').innerHTML != val)
    //  this.node.querySelector('.templateValue').innerHTML = val; 
};

tcDisplay.prototype.limit = function(newVal) 
{
    if ((newVal == "RED_LIMIT_HIGH") || (newVal == "RED_LIMIT_LOW"))
    {
        this.node.querySelector('.templateValue').style.background = "#ff0000";
        this.node.querySelector('.templateValue').style.color = "#2e2e2e";
    }
    else if ((newVal == "YELLOW_LIMIT_HIGH") || (newVal == "YELLOW_LIMIT_LOW"))
    {
        this.node.querySelector('.templateValue').style.background = "#ffff00";
        this.node.querySelector('.templateValue').style.color = "#2e2e2e";
    }
    else
    {
        this.node.querySelector('.templateValue').style.background = "#2e2e2e";
        this.node.querySelector('.templateValue').style.color = "#f9f9f9";
    }
};

tcDisplay.prototype.name = function (name, replacement) {
    var replacing = this.node.querySelector('.templateLabel');
    if (replacing != null && replacing != undefined) {
        this.node.setAttribute("data-label", replacement);
        replacing.innerHTML = replacement;
    }
};
tcDisplay.prototype.units = function (val) {
    var units = this.node.querySelector('.templateUnits');
    if (units != null && units != undefined) {
        units.innerHTML = val;
    }
};
// -----  End Telemetry Display --------------------------------------------------------------

(function(){
    var powerButtons = document.getElementsByClassName("powerButton");
    for(var i = 0, l = powerButtons.length; i < l; i++){
       	var tempPowerButton = new PowerButton(powerButtons[i], i);
    }
})();

// Register all Telemetry Displays
(function(){
    var displays = document.getElementsByClassName("telemDisplay");
    for(var i = 0, l = displays.length; i < l; i++){
       	var tempTelemDisp = new TelemDisplay(displays[i]);
    }
})();

// Register all Telemetry Displays
(function(){
    var displays = document.getElementsByClassName("tcDisplay");
    for(var i = 0, l = displays.length; i < l; i++){
        var tempTcDisp = new tcDisplay(displays[i]);
    }
})();

// Register Gauges 
(function () {
    var gauges =  document.getElementsByClassName("gauge");
    for (var i = 0, l = gauges.length; i < l; i++) {
        new Gauge(gauges[i]);
    }
})();

// Register all linear Meters
(function(){
    var meters = document.getElementsByClassName("metercontainer");
    for(var i = 0, l = meters.length; i < l; i++){
       	var tempMeterDisp = new LinearMeter(meters[i]);
    }
})();

// Register all vertical linear Meters
(function () {
    var meters = document.getElementsByClassName("vertmetercontainer");
    for (var i = 0, l = meters.length; i < l; i++) {
        var tempMeterDisp = new VerticalMeter(meters[i]);
    }
})();

// Register all leds
(function(){
    var leds = document.getElementsByClassName("led");
    for(var i = 0, l = leds.length; i < l; i++){
       	new LedDisplay(leds[i]);
    }
})();

// Register all inverse leds
(function () {
    var leds = document.getElementsByClassName("inverseled");
    for (var i = 0, l = leds.length; i < l; i++) {
        new InverseLedDisplay(leds[i]);
    }
})();

// Register all ledbars
(function () {
    var ledbars = document.getElementsByClassName("ledbar");
    for (var i = 0, l = ledbars.length; i < l; i++) {
        new LedBarDisplay(ledbars[i]);
    }
})();

// End Custom Elements

var RotateIcon = function(options){
    this.options = options || {};
    this.rImg = options.img || new Image();
    this.rImg.src = this.rImg.src || this.options.url || '';
    this.rImg.setAttribute('crossOrigin', 'anonymous');
    this.options.width = this.options.width || this.rImg.width || 52;
    this.options.height = this.options.height || this.rImg.height || 60;
    canvas = document.createElement("canvas");
    canvas.width = this.options.width;
    canvas.height = this.options.height;
    this.context = canvas.getContext("2d");
    this.canvas = canvas;
};
RotateIcon.makeIcon = function(url) {
    return new RotateIcon({url: url});
};
RotateIcon.prototype.setRotation = function(options){
    var canvas = this.context,
        angle = options.deg ? options.deg * Math.PI / 180:
            options.rad,
        centerX = this.options.width/2,
        centerY = this.options.height/2;

    canvas.clearRect(0, 0, this.options.width, this.options.height);
    canvas.save();
    canvas.translate(centerX, centerY);
    canvas.rotate(angle);
    canvas.translate(-centerX, -centerY);
    canvas.drawImage(this.rImg, 0, 0);
    canvas.restore();
    return this;
};
RotateIcon.prototype.getUrl = function()
{
//    return this.canvas.toDataURL('image/png');
    return this.canvas.toDataURL();
};

// img.setAttribute('crossOrigin', 'anonymous');