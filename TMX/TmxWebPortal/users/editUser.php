<?php 
$page_Title = "Edit User '".$_POST['username']."'";
$page_AccessLevel = 3;
?>
<?php include("../header.php"); ?>

<?php

if(!isset($_POST['username'])) {
	header("location:users.php");
	exit;
}

$query = "SELECT * FROM `user` WHERE `username` = '".$_POST['username']."'";
$result = sqlQuery($con, $query, "Error retreiving user information");
if (!($user = mysqli_fetch_array($result))) {
	header("location:users.php");
	exit;
}


?>

<h2><?php echo($page_Title); ?></h2>


<form action="changeAccessLevel.php" method="post">
	<input type="hidden" name="username" id="username" value="<?php echo($_POST['username']); ?>" />
	<input type="hidden" name="execute" id="execute" value="1" />
	<label for="accessLevelStr">Access Level: </label>
	<select name="accessLevelStr" id="accessLevelStr">
		<option><?php echo(getAccessLevelString(1)); ?></option>
		<option><?php echo(getAccessLevelString(2)); ?></option>
		<option><?php echo(getAccessLevelString(3)); ?></option>
	</select>
	<input type="submit" value="Update" />
</form>
<hr/>
<form action="resetPassword.php" method="post">
	<input type="hidden" name="username" id="username" value="<?php echo($_POST['username']); ?>" />
	<input type="submit" value="Reset Password" />
</form>

<form action="removeUser.php" method="post">
	<input type="hidden" name="username" id="username" value="<?php echo($_POST['username']); ?>" />
	<input type="submit" value="Remove User" />
</form>


<?php include("../footer.php"); ?>
