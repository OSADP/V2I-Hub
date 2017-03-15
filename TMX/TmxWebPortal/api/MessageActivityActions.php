<?php

require('../dbconnectionPDO.php');
require('Authentication.php');

if($_GET["action"] == "list")
{
	$query = "SELECT `messageActivity`.`id` AS id, `messageActivity`.`count` AS count, `messageActivity`.`lastReceivedTimestamp` AS lastReceivedTimestamp, `messageActivity`.`averageInterval` AS averageInterval, `plugin`.`name` AS name, `messageType`.`type` AS type, `messageType`.`subtype` AS subtype ";
	$from  = "FROM `messageActivity` INNER JOIN `plugin` ON `messageActivity`.`pluginId` = `plugin`.`id` INNER JOIN `messageType` ON `messageActivity`.`messageTypeId` = `messageType`.`id`";

	$query .= $from;

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
?>
