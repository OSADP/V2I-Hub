<?php

if(isset($_GET['name'])) {
	$page_Title =  $_GET['name'] . " Status";
}
else
{
	$page_Title =  "Plugin Status";
}

$page_AccessLevel = 2;
?>
<?php include("../header.php"); ?>

<?php

if(!isset($_GET['id'])) {
	header("location:systemStatus.php");
	exit;
}
?>

<?php

$id = $_GET['id'];

$query = "SELECT * FROM `plugin` WHERE `id` = " . $id . "";

$result = sqlQuery($con, $query, "Error loading plugin");

if (!($plugin = mysqli_fetch_array($result))) {
	$_SESSION['message'] = "ERROR: Error loading plugin";
	header("location:view.php");
	exit;
}

$plugin_name = $plugin['name'];

$page_Title =  $plugin_name . " Status";

$page_Title =  "Greg";

?>

<div id="StatusValuesTableContainer"></div>
	<script type="text/javascript">

		function doReload() {
		     $('#StatusValuesTableContainer').jtable('reload');
		     $('#ConfigParametersTableContainer').jtable('reload');
		     timerID = setTimeout(doReload,2000);
		}

		$(document).ready(function () {

		    //Prepare jTable
			$('#StatusValuesTableContainer').jtable({
				title: 'Status Values',
				paging: true,
				pageSize: 10,
            			sorting: true,
            			defaultSorting: 'statusKey',
				actions: {
					listAction: '../api/StatusValuesActions.php?action=list&key=7yhlPtDvHsIAwkJXQqNXvv82HyW7077o&id=<?php echo $id?>'
				},
				fields: {
					id: {
						key: true,
						create: false,
						edit: false,
						list: false
					},
					pluginId: {
						create: false,
						edit: false,
						list: false
					},
					statusKey: {
						title: 'Key',
						width: '50%',
						create: false,
						edit: false
					},
					value: {
						title: 'Value',
						sorting: false,
						width: '50%',
						create: false,
					}
				}
			});

			//Load person list from server
			$('#StatusValuesTableContainer').jtable('load');
		});

	</script>
<br/>

<div id="ConfigParametersTableContainer"></div>
	<script type="text/javascript">
		$(document).ready(function () {

		    //Prepare jTable
			$('#ConfigParametersTableContainer').jtable({
				title: 'Configuration Parameters',
				paging: true,
				pageSize: 10,
            			sorting: true,
            			defaultSorting: 'parameterName',
				actions: {
					listAction: '../api/ConfigurationParameterActions.php?action=list&key=7yhlPtDvHsIAwkJXQqNXvv82HyW7077o&id=<?php echo $id?>'
				},
				fields: {
					id: {
						key: true,
						create: false,
						edit: false,
						list: false
					},
					parameterName: {
						title: 'Key',
						width: '20%',
						create: false,
						edit: false,
						list: true
					},
					value: {
						title: 'Value',
						sorting: false,
						width: '15%',
						create: false,
					},
					defaultValue: {
						title: 'Default Value',
						sorting: false,
						width: '15%',
						create: false,
						edit: false
					},
					description: {
						title: 'Description',
						width: '50%',
						sorting: false,
						create: false,
						edit: false
					}
				}
			});

			//Load person list from server
			$('#ConfigParametersTableContainer').jtable('load');

			doReload();
		});

	</script>




<?php include("../footer.php"); ?>
