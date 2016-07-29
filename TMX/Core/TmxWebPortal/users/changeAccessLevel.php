<?php 
$page_Title = "Change Access Level for User: ".$_POST['username'];
$page_AccessLevel = 3;
?>
<?php include("../header.php"); ?>

<?php

if(!isset($_POST['username']) || !isset($_POST['accessLevelStr'])) {
	header("location:users.php");
	exit;
}

if(isset($_POST['execute']))
{
	if ($_POST['execute']==1)
	{
		$newAccessLevel = getAccessLevel($_POST['accessLevelStr']);

		$query = "UPDATE `user` SET `accessLevel` = '".$newAccessLevel."' WHERE  `user`.`username` = '".$_POST['username']."';";
		sqlQuery($con, $query, "Error changing access level");
		$_SESSION['message'] = "<b>".$_POST['username']."</b>'s new access level is: <b>".$_POST['accessLevelStr']." (".$newAccessLevel.")</b>";
		header("location:users.php");
	}
}
else
{?>

	<form action="<?php echo htmlentities($_SERVER['PHP_SELF']); ?>" method="post">
		<input type="hidden" name="username" id="username" value="<?php echo($_POST['username']); ?>" />
		<input type="hidden" name="accessLevelStr" id="accessLevelStr" value="<?php echo($_POST['accessLevelStr']); ?>" />
		<input type="hidden" name="execute" id="execute" value="1" />
		<label>Are you sure you want to reset password for user <b>'<?php echo($_POST['username']); ?></b>'?.</label><br/>
		<input type="submit" value="Reset" />
	</form>

<?php }

?>

<?php include("../footer.php"); ?>
