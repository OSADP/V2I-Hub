<?php

require('../dbconnectionPDO.php');
require('Authentication.php');

if($_GET["action"] == "list")
{
	$query = "SELECT * FROM `eventLog`";

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
else if($_GET["action"]=="clear")
{
	$query = "DELETE FROM `eventLog`;";
	$STH = $DBH->query($query);

	$query = "INSERT INTO `eventLog` (`source`, `description`, `logLevel`) VALUES ('Web Portal','Event Log cleared','Info');";
	$STH = $DBH->query($query);

}
?>

