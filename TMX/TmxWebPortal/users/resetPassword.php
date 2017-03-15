<?php 
$page_Title = "Reset Password for User: ".$_POST['username'];
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
		//These are the characters that will be used for the new password
		$chars = 'abcdefghijklmnopqrstuvwxyz0123456789';
	    $count = mb_strlen($chars);

	    for ($i = 0, $newPassword = ''; $i < 8; $i++) {
	        $index = rand(0, $count - 1);
	        $newPassword .= mb_substr($chars, $index, 1);
		}

		$query = "UPDATE `user` SET `password` = '".MD5($newPassword)."' WHERE  `user`.`username` = '".$_POST['username']."';";
		sqlQuery($con, $query, "Error reseting password");
		echo "<b>".$_POST['username']."</b>'s new password is: <b>".$newPassword."</b>";
	}
}
else
{?>

	<form method="post">
		<input type="hidden" name="username" id="username" value="<?php echo($_POST['username']); ?>" />
		<input type="hidden" name="execute" id="execute" value="1" />
		<label>Are you sure you want to reset password for user <b>'<?php echo($_POST['username']); ?></b>'?.</label><br/>
		<input type="submit" value="Reset" />
	</form>

<?php }

?>

<?php include("../footer.php"); ?>
