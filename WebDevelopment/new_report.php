<?php include_once "resources/php/db_connect.php"; ?>
<?php include_once "resources/php/site_info.php"; ?>
<?php
function show($errors, $code){
	if(isset($errors[$code]) ){
		echo $errors[$code];
	}
}
	
$nLat = $nLng = $nDescription = $nCategory = "";
$max_photos = 4;
$max_size_MB = 3; // Photo's max size in MB: 
$max_size = $max_size_MB*1024*1024;
$photo_exists = false;
$errors = array();

$categories_query = "SELECT * FROM category";
$categories = mysqli_query($dblink, $categories_query);

if( !isset($_SESSION["loggedIn"]) ){   //User is not logged in. Redirect to home page
	header("Location: index.php");
	exit;
}elseif( $_SERVER["REQUEST_METHOD"] == "POST" ){ 
/************************************ Validation ************************************/	
	if( empty($_POST["nLat"]) ){
		$errors["nPosition"] = "Παρακαλώ διαλέξτε την θέση στον χάρτη.";
	}else{
		$nLat = trim($_POST["nLat"]);
		$nLng = trim($_POST["nLng"]);	
	}
	if( empty($_POST["nDescription"]) ){
		$errors["nDescription"] = "Παρακαλώ γράψετε μία σύντομη περιγραφή.";
	}else{
		$nDescription = trim($_POST["nDescription"]);	
	}
	$nCategory = $_POST["nCategory"];
/******************************* Photos Validation ********************************/	
	$allowedExts = array("jpeg", "jpg", "png");
	
	for($i=0; $i<count($_FILES["nPhoto"]["name"]); $i++){
		if( file_exists($_FILES["nPhoto"]["tmp_name"][$i]) && is_uploaded_file($_FILES["nPhoto"]["tmp_name"][$i])){	
			$photo_exists = true;
			$temp = explode(".", $_FILES["nPhoto"]["name"][$i]);
			$extension = end($temp);
			if($_FILES["nPhoto"]["error"][$i] > 0){
				$errors["nPhoto"] = "Παρουσιάστηκε σφάλμα κατα το ανέβασμα φωτογραφίας. Παρακαλούμε προσπαθήστε ξανά.";
			}elseif( !(($_FILES["nPhoto"]["type"][$i] == "image/jpeg")
					||  ($_FILES["nPhoto"]["type"][$i] == "image/jpg")
					||  ($_FILES["nPhoto"]["type"][$i] == "image/pjpeg")
					||  ($_FILES["nPhoto"]["type"][$i] == "image/x-png")
					||  ($_FILES["nPhoto"]["type"][$i] == "image/png"))
					|| !(in_array( strtolower($extension), $allowedExts, TRUE))
				) 
			{
				$errors["nPhoto"] = "Επιλέχθηκε αρχείο που δεν ήταν εικόνα.";
			}elseif($_FILES["nPhoto"]["size"][$i] > $max_size){
				$errors["nPhoto"] = "Η φωτογραφίες πρέπει να είναι κάτω από " . $max_size_MB . " MB";
			}			
		}
	}
/*********************************** No Errors ***********************************/	
	if( count($errors) == 0 ){
		$nLat         = mysqli_real_escape_string($dblink, $_POST["nLat"]);
		$nLng         = mysqli_real_escape_string($dblink, $_POST["nLng"]);	
		$nDescription = mysqli_real_escape_string($dblink, $_POST["nDescription"]);
		$nCategory    = mysqli_real_escape_string($dblink, $_POST["nCategory"]);
		
		$register_query = "INSERT INTO report (user_id,category_id,lat,lng,entry_date,description)  
		VALUES (". "'".$_SESSION["user_id"]."','".$nCategory."','".$nLat."','".$nLng."',"."now()".",'".$nDescription."')"; 
		
		if( isset($_SESSION["data"]) ){
			unset($_SESSION["data"]);
		}
		if( mysqli_query($dblink, $register_query) ){
/************************* Save photo in the specified path *************************/	
			if($photo_exists){ //If there is a photo to upload
				$last_query = "SELECT report_id FROM report WHERE user_id= '" .$_SESSION["user_id"]. "'
					ORDER BY report.entry_date DESC LIMIT 1";
				$check_report_id = mysqli_query($dblink, $last_query);
				$report_id = mysqli_fetch_array($check_report_id)["report_id"];				
				$photos_path = "uploaded_photos/" .$_SESSION["user_id"]. "/" .$report_id. "/";	
				mkdir($photos_path, 0777, true) ;
				
				$photo_id = -1;
				for($i=0; $i<count($_FILES["nPhoto"]["name"]); $i++){
					if( file_exists($_FILES["nPhoto"]["tmp_name"][$i]) && is_uploaded_file($_FILES["nPhoto"]["tmp_name"][$i])){	
						$photo_id++;
						move_uploaded_file($_FILES["nPhoto"]["tmp_name"][$i], $photos_path . $photo_id . "." . $extension);
					}
				}
			}		 
			$_SESSION["new_report_ok"] = true;
		}else{
			$_SESSION["new_report_ok"] = false;				
		}
		header("Location: new_report_done.php");
		exit;
	}else{
		$_SESSION["data"] = array();
		$_SESSION["data"]["nLat"]         = $nLat;
		$_SESSION["data"]["nLng"]         = $nLng;
		$_SESSION["data"]["nCategory"]    = $nCategory;
		$_SESSION["data"]["nDescription"] = $nDescription;
		$_SESSION["data"]["errors"]       = $errors;
		$_SESSION["data"]["first_redirect"]    = 1;
		// Make a GET redirect to this page to avoid re-submission errors (from reloading, etc)
		header("Location: new_report.php", true, 303);
		exit;
	}
}elseif( isset($_SESSION["data"]) && is_array($_SESSION["data"]) ){
	if( isset($_SESSION["data"]["first_redirect"]) ){
		unset($_SESSION["data"]["first_redirect"]);
		$nLat         = $_SESSION["data"]["nLat"];
		$nLng         = $_SESSION["data"]["nLng"];
		$nCategory    = $_SESSION["data"]["nCategory"];
		$nDescription = $_SESSION["data"]["nDescription"];
		$errors       = $_SESSION["data"]["errors"];
	}else{
		unset($_SESSION["data"]);
	}	
}
?>
<!DOCTYPE html>
<html lang="el">
<head> 
	<meta charset="utf-8">
	<title><?php echo "$site_name"; ?></title> 
	<link type="text/css" rel="stylesheet" href="general.css">
	<link type="text/css" rel="stylesheet" href="forms.css">
