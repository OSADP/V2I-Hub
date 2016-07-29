<?php 
$page_Title = "TMX Installed Plugins";
$page_AccessLevel = 2;


?>
<?php include("../header.php"); ?>

<div id="PluginsTableContainer"></div>
	<script type="text/javascript">

		$(document).ready(function () {

		    //Prepare jTable
			$('#PluginsTableContainer').jtable({
				title: 'Installed Plugins',
				paging: true,
				pageSize: 20,
            			sorting: true,
            			defaultSorting: 'name',
				actions: {
					listAction: '../api/InstalledPluginActions.php?action=list&key=7yhlPtDvHsIAwkJXQqNXvv82HyW7077o',
					updateAction: '../api/InstalledPluginActions.php?action=update&key=7yhlPtDvHsIAwkJXQqNXvv82HyW7077o',
					deleteAction: '../api/InstalledPluginActions.php?action=delete&key=7yhlPtDvHsIAwkJXQqNXvv82HyW7077o'
				},
				fields: {
					id: {
						key: true,
						create: false,
						edit: false,
						list: false
					},
					name: {
						title: 'Plugin Name',
						width: '20%',
						create: false,
						edit: false,
						list: true
					},
					version: {
						title: 'Version',
						sorting: false,
						width: '10%',
						create: false,
						edit: false
					},
					enabled: {
						title: 'Enabled',
	 					type: 'checkbox',
                    				values: { '0': 'Disabled', '1': 'Enabled', '-1': 'External' },
						sorting: false,
						width: '10%',
						create: false,
					},
					maxMessageInterval: {
						title: 'Max Msg Int',
						width: '10%',
						sorting: false,
						create: false,
					},
					commandLineParameters: {
						title: 'Cmd Line Parameters',
						width: '20%',
						sorting: false,
						create: false,
					},
					Parameters: {
                        title: '',
                        width: '5%',
                        sorting: false,
                        edit: false,
                        create: false,
                        display: function (data) {
                            //Create an image that will be used to open child table
                            var $img = $('<img height ="20" src="../media/20-gear-2@2x.png" title="Config Data" />');
                            //Open child table when user clicks the image
                            $img.click(function () {
                                $('#PluginsTableContainer').jtable('openChildTable',
                                        $img.closest('tr'),
                                        {
                                            title: 'Configuration Parameters',
                                            paging: true,
                                            pageSize: 20,
                                            sorting: true,
                                            defaultSorting: 'parameterName',
                                            actions: {
                                                listAction: '../api/ConfigurationParameterActions.php?action=list&key=7yhlPtDvHsIAwkJXQqNXvv82HyW7077o&id=' + data.record.id,
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
                                                    create: false,
                                                    edit: false,
                                                    list: true
                                                },
                                                value: {
                                                    type: 'textarea',
                                                    title: 'Value',
                                                    sorting: false,
                                                    width: '15%',
                                                    create: false,
                                                },
                                                defaultValue: {
                                                    type: 'textarea',
                                                    title: 'Default Value',
                                                    sorting: false,
                                                    width: '15%',
                                                    create: false,
                                                    edit: false
                                                },
                                                description: {
                                                    type: 'textarea',
                                                    title: 'Description',
                                                    width: '50%',
                                                    sorting: false,
                                                    create: false,
                                                    edit: false
                                                }
                                            }
                                        }, function (data) { //opened handler
                                            data.childTable.jtable('load');
                                        });
                            });
                            //Return image to show on the person row
                            return $img;
                        }
                    },
                    uploadColumn: {
					    title: '',
					    sorting: false,
                        edit:false,
					    width: '5%',
					    display: function (data) {
						    if (data.record.enabled < 0)
							    return ''; 
						    
					        var $link = '<a href="uploadPlugin.php?update=1"><img height = "20" src="../media/266-upload@2x.png" title="Update"  /></a>';
					        //your other javascript/jquery methods.
					        return $link;
					    }
					},
				},
				rowInserted: function(event, data) {
					if (data.record.enabled < 0)
						data.row.find('.jtable-edit-command-button').hide();
				}
			});

			//Load person list from server
			$('#PluginsTableContainer').jtable('load');
		});

	</script>
<br/>
<a href="uploadPlugin.php?update=0"><img height = "30" src="../media/31-circle-plus@2x.png" title="Status"  /></a>

<?php include("../footer.php"); ?>
