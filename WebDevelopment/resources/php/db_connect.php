<?php include_once "resources/php/site_info.php"; ?>
<?php
session_start();

$dbhost = "localhost";
$dbname = "web_db";
$dbuser = "damageReportSql";
$dbpass = "DAMAGEsqlreport";

$dblink = @mysqli_connect($dbhost, $dbuser, $dbpass, $dbname);

if(!$dblink) {
?>
<!DOCTYPE html>
<html lang="en">
<head> 
	<meta charset="utf-8">
	<title><?php echo "$site_name"; ?></title> 
	<link type="text/css" rel="stylesheet" href="general.css">
</head>
<body>
	<?php echo "<h1> The server is currently down </h1>"; ?>		
</body>
</html>

<?php
	exit;
}
?>