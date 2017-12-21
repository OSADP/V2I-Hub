"use strict";
var componentLibraryJsVersion = "0.0.2";

function ToggleCollapsible(target) {
    if (target != null && target != undefined) {
        if (target.indexOf("configuration_") != -1) {
            if ($('div[id="' + target + '"]').css("display") != "none") {
                $('[id=\"configurationAddNewBtn_' + target.replace("configuration_", "") + "\"]").css("display", "none");
            } else {
                var btn = $('[id=\"configurationAddNewBtn_' + target.replace("configuration_", "") + "\"]").css("display", "");
            }
        }
        $('div[id="' + target + '"]').slideToggle();
    }
}

function SendEnableCommand(target) {
    generateAndSendCommandMessage($("button.enableButton[data-target=\"" + target + "\"]").html().toLowerCase(), [{ name: "plugin", value: target }]);
}

function AddNewConfigurationItem(target) {
    var keyInput = $(".newConfigInput[data-plugin=\"" + target + "\"][data-type=\"Key\"]");
    var key = keyInput.val();
    var valueInput = $(".newConfigInput[data-plugin=\"" + target + "\"][data-type=\"Value\"]");
    var value = valueInput.val();
    var defaultValueInput = $(".newConfigInput[data-plugin=\"" + target + "\"][data-type=\"Default Value\"]");
    var defaultValue = defaultValueInput.val();
    var descriptionInput = $(".newConfigInput[data-plugin=\"" + target + "\"][data-type=\"Description\"]");
    var description = descriptionInput.val();
    generateAndSendCommandMessage("set", [ { name: "plugin", value: target }, { name: "key", value: key }, { name: "value", value: value }, { name: "defaultValue", value: defaultValue }, { name: "description", value: description }]);
    keyInput.val("");
    valueInput.val("");
    defaultValueInput.val("");
    descriptionInput.val("");
}

function OpenNewInputs(btn, target) {
    if (btn.innerHTML == "+") {
        btn.innerHTML = "-";
        $(".newConfigDiv[data-plugin=\"" + target + "\"]").css("display", "");
    } else {
        btn.innerHTML = "+";
        $(".newConfigDiv[data-plugin=\"" + target + "\"]").css("display", "none");
    }
}

