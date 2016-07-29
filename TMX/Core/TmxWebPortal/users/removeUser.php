<?php 
$page_Title = "Remove User";
$page_AccessLevel = 3;
?>
<?php include("../header.php"); ?>

<?php

if(!isset($_POST['username'])) {
	header("location:users.php");
	exit;
}

if(isset($_POST['execute']))
{
	if ($_POST['execute']==1)
	{
		$query = "DELETE FROM `user` WHERE `user`.`username` = '".$_POST['username']."'";
		sqlQuery($con, $query, "Error removing user");
		$_SESSION['message'] = "Removed user '<b>".$_POST['username']."</b>'";
		header("location:users.php");
	}
}
else
{?>

	<form action="<?php echo htmlentities($_SERVER['PHP_SELF']); ?>" method="post">
		<input type="hidden" name="username" id="username" value="<?php echo($_POST['username']); ?>" />
		<input type="hidden" name="execute" id="execute" value="1" />
		<label>Are you sure you want to remove user <b>'<?php echo($_POST['username']); ?></b>'?.</label><br/>
		<input type="submit" value="Remove" />
	</form>

<?php }

?>

<?php include("../footer.php"); ?>
