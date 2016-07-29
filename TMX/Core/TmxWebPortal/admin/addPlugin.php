<?php 
$page_Title = "Add Plugin";
$page_AccessLevel = 2;
$target_path = "/var/www/plugins/";  // change this to the correct site path

?>
<?php 
include("../header.php"); 
include("../classes/ProcessPluginZip.php");
?>

<script type="text/javascript">
	
function addPlugin(name, description, version, path, exeName, manifestName, cmdLineParams, enabled, maxMsgInterval)
{

	var formData = {'name':name, 'description': description, 'version': version, 'path': path, 'exeName': exeName, 'manifestName': 'manifest.json', 'cmdLineParams': '', 'enabled': '0', 'maxMsgInterval': '5000'};

	var request = $.ajax({
		url:"../api/InstalledPluginActions.php?action=add&key=7yhlPtDvHsIAwkJXQqNXvv82HyW7077o",
		type:"post",
		data:formData,
		success: function (data, textStatus, jqXHR) {
			$.post('../setsessionvariable.php', { message: 'Plugin Installed' });
			window.location.replace("./plugins.php");
		},
		error: function(jqXHR, textStatus, errorThrown)
		{
			$.post('../setsessionvariable.php', { message: 'Plugin Installation Failed' });
			window.location.replace("./plugins.php");
		}
	});
}
</script>

<?php

if ($_FILES["zip_file"]["error"] > 0) {
  echo "Error: " . $_FILES["zip_file"]["error"] . "<br>";
}


if($_FILES["zip_file"]["name"]) {
	$filename = $_FILES["zip_file"]["name"];
	$source = $_FILES["zip_file"]["tmp_name"];
	$type = $_FILES["zip_file"]["type"];
	
	$name = explode(".", $filename);
	$accepted_types = array('application/zip', 'application/x-zip-compressed', 'multipart/x-zip', 'application/x-compressed');
	foreach($accepted_types as $mime_type) {
		if($mime_type == $type) {
			$okay = true;
			break;
		} 
	}
	
	$continue = strtolower($name[1]) == 'zip' ? true : false;
	if(!$continue) {
		$message = "The file you are trying to upload is not a .zip file. Please try again.";
	}

	$target_file = $target_path.$filename;


	$zipProcessor = new ProcessPluginZip;

	$zipProcessor->processZipFile($target_path, $source, $target_file, $con, false);


	if($zipProcessor->message) echo "<p>".$zipProcessor->message."</p>";
}
else{
	echo "<p>no zip file</p>";
}
?> 

<?php 

if($zipProcessor->alreadyExists == TRUE)
{
	echo "<p>" . $zipProcessor->jsonObj->name ." already exists.  Vist the installed plugin page to update the plugin.</p>";
}

else if(is_null($zipProcessor->jsonObj) == FALSE) 
{ 
	echo '<script type="text/javascript">'
   , 'addPlugin("'.$zipProcessor->jsonObj->name.'","'. $zipProcessor->jsonObj->description.'","'. $zipProcessor->jsonObj->version.'","'. $zipProcessor->directory.'","'. $zipProcessor->jsonObj->exeLocation.'");'
   , '</script>';


}
?>

<?php include("../footer.php"); ?>