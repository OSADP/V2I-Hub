<?php 
$page_Title = "Edit Plugin";
$page_AccessLevel = 2;

//require_once('FirePHPCore/FirePHP.class.php');
//$firephp = FirePHP::getInstance(true);
 
?>
<?php include("../header.php"); ?>

<?php

$update = $_GET['update'];

//$firephp->log($update, 'update');

if($update == "0")
{
?>


<h2>Add New Plugin</h2>

<form action="addPlugin.php" method="post" enctype="multipart/form-data">

File: <input type="file" name="zip_file" id="zip_file"><br/>
	<br/>
	
	<input type="submit" value="Add" /><br/>
</form>
<?php
}

else
{
?>
<h2>Update Plugin</h2>

<form action="updatePlugin.php" method="post" enctype="multipart/form-data">

File: <input type="file" name="zip_file" id="zip_file"><br/>
	<br/>
	
	<input type="submit" value="Update" /><br/>
</form>
<?php
}
?>
<?php include("../footer.php"); ?>
