<?php

require('../dbconnectionPDO.php');
require('Authentication.php');

$select        = "SELECT `pluginActivity`.`id` AS id, `pluginActivity`.`rPluginName` as Name, `pluginActivity`.`sPluginName` as sPlugin, `pluginActivity`.`msgType` AS type, `pluginActivity`.`msgSubtype` AS subtype, COUNT(`pluginActivity`.`msgSubtype`) AS count, ";
$handled_time  = "(`pluginActivity`.`msgHandledTimestamp`-`msgReceivedTimestamp`/1000) ";
$received_time = "(`pluginActivity`.`msgReceivedTimestamp`/1000-`pluginActivity`.`msgCreatedTimestamp`) ";
$total_time    = "(`pluginActivity`.`msgHandledTimestamp`-`pluginActivity`.`origMsgTimestamp`) ";
$group         = "`pluginActivity`.`rPluginName`,`pluginActivity`.`msgType`,`pluginActivity`.`msgSubtype`,`pluginActivity`.`sPluginName` ";
$groupby       = "GROUP BY " . $group;
$from          = "FROM `pluginActivity` ";

if($_GET["action"] == "list")
{
	$query = $select;
	$aggregate = " AVG" . $handled_time . "AS avgHandledTimestamp, AVG" . $received_time . "AS avgReceievedTimestamp, AVG" . $total_time . "AS avgTotalTimestamp, ";
	$max = " MAX" . $handled_time . "AS maxHandledTimestamp, MAX" . $received_time . "AS maxReceievedTimestamp, MAX" . $total_time . "AS maxTotalTimestamp ";
	$query .= $aggregate . $max;
	$query .= $from;
	$rPlugin = $_POST["rPlugin"];
	$DBH->quote($rPlugin);
	$type = $_POST["type"];
	$DBH->quote($type);
	$subtype = $_POST["subtype"];
	$DBH->quote($subtype);
	$sPlugin = $_POST["sPlugin"];
	$DBH->quote($sPlugin);
	
	$rPluginS = "`pluginActivity`.`rPluginName` = '" . $rPlugin . "' ";
	$typeS = "`pluginActivity`.`msgType` = '" . $type . "' ";
	$subtypeS = "`pluginActivity`.`msgSubtype` = '" . $subtype . "' ";
	$sPluginS = "`pluginActivity`.`sPluginName` = '" . $sPlugin . "' ";
//	$rPluginS = "`pluginActivity`.`rPluginName` = :rPlugin ";
//	$typeS = "`pluginActivity`.`msgType` = :type ";
//	$subtypeS = "`pluginActivity`.`msgSubtype` = :subtype ";
//	$sPluginS = "`pluginActivity`.`sPluginName` = :sPlugin ";
	
	if (empty($_POST["rPlugin"]) && empty($_POST["type"]) && empty($_POST["subtype"]) && empty($_POST["sPlugin"]))
	{
		$where = "";
	}
	else
	{
 		$where = "WHERE ";
 		if (!empty($_POST["rPlugin"]))
 		{
 			$where .= $rPluginS;
  			if (!empty($_POST["type"]) || !empty($_POST["subtype"]) || !empty($_POST["sPlugin"]))
 				$where .= "AND ";
 		}
		
		if (!empty($_POST["type"]))
		{
			$where .= $typeS;
			if (!empty($_POST["subtype"]) || !empty($_POST["sPlugin"]))
				$where .= "AND ";							
		}
		
		if (!empty($_POST["subtype"]))
		{
			$where .= $subtypeS;
			if (!empty($_POST["sPlugin"]))
				$where .= "AND ";
		}
		
		if (!empty($_POST["sPlugin"]))
		{
			$where .= $sPluginS;
		}	
		
	}
	
	$query .= $where;
	$query .= $groupby;	
	
	$count = $DBH->query("SELECT COUNT(DISTINCT " .$group . " ) FROM `pluginActivity`");
	
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
//	$STH->bindParam(':rPlugin', $rPlugin, PDO::PARAM_STR);
//	$STH->bindParam(':type',    $type,    PDO::PARAM_STR);
//	$STH->bindParam(':subtype', $subtype, PDO::PARAM_STR);
//	$STH->bindParam(':sPlugin', $sPlugin, PDO::PARAM_STR);
	
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
else if($_GET["action"] == "listAvg")
{
	$query = $select;
//		$aggregate = " AVG(`pluginActivity`.`msgHandledTimestamp`-`msgReceivedTimestamp`/1000) AS avgHandledTimestamp, AVG(`pluginActivity`.`msgReceivedTimestamp`/1000-`pluginActivity`.`msgCreatedTimestamp`) AS avgReceievedTimestamp ";
	$aggregate = " AVG" . $handled_time . "AS avgHandledTimestamp, AVG" . $received_time . "AS avgReceievedTimestamp, AVG" . $total_time . "AS avgTotalTimestamp ";
	$query .= $aggregate;
	$query .= $from;
		
	$count = $DBH->query("SELECT COUNT(DISTINCT " .$group . " ) FROM `pluginActivity`");
		
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
else if($_GET["action"] == "listMax")	
{
	$query = $select;
//		$aggregate = " MAX(`pluginActivity`.`msgHandledTimestamp`-`msgReceivedTimestamp`/1000) AS maxHandledTimestamp, MAX(`pluginActivity`.`msgReceivedTimestamp`/1000-`pluginActivity`.`msgCreatedTimestamp`) AS maxReceievedTimestamp ";
	$aggregate = " MAX" . $handled_time . "AS maxHandledTimestamp, MAX" . $received_time . "AS maxReceievedTimestamp, MAX" . $total_time . "AS maxTotalTimestamp ";
	$query .= $aggregate;
	$query .= $from;

	$count = $DBH->query("SELECT COUNT(DISTINCT " .$group . " ) FROM `pluginActivity`");

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
else if($_GET["action"]=="clear")
{
	$query = "DELETE FROM `pluginActivity`;";
	$STH = $DBH->query($query);

//	$query = "INSERT INTO `pluginActivity` (`source`, `description`, `logLevel`) VALUES ('Web Portal','Event Log cleared','Info');";
//	$STH = $DBH->query($query);

}

?>
