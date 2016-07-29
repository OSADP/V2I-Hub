<?php 
$page_AccessLevel = 3;
?>
<?php include("../header.php"); ?>

<?php

if(!isset($_POST['username']) || !isset($_POST['accessLevelStr']) || mb_strlen($_POST['username']) < 2) {
	header("location:users.php");
	exit;
}

$accessLevel = getAccessLevel($_POST['accessLevelStr']);

$query = "INSERT INTO `user`(`username`, `password`, `accessLevel`) VALUES ('" . $_POST['username'] . "', '_null', '" . $accessLevel . "')";
$results = sqlQuery($con, $query, "Error creating new user");


echo "Created user ".$_POST['username'];
?>

<form id="resetPasswordForm" name="resetPasswordForm" action="resetPassword.php" method="post" >
	<input type="hidden" name="username" value="<?php echo($_POST['username']); ?>" />
	<input type="hidden" name="execute" value="1" />
	<input type="submit" value="Set Initial Password" />
</form>

<script>
	document.resetPasswordForm.submit();
</script>

<?php include("../footer.php"); ?>
