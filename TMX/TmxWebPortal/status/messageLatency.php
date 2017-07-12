<?php
$page_Title =  "TMX Message Latency Analysis";
$page_AccessLevel = 1;
?>
<?php include("../header.php"); ?>

<script type="text/javascript">
	
function clearLatencyLog()
{

	var r = confirm("Latency log will be cleared.  Are you sure?");
	if (r == true) {
	    var request = $.ajax({
			url:"../api/MessageLatencyActions.php?action=clear&key=7yhlPtDvHsIAwkJXQqNXvv82HyW7077o",
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

<div class="filtering">
    <form>
        Destination Plugin Name: <input type="text" name="rPlugin" id="rPlugin" />
        Msg. Type: <input type="text" name="type" id="type" />
        Msg. Subtype: <input type="text" name="subtype" id="subtype" />
        Source Plugin Name: <input type="text" name="sPlugin" id="sPlugin" />
        <button type="submit" id="LoadRecordsButton">Load records</button>
    </form>
</div>
<p><b>Right click the column header to select columns to display</b>.</p>

<div id="MessageLoadTableContainer"></div>
<?php if($_SESSION['accessLevel'] > 2) { ?>
	<input id="clearLogButton" type="button" value="Clear Log" onclick="clearLatencyLog();" />
<?php } ?>
	<script type="text/javascript">

		function doReload() {
		     $('#MessageLoadTableContainer').jtable('reload');
		     timerID = setTimeout(doReload,2000);
		}

		$(document).ready(function () {

		    //Prepare jTable
			$('#MessageLoadTableContainer').jtable({
				title: 'Message Latency Stats',
				paging: true,
				pageSize: 20,
            	sorting: true,
            	defaultSorting: 'name',
				actions: {
					listAction: '../api/MessageLatencyActions.php?action=list&key=7yhlPtDvHsIAwkJXQqNXvv82HyW7077o'
				},
				fields: {
					id:
					{
						key: true,
						create: false,
						edit: false,
						list: false
					},
					Name: {
						title: 'Destination',
						width: '5%'
					},
					type: {
						title: 'Type',
						width: '1%'
					},
					subtype: {
						title: 'Subtype',
						width: '1%'
					},
					sPlugin: {
						title: 'Source',
						width: '5%'
					},
					count: {
						title: 'Count',
						width: '1%'
					},
					avgReceievedTimestamp: {
						title: 'Avg. Recv. Time(ms)',
						width: '10%',
						create: false,
						edit: false
					},
					avgHandledTimestamp: {
						title: 'Avg. Handle Time(ms)',
						width: '10%',
						create: false,
						edit: false
					},
					avgTotalTimestamp: {
						title: 'Avg. Total Time(ms)',
						width: '10%',
						create: false,
						edit: false
					},
					maxReceievedTimestamp: {
						title: 'Max Recv. Time(ms)',
						width: '10%',
						create: false,
						edit: false
					},
					maxHandledTimestamp: {
						title: 'Max Handle Time(ms)',
						width: '10%',
						create: false,
						edit: false
					},
					maxTotalTimestamp: {
						title: 'Max Total Time(ms)',
						width: '10%',
						create: false,
						edit: false
					}	
				}
			});

	        //Re-load records when user click 'load records' button.
	        $('#LoadRecordsButton').click(function (e) {
	            e.preventDefault();
	            $('#MessageLoadTableContainer').jtable('load', {
	            	rPlugin: $('#rPlugin').val(),
	                type: $('#type').val(),
	                subtype: $('#subtype').val(),
	                sPlugin: $('#sPlugin').val()
	            });
	        });
	 
	        //Load all records when page is first shown
	        $('#LoadRecordsButton').click();

			doReload();
		});

	</script>

<p>	
	

<?php include("../footer.php"); ?>