var PluginDisplay = function (component, json) {
    var obj = this;
    obj.node = component;

    var target = obj.node.getAttribute("data-target");
    var info = "";

    // Parse Plugin Information
    var object = json;
    var jsonPosArray = [];
    var pos = 0;
    var info = "";
    var prefix = "";

    var headerInfoStatus = null;
    var headerInfoVersion = null;
    var headerInfoDescription = null;

    while (typeof (object) == "object") {
        var items = Object.keys(object);
        for (var i = pos; i < items.length; i++) {
            if (typeof (object[items[i]]) == "object") {
                jsonPosArray.push({ obj: object, pos: i + 1, prefix: prefix });
                prefix += items[i] + "->";
                object = object[items[i]];
                items = Object.keys(object);
                pos = 0;
                i = 0;
                break;
            } else {
                info +=  "<tr><td>" + prefix + items[i] + "</td><td>" +  object[items[i]] + "</td></tr>";

                //Set the enabled status of the header.
                if (items[i].toUpperCase() == "ENABLED") {
                    headerInfoStatus = object[items[i]];
                } else if (items[i].toUpperCase() == "VERSION") {
                    headerInfoVersion = object[items[i]];
                } else if (items[i].toUpperCase() == "DESCRIPTION") {
                    headerInfoDescription = object[items[i]];
                }
            }
        }

        if (object == json) {
            break;
        }

        if (i >= items.length) {
//            info += "</div>";
            var infoArray = jsonPosArray.pop();
            object = infoArray.obj;
            pos = infoArray.pos;
            prefix = infoArray.prefix.replace(new RegExp(items[i] + "->"), "");
        }
    }

    if (permissions == 1) {
        obj.node.innerHTML = "<div id='pluginHeader_" + target + "' class='collapsibleHeader' data-target='" + target + "' onmousedown=\"ToggleCollapsible('" + target + "')\">"
//                            + "<img id='errorImage' class='errorImage' src='../images/Common/error.jpg' />"
                            + "<div class='led pluginState'></div><div class='pluginName'>" + target + "</div>" + "<div class='headerVersion'></div>"
                            + "</div>";
        obj.node.innerHTML += "<div id='" + target + "' style='display: none;vertical-align:middle;'>"
                                + "<div class='pluginInfo' data-target='" + target + "'>"
                                    + "<div id='infoHeader_" + target + "' class='collapsibleHeader subHeaderItem' data-target='info_" + target + "' onmousedown=\"ToggleCollapsible('info_" + target + "')\">Plugin Information</div>"
                                    + "<div id='info_" + target + "' class='infoSection' style='display: none;'><table id='infoTable_" + target + "' class='message-table'><thead><tr><th data-type='key'>Key</th><th data-type='value'>Value</th></tr></thead><tbody>" + info + "</tbody></table></div>"
                                + "</div>"
                                + "<div class='messages'>"
                                    + "<div id='messagesHeader_" + target + "' class='collapsibleHeader subHeader' data-target='messages_" + target + "' onmousedown=\"ToggleCollapsible('messages_" + target + "')\"'>Messages</div>"
                                    + "<div id='messages_" + target + "' class='messagesSection' style='display: none;'><table id='messagesTable_" + target + "' class='message-table'><thead><tr><th data-type='type'>Type</th><th data-type='subtype'>Subtype</th><th data-type='count'>Count</th><th data-type='lastTimestamp'>Last Timestamp</th><th data-type='averageInterval'>Average Interval</th></tr></thead><tbody></tbody></table></div>"
                                + "</div>"
                                + "<div class='state'>"
                                    + "<div id='stateHeader_" + target + "' class='collapsibleHeader subHeader' data-target='state_" + target + "' onmousedown=\"ToggleCollapsible('state_" + target + "')\"'>State</div>"
                                    + "<div id='state_" + target + "' class='stateSection' style='display: none;'><table id='stateTable_" + target + "' class='message-table'><thead><tr><th data-type='key'>Key</th><th data-type='value'>Value</th></tr></thead><tbody></tbody></table></div>"
                                + "</div>"
                                + "<div class='configuration'>"
                                    + "<div id='configurationHeader_" + target + "' class='collapsibleHeader subHeader' data-target='configuration_" + target + "' onmousedown=\"ToggleCollapsible('configuration_" + target + "')\"'>Configuration</div>"
                                    + "<div id='configuration_" + target + "' class='configurationSection' style='display: none;'><table id='configsTable_" + target + "' class='message-table'><thead><tr><th data-type='key'>Key</th><th data-type='value'>Value</th><th data-type='defaultValue'>Default Value</th><th data-type='description'>Description</th></tr></thead><tbody></tbody></table>"
                                + "</div>"
                            + "</div>";
    } else if (permissions == 2 || permissions == 3) {
        obj.node.innerHTML = "<button class='enableButton' data-target='" + target + "' onmousedown=\"SendEnableCommand('" + target + "')\">Enable</button>"
                            + "<div id='pluginHeader_" + target + "' class='collapsibleHeader' data-target='" + target + "' onmousedown=\"ToggleCollapsible('" + target + "')\">"
//                            + "<img id='errorImage' class='errorImage' src='../images/Common/error.jpg' />"
                            + "<div class='led pluginState'></div><div class='pluginName'>" + target + "</div>" + "<div class='headerVersion'></div>"
                            + "</div>";
        obj.node.innerHTML += "<div id='" + target + "' style='display: none;vertical-align:middle;'>"
                                + "<div class='pluginInfo' data-target='" + target + "'>"
                                    + "<div id='infoHeader_" + target + "' class='collapsibleHeader subHeaderItem' data-target='info_" + target + "' onmousedown=\"ToggleCollapsible('info_" + target + "')\">Plugin Information</div>"
                                    + "<div id='info_" + target + "' class='infoSection' style='display: none;'><table id='infoTable_" + target + "' class='message-table'><thead><tr><th data-type='key'>Key</th><th data-type='value'>Value</th></tr></thead><tbody>" + info + "</tbody></table></div>"
                                + "</div>"
                                + "<div class='messages'>"
                                    + "<div id='messagesHeader_" + target + "' class='collapsibleHeader subHeader' data-target='messages_" + target + "' onmousedown=\"ToggleCollapsible('messages_" + target + "')\"'>Messages</div>"
                                    + "<div id='messages_" + target + "' class='messagesSection' style='display: none;'><table id='messagesTable_" + target + "' class='message-table'><thead><tr><th data-type='type'>Type</th><th data-type='subtype'>Subtype</th><th data-type='count'>Count</th><th data-type='lastTimestamp'>Last Timestamp</th><th data-type='averageInterval'>Average Interval</th></tr></thead><tbody></tbody></table></div>"
                                + "</div>"
                                + "<div class='state'>"
                                    + "<div id='stateHeader_" + target + "' class='collapsibleHeader subHeader' data-target='state_" + target + "' onmousedown=\"ToggleCollapsible('state_" + target + "')\"'>State</div>"
                                    + "<div id='state_" + target + "' class='stateSection' style='display: none;'><table id='stateTable_" + target + "' class='message-table'><thead><tr><th data-type='key'>Key</th><th data-type='value'>Value</th></tr></thead><tbody></tbody></table></div>"
                                + "</div>"
                                + "<button id='configurationAddNewBtn_" + target + "' class='configurationBtn' onmousedown='OpenNewInputs(this, \"" + target + "\")' style='display:none;'>+</button>"
                                + "<div class='configuration'>"
                                    + "<div id='configurationHeader_" + target + "' class='collapsibleHeader subHeader' data-target='configuration_" + target + "' onmousedown=\"ToggleCollapsible('configuration_" + target + "')\"'>Configuration</div>"
                                    + "<div id='configuration_" + target + "' class='configurationSection' style='display: none;'><table id='configsTable_" + target + "' class='message-table'><thead><tr><th data-type='key'>Key</th><th data-type='value'>Value</th><th data-type='defaultValue'>Default Value</th><th data-type='description'>Description</th></tr></thead><tbody></tbody></table>"
                                    + "<div class='newConfigDiv' data-plugin='" + target + "' style='display:none;'><input class='newConfigInput' data-type=\"Key\" data-plugin='" + target + "' value=''/><input class='newConfigInput' data-type=\"Value\" data-plugin='" + target + "' value=''/><input data-type=\"Default Value\" class='newConfigInput' data-plugin='" + target + "' value=''/><input class='newConfigInput' data-type=\"Description\" data-plugin='" + target + "' value=''/><button onmousedown=\"AddNewConfigurationItem('" + target + "')\">Add</button></div></div>"
                                + "</div>"
                            + "</div>";
    }

    //Set the enabled status of the header.
    if (headerInfoStatus != null) {
        RefreshHeaderStatus(target, headerInfoStatus);
    }

    if (headerInfoVersion != null) {
        $("div[id=\"pluginHeader_" + target + "\"] > .headerVersion").html(headerInfoVersion);
    }

    if (headerInfoDescription != null) {
        $("div[id=\"infoHeader_" + target + "\"]").html(headerInfoDescription);
    }
};

