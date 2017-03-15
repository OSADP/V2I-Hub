<?php

require('../dbconnectionPDO.php');
require('Authentication.php');

if($_GET["action"] == "list")
{	
	$query = "SELECT `id`, `key` as parameterName , `value`, `defaultValue` ";
	$from  = "FROM `systemConfigurationParameter` ";
	$query .= $from;
	
	// insert system name if it is not yet setup
	$where = "WHERE `key` = 'SYSTEM_NAME'";
	
	$count = $DBH->query("SELECT COUNT(*) " . $from . $where);
	$found = $count->fetchColumn();
	
	if ($found == 0)
	{
		$host = gethostname();
		$insert  = "INSERT INTO `systemConfigurationParameter` ( `key`, `value`, `defaultValue` ) ";
		$insert .= "VALUES ( 'SYSTEM_NAME', '$host', '$host' )";
		$STH = $DBH->query($insert);
	}
	
	// Obtain total record count
	$count = $DBH->query("SELECT COUNT(*) " . $from);
	$recordCount = $count->fetchColumn();
	

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

	$STH = $DBH->query($query);
	
	$STH->execute();
	
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
else if($_GET["action"] == "add")
{
	//Add a new record in database
	$id = $_GET["id"];
	
	$key = $_POST["parameterName"];
	$DBH->quote($key);
	$value = $_POST["value"];
	$DBH->quote($value);
	$defValue = $_POST["defaultValue"];
	$DBH->quote($defValue);
	
	$query = "INSERT INTO `systemConfigurationParameter` ( `key`, `value`, `defaultValue` ) ";
	$query .= "VALUES ( :key, :value, :defValue )";
	
	$STH = $DBH->prepare($query);
	$STH->bindParam(':key', $key);
	$STH->bindParam(':value', $value);
	$STH->bindParam(':defValue', $defValue);
	
	$STH->execute();
	
	$query = "SELECT * FROM `systemConfigurationParameter` WHERE `id` = LAST_INSERT_ID()";
	
	$STH = $DBH->query($query);
	$row = $STH->fetch();
	
	//Return result to jTable
	$jTableResult = array();
	$jTableResult['Result'] = "OK";
	$jTableResult['Record'] = $row;
	print json_encode($jTableResult);
}
else if($_GET["action"] == "update")
{
	//Update record in database
 	$id = $_POST["id"];
 	$DBH->quote($id);
 	$value = $_POST["value"];
 	$DBH->quote($value);

 	$query = "UPDATE `systemConfigurationParameter` SET `value` = :value WHERE `id` = :id";

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
