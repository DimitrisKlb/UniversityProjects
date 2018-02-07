<?php include_once "resources/php/db_connect.php"; ?>
<?php
	session_unset();
	session_destroy();
	header("Location: index.php");
	exit;
?>