<?php

require('../dbconnectionPDO.php');

require('Authentication.php');

//recursivly remove directory
function rrmdir($dir) { 
   if (is_dir($dir)) { 
     $objects = scandir($dir); 
     foreach ($objects as $object) { 
       if ($object != "." && $object != "..") { 
         if (filetype($dir."/".$object) == "dir") rrmdir($dir."/".$object); else unlink($dir."/".$object); 
       } 
     } 
     reset($objects); 
     rmdir($dir); 
   } 
 }

if($_GET["action"] == "list")
{

	$query  = "SELECT `plugin`.`id` , `plugin`.`name` , `plugin`.`description` , `plugin`.`version` , ";
	$query .= "`installedPlugin`.`pluginId` , COALESCE( `installedPlugin`.`enabled` , -1 ) AS `enabled` , ";
	$query .= "`installedPlugin`.`commandLineParameters` , `installedPlugin`.`maxMessageInterval` ";
	$from   = "FROM `plugin` LEFT JOIN `installedPlugin` ON `installedPlugin`.`pluginId` = `plugin`.`id` ";
	$where  = "WHERE `plugin`.`name` NOT LIKE 'ivpcore.%'";
	$query .= $from . $where;

	$count = $DBH->query("SELECT COUNT(*) " . $from . $where);

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
else if($_GET["action"] == "update")
{
	//Update record in database

	$id = $_POST["id"];
	$DBH->quote($id);
	$maxMessageInterval = $_POST["maxMessageInterval"];
	$DBH->quote($maxMessageInterval);
	$commandLineParameters = $_POST["commandLineParameters"];
	$DBH->quote($commandLineParameters);
	$enabled = $_POST["enabled"];
	$DBH->quote($id);

	$query = "UPDATE `installedPlugin` SET `enabled` = :enabled, `commandLineParameters` = :commandLineParameters, `maxMessageInterval` = :maxMessageInterval WHERE `pluginId` = :pluginId";

	$STH = $DBH->prepare($query);

	$STH->bindParam(':enabled', $enabled, PDO::PARAM_STR);
	$STH->bindParam(':commandLineParameters', $commandLineParameters, PDO::PARAM_STR);
	$STH->bindParam(':maxMessageInterval', $maxMessageInterval, PDO::PARAM_STR);
	$STH->bindParam(':pluginId', $id, PDO::PARAM_INT);

	$STH->execute();


	//Return result to jTable
	$jTableResult = array();
	$jTableResult['Result'] = "OK";
	print json_encode($jTableResult);
}
else if($_GET["action"] == "delete")
{
	$id = $_POST["id"];

	$DBH->quote($id);

	//Delete from database
	$query = "SELECT * FROM `installedPlugin` WHERE `pluginId` = :pluginId";

	$STH = $DBH->prepare($query);

	$STH->bindParam(':pluginId', $id, PDO::PARAM_INT);

	$STH->execute();

	$plugin = $STH->fetch(PDO::FETCH_ASSOC);

	$path = $plugin['path'];

	rrmdir($path);

	$query = "DELETE FROM `installedPlugin` WHERE pluginId = :pluginId";


	$STH = $DBH->prepare($query);

	$STH->bindParam(':pluginId', $id, PDO::PARAM_INT);

	$STH->execute();

	$query = "DELETE FROM `plugin` WHERE id = :pluginId";

	$STH = $DBH->prepare($query);

	$STH->bindParam(':pluginId', $id, PDO::PARAM_INT);

	$STH->execute();

	//Return result to jTable
	$jTableResult = array();
	$jTableResult['Result'] = "OK";
	print json_encode($jTableResult);
}
else if($_GET["action"] == "add")
{
	$name = $_POST["name"];
	$DBH->quote($name);

	$description = $_POST["description"];
	$DBH->quote($description);

	$version = $_POST["version"];
	$DBH->quote($version);

	$query = "INSERT INTO `plugin`(`name`, `description`, `version`)".
	"VALUES (:name, :description, :version)";

	$STH = $DBH->prepare($query);

	$STH->bindParam(':name', $name, PDO::PARAM_STR);
	$STH->bindParam(':description', $description, PDO::PARAM_STR);
	$STH->bindParam(':version', $version, PDO::PARAM_STR);

	$STH->execute();

	$query = "SELECT *FROM `plugin` WHERE `name` = :name";

	$STH = $DBH->prepare($query);

	$STH->bindParam(':name', $name, PDO::PARAM_STR);

	$STH->execute();

	$plugin = $STH->fetch(PDO::FETCH_ASSOC);

	$pluginId = $plugin["id"];



	$query = "INSERT INTO `installedPlugin`(`pluginId`, `path`, `exeName`, `manifestName`, `commandLineParameters`, `enabled`, `maxMessageInterval`)".
	"VALUES (:pluginId, :directory, :exeLocation, :manifest, :cmdLinParms, :enabled, :maxMsgInterval)";


	$path = $_POST["path"];
	$DBH->quote($path);

	$exeName = $_POST["exeName"];
	$DBH->quote($exeName);

	$manifestName = $_POST["manifestName"];
	$DBH->quote($manifestName);


	$cmdLineParams = $_POST["cmdLineParams"];
	$DBH->quote($cmdLineParams);

	$enabled = $_POST["enabled"];
	$DBH->quote($enabled);

	$maxMsgInterval = $_POST["maxMsgInterval"];
	$DBH->quote($maxMsgInterval);

	$STH = $DBH->prepare($query);

	$STH->bindParam(':pluginId', $pluginId, PDO::PARAM_STR);
	$STH->bindParam(':directory', $path, PDO::PARAM_STR);
	$STH->bindParam(':exeLocation', $exeName, PDO::PARAM_STR);
	$STH->bindParam(':manifest', $manifestName, PDO::PARAM_STR);
	$STH->bindParam(':cmdLinParms', $cmdLineParams, PDO::PARAM_STR);
	$STH->bindParam(':enabled', $enabled, PDO::PARAM_STR);
	$STH->bindParam(':maxMsgInterval', $maxMsgInterval, PDO::PARAM_STR);

	$STH->execute();

}
?>
