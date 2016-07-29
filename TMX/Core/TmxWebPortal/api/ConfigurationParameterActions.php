<?php

require('../dbconnectionPDO.php');
require('Authentication.php');

if($_GET["action"] == "list")
{

	$id = $_GET['id'];

	$query = "SELECT `id`, `pluginId`, `key` as parameterName , `value`, `defaultValue`, `description` FROM `pluginConfigurationParameter` WHERE `pluginId` = :pluginId";

	if(array_key_exists('jtSorting', $_GET) == TRUE)
	{
		$orderby = $_GET["jtSorting"];
		$DBH->quote($orderby);
		$query .= " ORDER BY " . $orderby;
	}

	if(array_key_exists('jtStartIndex', $_GET) == TRUE)
	{
		$start = (int)$_GET['jtStartIndex'];
		$pagesize = (int)$_GET['jtPageSize'];
		$query .= " LIMIT " . $start .", " . $pagesize;
	}

	$STH = $DBH->prepare($query);

	$STH->bindParam(':pluginId', $id, PDO::PARAM_INT);

	$STH->execute();
	
	$recordCount = $STH->rowCount();

	//Add all records to an array
	$rows = array();
	while($row = $STH->fetch(PDO::FETCH_ASSOC)) {
    	$rows[] = $row;
	}
	 
	//Return result to jTable
	$jTableResult = array();
	$jTableResult['Result'] = "OK";
	$jTableResult['TotalRecordCount'] = $recordCount;
	$jTableResult['Records'] = $rows;
	print json_encode($jTableResult);
}
else if($_GET["action"] == "update")
{
	//Update record in database
	$id = $_POST["id"];
	$DBH->quote($id);
	$value = $_POST["value"];
	$DBH->quote($value);

	$query = "UPDATE `pluginConfigurationParameter` SET `value` = :value WHERE `id` = :id";

	$STH = $DBH->prepare($query);

	$STH->bindParam(':id', $id, PDO::PARAM_INT);
	$STH->bindParam(':value', $value, PDO::PARAM_STR);

	$STH->execute();

	//Return result to jTable
	$jTableResult = array();
	$jTableResult['Result'] = "OK";
	print json_encode($jTableResult);
}


?>
