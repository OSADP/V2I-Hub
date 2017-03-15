<?php
$page_Title =  "TMX System Status";
$page_AccessLevel = 1;
?>
<?php include("../header.php"); ?>

<div id="SystemStatusTableContainer"></div>
	<script type="text/javascript">

		function doReload() {
		     $('#SystemStatusTableContainer').jtable('reload');
		     timerID = setTimeout(doReload,2000);
		}

		$(document).ready(function () {

		    //Prepare jTable
			$('#SystemStatusTableContainer').jtable({
				title: 'System Status',
				paging: true,
				pageSize: 20,
            			sorting: true,
            			defaultSorting: 'name',
				actions: {
					listAction: '../api/PluginStatusActions.php?action=list&key=7yhlPtDvHsIAwkJXQqNXvv82HyW7077o'
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
						width: '20%'
					},
					description: {
						title: 'Description',
						sorting: false,
						width: '45%'
					},
					version: {
						title: 'Version',
						sorting: false,
						width: '10%'
					},
					status: {
						title: 'Status',
						width: '15%',
					},
					enabled: {
						title: 'Enabled',
	 					type: 'checkbox',
                    				values: { '0': 'Disabled', '1': 'Enabled' },
						width: '10%'
					},
					ShowDetailColumn: {
					    title: '',
					    sorting: false,
					    width: '5%',
					    display: function (data) {
					        var $link = '<a href="pluginStatus.php?id=' + data.record.id + '&name=' + data.record.name + '"><img height = "20" src="../media/77-ekg@2x.png" title="Status"  /></a>';
					        //your other javascript/jquery methods.
					        return $link;
					    }
					},
				},
			});

			//Load person list from server
			$('#SystemStatusTableContainer').jtable('load');

			doReload();
		});

	</script>

<?php include("../footer.php"); ?>
