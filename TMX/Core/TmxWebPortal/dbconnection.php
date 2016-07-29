<?php
$c_DbName = 'IVP';
$c_DbUser = 'IVP';
$c_DbPass = 'ivp';


function sqlQuery($arg_con, $arg_query, $arg_message = "")
{
	$f_results = mysqli_query($arg_con, $arg_query);
	if (mysqli_errno($arg_con) != '')
	{
		echo "<span style=\"color:#FF4400;\"><b>".$arg_message.":</b></span><br/><i>".mysqli_error($arg_con)."</i>";
		exit;
	}
	else
	{
		return $f_results;
	}
}

$con = mysqli_connect("localhost", $c_DbUser, $c_DbPass, $c_DbName);


if (mysqli_connect_errno()) { die("Failed to connect to MySQL:<br/>" . mysqli_connect_error()); }


?>



