<?php
$page_Title =  "TMX Message Activity";
$page_AccessLevel = 1;
?>
<?php include("../header.php"); ?>

<div id="MessageActivityTableContainer"></div>
	<script type="text/javascript">

		function doReload() {
		     $('#MessageActivityTableContainer').jtable('reload');
		     timerID = setTimeout(doReload,2000);
		}

		$(document).ready(function () {

		    //Prepare jTable
			$('#MessageActivityTableContainer').jtable({
				title: 'Message Activity',
				paging: true,
				pageSize: 20,
            	sorting: true,
            	defaultSorting: 'name',
				actions: {
					listAction: '../api/MessageActivityActions.php?action=list&key=7yhlPtDvHsIAwkJXQqNXvv82HyW7077o'
				},
				fields: {
					id:
					{
						key: true,
						create: false,
						edit: false,
						list: false
					},
					name: {
						title: 'Plugin Name',
						width: '25%'
					},
					type: {
						title: 'Message Type',
						width: '20%'
					},
					subtype: {
						title: 'Message Subtype',
						width: '20%'
					},
					count: {
						title: 'Count',
						width: '10%'
					},
					lastReceivedTimestamp: {
						title: 'Last Timestamp',
						width: '15%',
						create: false,
						edit: false
					},
					averageInterval: {
						title: 'Average Interval',
						width: '10%'
					}
				}
			});

			//Load person list from server
			$('#MessageActivityTableContainer').jtable('load');

			doReload();
		});

	</script>

<?php include("../footer.php"); ?>
