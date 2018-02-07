<?php include "resources/php/db_connect.php"; ?>
<?php include "resources/php/site_info.php"; ?>
<?php
if( !isset($_SESSION["register_ok"]) ){   //No reason to be here. Redirect to home page
	header("Location: index.php");
	exit;
}
?>
<!DOCTYPE html>
<html lang="el">
<head> 
	<meta charset="utf-8">
	<title><?php echo "$site_name"; ?></title> 
	<link type="text/css" rel="stylesheet" href="general.css">
</head>
<body>
	<?php include "resources/php/header.php"; ?>
	
	<div id="main">
		<div class="header_message">
		<?php
		if($_SESSION["register_ok"] == true){
		?>
			<h1> Ο λογαριασμός σας καταχωρήθηκε επιτυχώς ! </h1> 
		<?php
		}else{
		?>	
			<h1> Η εγγραφή απέτυχε. Παρακαλούμε προσπαθήστε ξανά. </h1>
		<?php
		}
		?>	
		</div>
	</div>
	
	<?php include "resources/php/footer.php"; ?>
</body>

</html>

<?php
unset($_SESSION["register_ok"]);
?>