</head>
<body>
	<?php include "resources/php/header.php"; ?>
	
	<div id="main">	
		<div class="header_message">
			<h1> Καταχώρηστε μία αναφορά </h1>
		</div>
		<div class="main_div form_box" id="new_report_box">
			<form id="new_report_form" action="new_report.php" name="new_report_form" enctype="multipart/form-data" method="post">
				<div class="form_field">
					<label for name="nCategory">
						<strong>Κατηγορία: </strong>
						<select name="nCategory" id="category">
						<?php 
						while( $row = @mysqli_fetch_assoc($categories) ){ 
						?>
							<option value="<?php echo $row["category_id"]?>" > <?php echo $row["name"]?> </option>
						<?php 
						}
						?>
						</select>
					</label>
				</div>
				<div class="form_field">
					<label for name="nDescription">
						<strong>Περιγραφή: </strong>
						<textarea cols="50" rows="18" name="nDescription" id="description" maxlength="1000" 
						placeholder="Δώστε μία σύντομη περιγραφή σχετικά με την βλάβη."><?php echo $nDescription; ?></textarea>
					</label>
					<span class="error"><?php show($errors, "nDescription"); ?></span>
				</div>
				<div class="form_field">
					<strong>Θέση: </strong>
					<p id="position_describe">Κάντε κλικ στον παρακάτω χάρτη για να καθορίσετε ακριβώς την τοποθεσία της βλάβης που θέλετε να αναφέρετε ή πατήστε "Βρες με" για να εντοπιστεί αυτόματα η θέση σας αν είναι διαθέσιμα δεδομένα από GPS. Έπειτα μπορείτε να μετακινήσετε την "καρφίτσα" για να δώσετε μια πιο λεπτομερή θέση.</p>
					<input type="button" id="find_but" value="Βρες με" onclick="geolocate()"></input>
					<input type="hidden" name="nLat" id="lat" value="<?php echo $nLat; ?>"></input>
					<input type="hidden" name="nLng" id="lng" value="<?php echo $nLng; ?>"></input>
					<span class="error"><?php show($errors, "nPosition"); ?></span>
				</div>
				<div class="form_field">
					<label for name="nPhoto">
						<strong><?php echo "Ανεβάστε μέχρι ".$max_photos." φωτογραφίες: (".$max_size_MB." MB μέγιστο)";?></strong>
						<div id="add_file">Προσθήκη φωτογραφίας</div>
<?php 
	for($i=0; $i<$max_photos; $i++){
?>
						<input type="file" name="nPhoto[]" id="<?php echo "nPhoto".$i; ?>" accept="image/*;capture=camera">						
<?php 
	}
?>
						<span class="error"><?php show($errors, "nPhoto"); ?></span>
					</label>
				</div>
				<div class="form_field">
					<input type="submit" name="submit_but" id="submit_but" value ="Ανέφερε το"></input>
				</div>
			</form>
		</div>
		<div class="main_div" id="google_map">			
		</div>
	<?php include "resources/php/footer.php"; ?>
	</div>	
	
	<script src="resources/js/init.js"></script>
	<script src="http://ajax.googleapis.com/ajax/libs/jquery/1.11.1/jquery.min.js"></script>
	<script src="https://maps.googleapis.com/maps/api/js?sensor=false"></script>
	<script src="resources/js/init_new_report.js"></script>
</body>
</html>
