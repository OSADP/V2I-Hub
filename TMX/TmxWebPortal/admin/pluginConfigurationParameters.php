<?php 
$page_Title = "Edit Plugin";
$page_AccessLevel = 2;
?>
<?php include("../header.php"); 

$id=$_POST["id"];
?>


<div id="PluginsTableContainer"></div>
	<script type="text/javascript">
		$(document).ready(function () {

		    //Prepare jTable
			$('#PluginsTableContainer').jtable({
				title: 'Configuration Parameters',
				paging: true,
				pageSize: 20,
				sorting: true,
            	defaultSorting: 'parameterName',
				actions: {
					listAction: '../api/ConfigurationParameterActions.php?action=list&id=<?php echo $id?>&key=7yhlPtDvHsIAwkJXQqNXvv82HyW7077o',
					createAction: '../api/ConfigurationParameterActions.php?action=add&id=<?php echo $id?>&key=7yhlPtDvHsIAwkJXQqNXvv82HyW7077o'
					updateAction: '../api/ConfigurationParameterActions.php?action=update&key=7yhlPtDvHsIAwkJXQqNXvv82HyW7077o'
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
						create: true,
						edit: false,
						list: true
					},
					value: {
						title: 'Value',
						sorting: false,
						width: '15%',
						create: true,
					},
					defaultValue: {
						title: 'Default Value',
						sorting: false,
						width: '15%',
						create: true,
						edit: false
					},
					description: {
						title: 'Description',
						width: '50%',
						sorting: false,
						create: true,
						edit: false
					}
				}
			});

			//Load person list from server
			$('#PluginsTableContainer').jtable('load');
		});

	</script>

<?php include("../footer.php"); ?>