function RefreshItemDisplay(obj, val) {
    obj.setAttribute("data-value", val);
    obj.querySelector("[class$=\"ItemValue\"]").innerHTML = val;
}

function RefreshPluginInfoItemDisplay(obj, val) {
    RefreshItemDisplay(obj, val);
    var target = obj.getAttribute("data-plugin");
    if (target != null && target != undefined) {
        var sensor = obj.getAttribute("data-sensor");
        if (sensor.toUpperCase() == "ENABLED") {
            if (val.toUpperCase() == "DISABLED") {
                $("div[id=\"pluginHeader_" + target.replace("info_", "") + "\"]").removeClass("pluginHeaderExternal");
                $("div[id=\"pluginHeader_" + target.replace("info_", "") + "\"]").addClass("pluginHeaderDisabled");
                $("button.enableButton[data-target=\"" + target + "\"]").html("Enable");
            } else if (val.toUpperCase() == "ENABLED") {
                $("div[id=\"pluginHeader_" + target.replace("info_", "") + "\"]").removeClass("pluginHeaderDisabled");
                $("button.enableButton[data-target=\"" + target + "\"]").html("Disable");
            } else if (val.toUpperCase() == "EXTERNAL") {
                $("div[id=\"pluginHeader_" + target.replace("info_", "") + "\"]").removeClass("pluginHeaderDisabled");
                $("div[id=\"pluginHeader_" + target.replace("info_", "") + "\"]").addClass("pluginHeaderExternal");
                $("button.enableButton[data-target=\"" + target + "\"]").html("Enable");
            }
        } else if (sensor.toUpperCase() == "VERSION") {
            $("div[id=\"pluginHeader_" + target + "\"] > .headerVersion").html(val);
        } else if (sensor.toUpperCase() == "DESCRIPTION") {
            $("div[id=\"pluginHeader_" + target + "\"] > .headerDescription").html(val);
        }
    }
}

