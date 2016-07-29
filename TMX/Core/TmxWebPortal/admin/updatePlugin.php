<?php 
$page_Title = "Add Plugin";
$page_AccessLevel = 2;
$target_path = "/var/www/plugins/";  // change this to the correct site path

?>
<?php 
include("../header.php"); 
include("../classes/ProcessPluginZip.php");
?>

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

	$zipProcessor->processZipFile($target_path, $source, $target_file, $con, true);


	if($zipProcessor->message) echo "<p>".$zipProcessor->message."</p>";
}
else{
	echo "<p>no zip file</p>";
}
?> 

<?php 
	$_SESSION['message'] = "<b>".$zipProcessor->jsonObj->name."</b> has been updated.";
	header("location:plugins.php");
?>


<?php include("../footer.php"); ?>