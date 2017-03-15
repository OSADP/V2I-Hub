<?php

require('../dbconnectionPDO.php');
require('Authentication.php');

if($_GET["action"] == "list")
{

	$id = $_GET['id'];

	$query = "SELECT `id`, `pluginId`, `key` as parameterName , `value`, `defaultValue`, `description` ";
	$from  = "FROM `pluginConfigurationParameter` ";
	$where = "WHERE `pluginId` = :pluginId";
	$query .= $from . $where;

	$count = $DBH->prepare("SELECT COUNT(*) " . $from . $where);
	$count->bindParam(':pluginId', $id, PDO::PARAM_INT);
	$count->execute();
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

	$STH = $DBH->prepare($query);

	$STH->bindParam(':pluginId', $id, PDO::PARAM_INT);

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
	$descr = $_POST["description"];
	$DBH->quote($descr);
	
	$query = "INSERT INTO `pluginConfigurationParameter` ( `pluginId`, `key`, `value`, `defaultValue`, `description` ) ";
	$query .= "VALUES ( :pluginId, :key, :value, :defValue, :descr )";
	
	$STH = $DBH->prepare($query);
	$STH->bindParam(':pluginId', $id, PDO::PARAM_INT);
	$STH->bindParam(':key', $key);
	$STH->bindParam(':value', $value);
	$STH->bindParam(':defValue', $defValue);
	$STH->bindParam(':descr', $descr);
	
	$STH->execute();
	
	$query = "SELECT * FROM `pluginConfigurationParameter` WHERE `id` = LAST_INSERT_ID()";
	
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
