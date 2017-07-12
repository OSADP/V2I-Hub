<?php 
$page_Title = "TMX System Details";
$page_AccessLevel = 2;
?>
<?php 
include("../header.php"); 
$id=$_POST["id"];
?>

<div id="SystemConfigTableContainer"></div>
	<script type="text/javascript">
		$(document).ready(function () {

		    //Prepare jTable
			$('#SystemConfigTableContainer').jtable({
				title: 'System Configuration Parameters',
				//Event
	            rowUpdated: function (event, data) {
		            if (data.record.parameterName == "SYSTEM_NAME") {
		            	setTitle(data.record.value);
		            }
	            },
				paging: true,
				pageSize: 20,
				sorting: true,
            	defaultSorting: 'parameterName',
				actions: {
					listAction: '../api/SystemConfigActions.php?action=list&id=<?php echo $id?>&key=7yhlPtDvHsIAwkJXQqNXvv82HyW7077o',
					createAction: '../api/SystemConfigActions.php?action=add&id=<?php echo $id?>&key=7yhlPtDvHsIAwkJXQqNXvv82HyW7077o',
					updateAction: '../api/SystemConfigActions.php?action=update&key=7yhlPtDvHsIAwkJXQqNXvv82HyW7077o'
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
					}
				}
			});

			//Load System Configuration from server
			$('#SystemConfigTableContainer').jtable('load');
		});

	</script>
<br/>

<?php
$output = shell_exec('dpkg -l |grep tmx |grep -iv plugin');
$outputs = split("[\r|\n]", trim($output));

foreach($outputs as $line) 
{
	$cells = preg_split("/\s+/", $line, 5); // install | package name | version | type | description
}
?>

<div id="SystemPackagesTableContainer">
<div class="jtable-main-container"><div style="display: none;" class="jtable-busy-panel-background"></div>
<div style="display: none;" class="jtable-busy-message"></div>
<div class="jtable-title"><div class="jtable-title-text">Installed System Packages</div></div>
<table class="jtable">
	<thead><tr>
		<th class="jtable-column-header">
			<div class="jtable-column-header-container"><span class="jtable-column-header-text">Package Name</span><div class="jtable-column-resize-handler"></div></div></th>
		<th class="jtable-column-header">
			<div class="jtable-column-header-container"><span class="jtable-column-header-text">Version</span><div class="jtable-column-resize-handler"></div></div></th>
		<th class="jtable-column-header">
			<div class="jtable-column-header-container"><span class="jtable-column-header-text">Build Type</span><div class="jtable-column-resize-handler"></div></div></th>
		<th class="jtable-column-header">
			<div class="jtable-column-header-container"><span class="jtable-column-header-text">Description</span></div></th></tr>
	</thead>
	<tbody>
	<? foreach ($outputs as $row) : ?>
		<? $cells = preg_split("/\s+/", $row, 5); // install | package name | version | type | description ?>
		
     	<tr class="jtable-data-row jtable-row">
       		<td><? echo $cells[1]; ?></td>
       		<td><? echo $cells[2]; ?></td>
       		<td><? echo $cells[3]; ?></td>
       		<td><? echo $cells[4]; ?></td>
     	</tr>
    <? endforeach; ?>
			
	</tbody>
</table>

<div class="jtable-bottom-panel">
	<div class="jtable-left-area"><span class="jtable-page-list"></span>
	</div>
	<div class="jtable-right-area"></div>
</div>
<div style="display: none;" class="jtable-column-resize-bar"></div>
<div class="jtable-column-selection-container"></div>
</div>

<br/>

<?php include("../footer.php"); ?>