function RefreshHeaderStatus (plugin, status)
{       
    var header = $("div[id=\"pluginHeader_" + plugin + "\"]");
    var messagesHeader = $("div[id=\"messagesHeader_" + plugin + "\"]"); 
    var stateHeader = $("div[id=\"stateHeader_" + plugin + "\"]"); 
    var configurationHeader = $("div[id=\"configurationHeader_" + plugin + "\"]"); 
    if (status.toUpperCase() == "DISABLED") {
        header.removeClass("pluginHeaderExternal");
        header.addClass("pluginHeaderDisabled");
        messagesHeader.removeClass("pluginHeaderExternal");
        messagesHeader.addClass("pluginHeaderDisabled");
        stateHeader.removeClass("pluginHeaderExternal");
        stateHeader.addClass("pluginHeaderDisabled");
        configurationHeader.removeClass("pluginHeaderExternal");
        configurationHeader.addClass("pluginHeaderDisabled");
        $("button.enableButton[data-target=\"" + plugin + "\"]").html("Enable");
    } else if (status.toUpperCase() == "ENABLED") {
        header.removeClass("pluginHeaderDisabled");
        messagesHeader.removeClass("pluginHeaderDisabled");
        stateHeader.removeClass("pluginHeaderDisabled");
        configurationHeader.removeClass("pluginHeaderDisabled");
        header.addClass("pluginHeaderEnabled");
        messagesHeader.addClass("pluginHeaderEnabled");
        stateHeader.addClass("pluginHeaderEnabled");
        configurationHeader.addClass("pluginHeaderEnabled");
        $("button.enableButton[data-target=\"" + plugin + "\"]").html("Disable");
    } else if (status.toUpperCase() == "EXTERNAL") {
        header.removeClass("pluginHeaderDisabled");
        header.addClass("pluginHeaderExternal");
        messagesHeader.removeClass("pluginHeaderDisabled");
        messagesHeader.addClass("pluginHeaderExternal");
        stateHeader.removeClass("pluginHeaderDisabled");
        stateHeader.addClass("pluginHeaderExternal");
        configurationHeader.removeClass("pluginHeaderDisabled");
        configurationHeader.addClass("pluginHeaderExternal");
        $("button.enableButton[data-target=\"" + plugin + "\"]").addClass("hide");
        $("div[id=\"pluginHeader_" + plugin + "\"] > .pluginState").removeClass("led");
    }
}

function updateFilters ()
{

}

// End Custom Elements