<?php
$c_DbName = 'IVP';
$c_DbUser = 'IVP';
$c_DbPass = 'ivp';

$DBH = null;

try {
  # MS SQL Server and Sybase with PDO_DBLIB

  # MySQL with PDO_MYSQL
	$DBH = new PDO("mysql:host=$host;dbname=$c_DbName", $c_DbUser, $c_DbPass);
	$DBH->setAttribute( PDO::ATTR_EMULATE_PREPARES, false );
	$DBH->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
}
catch(PDOException $e) {
    echo $e->getMessage();
}
?>



