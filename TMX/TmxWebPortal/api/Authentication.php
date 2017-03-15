<?php

if(array_key_exists('key', $_GET) == TRUE)
{
	$key = $_GET['key'];

	$file = file_get_contents('./apikey.txt', FILE_USE_INCLUDE_PATH);
	$apikey = trim($file);

	if(strcmp($key,$apikey)==0)
	{

	}
	else
	{
		$jTableResult = array();
		$jTableResult['Result'] = "ERROR";
		$jTableResult['Message'] = "Invalid Key";
		print json_encode($jTableResult);
		exit;
	}
}
else
{
	$jTableResult = array();
	$jTableResult['Result'] = "ERROR";
	$jTableResult['Message'] = "Unauthorized";
	print json_encode($jTableResult);
	exit;
}


?>