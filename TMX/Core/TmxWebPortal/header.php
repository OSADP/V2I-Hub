<!DOCTYPE HTML>
<?
session_start();
$c_PathPrefix = '/tmx';
$c_BattelleUser = "ivpadmin";
$c_BattellePass = "bcfc7791fbdf1acc8348e5edae9217a6";

require('classes/LoginFormKey.class.php');
require('dbconnection.php');
$loginFormKey = new LoginFormKey();

function getAccessLevelString($arg_AccessLevel)
{
	switch ($arg_AccessLevel) {
		case 0:
			return "Guest";
		case 1:
			return "View Only";
		case 2:
			return "Application Administrator";
		case 3:
			return "System Administrator";
		default:
			return "Unknown Access Level";
	}
}

function getAccessLevel($arg_AccessLevelStr)
{
	switch ($arg_AccessLevelStr) {
		case "View Only":
			return 1;
		case "Application Administrator":
			return 2;
		case "System Administrator":
			return 3;
		case "Guest":
		default:
			return 0;
	}
}

if (!isset($page_AccessLevel)) { $page_AccessLevel = 0; }
if (!isset($page_Title)) { $page_Title = "TMX Administration Portal"; }
if (!isset($_SESSION['accessLevel'])) { $_SESSION['accessLevel'] = 0; }

//Validate the form key and check login
if(isset($_POST['_login_formKey']) && $loginFormKey->validate())
{
	if ($_POST['_login_username'] == $c_BattelleUser)
	{
		if($_POST['_login_password'] == md5($_POST['_login_formKey'] . $c_BattellePass))
		{
			$_SESSION['username'] = $_POST['_login_username'];
			$_SESSION['accessLevel'] = 3;
		}
		else
		{
			$loginError = "Incorrect Login";
		}
	}
	else
	{
		$query = "SELECT * FROM `user`";
		$result = mysqli_query($con, $query);
		if (mysqli_errno($con) != '')
		{
			$loginError = "Sql Query Error: ".mysqli_error($con);
		}
		else
		{
			while($row = mysqli_fetch_array($result)) {

				if ($_POST['_login_username'] == $row['username'])
				{
					if($_POST['_login_password'] == md5($_POST['_login_formKey'] . $row['password']))
					{
						$_SESSION['username'] = $_POST['_login_username'];
						$_SESSION['accessLevel'] = $row['accessLevel'];
					}
					else
					{
						$loginError = "Incorrect Login";
					}

					break;
				}
			}
		}
	}
}

?>
<html>
<head>
	<link href="<?php echo($c_PathPrefix) ?>/style.css" rel="stylesheet" type="text/css" />
	<link href="<?php echo($c_PathPrefix) ?>/favicon.ico" rel="icon" type="image/x-icon" />
	<link href="<?php echo($c_PathPrefix) ?>/js/jtable/themes/metro/blue/jtable.css" rel="stylesheet" type="text/css" />

	<link href="<?php echo($c_PathPrefix) ?>/js/themes/redmond/jquery-ui-1.8.16.custom.css" rel="stylesheet" type="text/css" />


	<script src="<?php echo($c_PathPrefix) ?>/js/jquery-1.6.4.min.js" type="text/javascript"></script>
    <script src="<?php echo($c_PathPrefix) ?>/js/jquery-ui-1.8.16.custom.min.js" type="text/javascript"></script>

    <script src="<?php echo($c_PathPrefix) ?>/js/jtable/jquery.jtable.js" type="text/javascript"></script>
	
	<title> <?php echo($page_Title) ?> </title>
</head>

<body style="min-width:1000px;">
<div id="container">
<div id="content">

