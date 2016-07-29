var setupLibraryJsVersion = "0.0.1";


/**
*   Event Handler. Processes all incoming event messages.
*   @private
*   @param {string} msg - Incoming event message to be processed
**/
function EventHandler(msg)
{
    var tokens = msg.split(";");
    $('#eventtable').DataTable().row.add([timeStamp(), tokens[3], tokens[4]]).draw();
}

/**
*   Version Reply Handler. Processes all incoming version reply messages.
*   @private
*   @param {string} msg - Incoming version reply message to be processed
**/
function VersionReplyHandler(msg)
{
    var tokens = msg.split(";");
    var verTable = $('#versiontable').DataTable().table().body();
    var numOfRows = verTable.children.length;
    //$('#versiontable').DataTable().clear().draw();
    for (var j = 3; j < tokens.length; j=j+2) {
        var bAlreadyListed = false;
        for (var i = 0; i < numOfRows; i++) {
            var sensor = verTable.children[i].children[0];
            if (sensor.innerText == tokens[j].trim()) {
                verTable.children[i].children[1] = tokens[j + 1].trim();
                bAlreadyListed = true;
            }
        }
        if (!bAlreadyListed) {
            $('#versiontable').DataTable().row.add([tokens[j].trim(), tokens[j + 1].trim()]).draw();
        }
    }
}

$(document).ready(function(){
    var versionTable = $('#versiontable').DataTable({
        bJQueryUI: true,
        bFilter: false, 
        bInfo: false,
        bPaginate: false,
        iDisplayLength: 5,
        sDom: 't'
    });

	// Connect Event Table
	var eventTable = $('#eventtable').DataTable({
		"sPaginationType":"full_numbers",                    
		"aaSorting":[[0, "asc"]],                    
		"bFilter": false,
		"bLengthChange": false,
		"iDisplayLength": 10
	});

    // Connect Event Table
    var tcTable = $('#tctable').DataTable({
        "sPaginationType":"full_numbers",                    
        "aaSorting":[[0, "asc"]],                    
        "bFilter": false,
        "bLengthChange": false,
        "iDisplayLength": 6
    });




});
  