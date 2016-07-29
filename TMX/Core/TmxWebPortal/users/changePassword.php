<?php 
//This form allows a user to change only their own password.

$page_Title = "Change Password";
$page_AccessLevel = 1;
?>
<?php include("../header.php"); ?>

<?php

if(!isset($_SESSION['username'])) {
	header("location:".$c_PathPrefix);
	exit;
}

if(isset($_POST['password']))
{
	//$query = "DELETE FROM `user` WHERE `user`.`username` = '".$_POST['username']."'";
	$query = "UPDATE `user` SET `password` = '".MD5($_POST['password'])."' WHERE  `user`.`username` = '".$_SESSION['username']."';";
	sqlQuery($con, $query, "Error changing password");
	$_SESSION['message'] = "Changed password for '<b>".$_SESSION['username']."</b>'";
	header("location:".$c_PathPrefix);
}
else
{?>

	<form name="changePasswordForm" action="<?php echo htmlentities($_SERVER['PHP_SELF']); ?>" method="post" onKeyPress="return checkSubmit(event);">
		<label name="message" style="color:#cc0000;font-weight:bold;"></label>
		<label for="password">Password: </label>
		<input type="password" name="password" id="password" value="" /><br/>
		<label for="password">Re-enter Password: </label>
		<input type="password" name="password2" id="password2" value="" /><br/>
		<input type="button" value="Update Password" onClick="return updatePassword();" />

		<script>
			function updatePassword()
			{
				if (document.changePasswordForm.password.value != document.changePasswordForm.password2.value)
				{
					//TODO need to place message about passwords not matching
				}
				else if (document.changePasswordForm.password.value.length < 6)
				{
					//TODO need to place message about passwords too short
				}
				else
				{
					document.changePasswordForm.submit();
				}

				return false;
			}

			function checkSubmit(e)
			{
				if(e && e.keyCode == 13)
				{
					updatePassword();
				}
			}
		</script>
	</form>

<?php }

?>

<?php include("../footer.php"); ?>