<table width="100%" style="color:#FFFFFF; margin-bottom:65px;">
	<tr>
		<td height="90px" valign="bottom"><h1 style="margin-bottom:0px; margin-top:10px"><?php echo($page_Title) ?></h1></td>
		<td align="right" valign="top">

		<?php if(isset($_SESSION['username'])) { ?>
			<form action="<?php echo($c_PathPrefix) ?>/logout.php" method="post" name="logoutForm">
				<label for="username">Logged in as: <?php echo("<b>" . $_SESSION['username'] . "</b> (" . getAccessLevelString($_SESSION['accessLevel']) . ")") ?></b></label>
				<input type="submit" value="Log Out"/>
			</form>
		<?php } else { ?>
			<noscript><div style="margin-bottom:10px;"><span style="color:#ff4400;font-weight:bold;">WARNING:</span> Javascript required but not available.</div></noscript>
			<?php if($loginError) { echo($loginError); } ?>
			<form action="<?php echo htmlentities($_SERVER['PHP_SELF']); ?>" method="post" name="loginForm" onKeyPress="return checkSubmit(event)">
				<?php $loginFormKey->outputKey(); ?>
				<label for="username">Username:</label>
				<input type="text" name="_login_username" id="_login_username" value="<?php echo($_POST['_login_username']); ?>" onclick="this.select()" style="width:160;" />
				<label for="password">Password:</label>
				<input type="password" name="_login_password" id="_login_password" onclick="this.select()" style="width:160;" />
				<input type="button" value="Submit" onClick="return login();"/>
				
				<script src="<?php echo($c_PathPrefix) ?>/js/md5.js"></script>
				<script>
					function login()
					{
						document.loginForm._login_password.value = CryptoJS.MD5(document.loginForm._login_formKey.value + CryptoJS.MD5(document.loginForm._login_password.value));
						document.loginForm.submit();

						return false;
					}

					function checkSubmit(e)
					{
						if(e && e.keyCode == 13)
						{
							login();
						}
					}
				</script>
			</form>
		<?php } ?></td>
	</tr>
</table>

<script>
	document.loginForm._login_username.click();
</script>

<?PHP
 
if($page_AccessLevel > $_SESSION['accessLevel'])
{
	echo ("<h2>Access Denied</h2>");
	echo ("Sorry, you are not authorized to access this page.");
	include("footer.php");

    exit;
}
?>


<!--This is the table that has the menu on left and content area on the right-->
<table border="0" cellspacing=5 cellpadding=5 width=100%><tr>
<td valign=top width=225>

<!--Main Menu-->
<?php if($_SESSION['accessLevel'] > 0) { ?>
<table class="menu" align="center" width="100%">
<tr><th>Main Menu</th></tr>
<tr><td>+ <a href="<?php echo($c_PathPrefix) ?>/index.php">Home</a></td></tr>
<tr><td>+ <a href="<?php echo($c_PathPrefix) ?>/users/changePassword.php">Change Password</a></td></tr>
<tr><td>+ <a href="<?php echo($c_PathPrefix) ?>/status/systemStatus.php">View System Status</a></td></tr>
<tr><td>+ <a href="<?php echo($c_PathPrefix) ?>/status/eventLog.php">View Event Log</a></td></tr>
<tr><td>+ <a href="<?php echo($c_PathPrefix) ?>/status/messageActivity.php">View Message Activity</a></td></tr>
</table>
<?php } ?>

<!--System Admin Menu-->
<?php if($_SESSION['accessLevel'] >= 2) { ?>
<br/>
<table class="menu" align="center" width="100%" >
<tr><th>System Admin Menu</th></tr>
<?php } ?>
<?php if($_SESSION['accessLevel'] >= 3) { ?>
<tr><td>+ <a href="<?php echo($c_PathPrefix) ?>/users/users.php">Manage Users</a></td></tr>
<?php } ?>
<?php if($_SESSION['accessLevel'] >= 2) { ?>
<tr><td>+ <a href="<?php echo($c_PathPrefix) ?>/admin/plugins.php">Installed Plugins</a></td></tr>
</table>
<?php } ?>

</td><td valign="top" style="padding:5px; padding-left:20px;">

<?php
if ($_SESSION['message'] != '')
{
	echo "<div id=messageBox>".$_SESSION['message']."</div>";
	$_SESSION['message'] = '';
}
?>


