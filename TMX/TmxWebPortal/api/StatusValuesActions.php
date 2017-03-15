<?php

require('../dbconnectionPDO.php');
require('Authentication.php');

if($_GET["action"] == "list")
{

	$id = $_GET['id'];

	$query = "SELECT `id`, `pluginId`, `key` AS statusKey, `value` ";
	$from  = "FROM `pluginStatus` ";
	$where = "WHERE `pluginId` = :pluginId AND `key` != ''";
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

?>
