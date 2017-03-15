<?php

require('../../../dbconnectionPDO.php');

if($_GET["action"] == "CohdaUpdate")
{
	
	$dsrc = $_POST['dsrcname'];
	$state =  $_POST['dscrstate'];
	
	if ($dsrc == "DSRC1") {
		if ($state == 'true')
		{
			$set1 = " SET IVP.pluginConfigurationParameter.value = '1' WHERE IVP.pluginConfigurationParameter.pluginId = :pluginId AND IVP.pluginConfigurationParameter.key = 'TxAntenna1'";
			$set2 = " SET IVP.pluginConfigurationParameter.value = '1' WHERE IVP.pluginConfigurationParameter.pluginId = :pluginId AND IVP.pluginConfigurationParameter.key = 'RxAntenna1'";
		} else {
			$set1 = " SET IVP.pluginConfigurationParameter.value = '0' WHERE IVP.pluginConfigurationParameter.pluginId = :pluginId AND IVP.pluginConfigurationParameter.key = 'TxAntenna1'";
			$set2 = " SET IVP.pluginConfigurationParameter.value = '0' WHERE IVP.pluginConfigurationParameter.pluginId = :pluginId AND IVP.pluginConfigurationParameter.key = 'RxAntenna1'";
		}
	} else {
		if ($state == 'true')
		{
			$set1 = " SET IVP.pluginConfigurationParameter.value = '1' WHERE IVP.pluginConfigurationParameter.pluginId = :pluginId AND IVP.pluginConfigurationParameter.key = 'TxAntenna2'";
			$set2 = " SET IVP.pluginConfigurationParameter.value = '1' WHERE IVP.pluginConfigurationParameter.pluginId = :pluginId AND IVP.pluginConfigurationParameter.key = 'RxAntenna2'";
		} else {
			$set1 = " SET IVP.pluginConfigurationParameter.value = '0' WHERE IVP.pluginConfigurationParameter.pluginId = :pluginId AND IVP.pluginConfigurationParameter.key = 'TxAntenna2'";
			$set2 = " SET IVP.pluginConfigurationParameter.value = '0' WHERE IVP.pluginConfigurationParameter.pluginId = :pluginId AND IVP.pluginConfigurationParameter.key = 'RxAntenna2'";
		}
	}
	
	$query = "SELECT id FROM `plugin` WHERE name = 'CohdaInterfacePlugin'";
	
	try {
	
		$STH = $DBH->query($query);
		$STH->execute();
		
		$recordCount = $STH->rowCount();
		
		if ($recordCount > 0)
		{
			$row = $STH->fetch();
			
			$pluginId = $row["id"];
		
			console_log($pluginId);
			$DBH->quote($pluginId);
			
			$STH = null;
			
			$query = "UPDATE IVP.pluginConfigurationParameter" . $set1;
		
			console_log($query);
		
			$DBH->beginTransaction();
			$STH = $DBH->prepare($query);
			$STH->bindParam(':pluginId', $pluginId, PDO::PARAM_INT);
			console_log($STH->execute());
		
			$query = "UPDATE IVP.pluginConfigurationParameter" . $set2;
		
			console_log($query);
			
			$STH = null;
		
			$STH = $DBH->prepare($query);
			$STH->bindParam(':pluginId', $pluginId, PDO::PARAM_INT);
			console_log($STH->execute());
			$DBH->commit();
		
		} else {
			die("could not find plugin: CohdaInterfacePlugin\n");
		}
	} catch (Exception $e) {
		echo $e->getMessage();
	}
}

function console_log( $data ){
	echo '<script>';
	echo 'console.log('. json_encode( $data ) .')';
	echo '</script>';
}

?>