<?
session_start();

if (isset($_POST['message'])) {$_SESSION['message'] = $_POST['message'];}
?>