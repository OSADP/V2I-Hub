<?php 
$page_Title = "Event Log";
$page_AccessLevel = 1;
?>
<?php include("../header.php"); ?>
<script type="text/javascript">
	
function clearEventLog()
{

	var r = confirm("Event log will be cleared.  Are you sure?");
	if (r == true) {
	    var request = $.ajax({
			url:"../api/EventLogActions.php?action=clear&key=7yhlPtDvHsIAwkJXQqNXvv82HyW7077o",
			type:"post",
			success: function (data, textStatus, jqXHR) {

			},
			error: function(jqXHR, textStatus, errorThrown)
			{

			}
		});
	} 
}
</script>


	<div id="EventsTableContainer"></div>
<?php if($_SESSION['accessLevel'] > 2) { ?>
	<input id="clearLogButton" type="button" value="Clear Log" onclick="clearEventLog();" />
<?php } ?>

	<script type="text/javascript">

		function doReload() {
		     $('#EventsTableContainer').jtable('reload');
		     timerID = setTimeout(doReload,2000);
		}

		$(document).ready(function () {

		    //Prepare jTable
			$('#EventsTableContainer').jtable({
				title: 'Events',
				paging: true,
				pageSize: 20,
            	sorting: true,
            	defaultSorting: 'timestamp DESC',
				actions: {
					listAction: '../api/EventLogActions.php?action=list&key=7yhlPtDvHsIAwkJXQqNXvv82HyW7077o',
				},
				fields: {
					id: {
						key: true,
						create: false,
						edit: false,
						list: false
					},
					logLevel: {
						title: 'Level',
						width: '10%'
					},
					source: {
						title: 'Source',
						width: '20%'
					},
					description: {
						title: 'Description',
						sorting: false,
						width: '60%'
					},
					timestamp: {
						title: 'Timestamp',
						width: '10%',
						create: false,
						edit: false
					}
				}
			});

			//Load person list from server
			$('#EventsTableContainer').jtable('load');

			doReload();
		});

	</script>
 
<?php include("../footer.php"); ?>
