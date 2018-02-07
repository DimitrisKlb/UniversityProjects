<?php include_once "resources/php/db_connect.php"; ?>
<?php include_once "resources/php/site_info.php"; ?>
<?php

function crypt_password($password){
	$cost = 5;
	$salt = strtr(base64_encode(mcrypt_create_iv(16, MCRYPT_DEV_URANDOM)), '+', '.');
	$salt = sprintf("$2a$%02d$", $cost) . $salt; //Use the Blowfish Algorithm
	$hash = crypt($password, $salt);
	return $hash;
}
function show($errors, $code){
	if(isset($errors[$code]) ){
		echo $errors[$code];
	}
}

$nEmail = $nEmail_confirm = $nPassword = $nPassword_confirm = $nFirstname = $nLastname = $nPhone = "";
$errors = array();

if( isset($_SESSION["loggedIn"]) ){   //User is logged in. Redirect to home page
	header("Location: index.php");
	exit;
}elseif( $_SERVER["REQUEST_METHOD"] == "POST" ){ 
/************************* Email validation *************************/	
	if( empty($_POST["nEmail"]) ){
		$errors["nEmail"] = "Email is required";
	}else{
		$nEmail = trim($_POST["nEmail"]);		
	}
	if( empty($errors["nEmail"]) && !filter_var($nEmail, FILTER_VALIDATE_EMAIL) ){
		$errors["nEmail"] = "Invalid email format";
	}
	$query = "SELECT * FROM member WHERE email = '".$nEmail."'";	
	$unique = mysqli_num_rows(mysqli_query($dblink, $query));
	if( empty($errors["nEmail"]) && $unique!=0 ){
		$errors["nEmail"] = "This email belongs to another user";
	}	
	//Email Confirmation	
	if( empty($_POST["nEmail_confirm"]) ){
		$errors["nEmail_confirm"] = "Please confirm your email";
	}else{		
		$nEmail_confirm = trim($_POST["nEmail_confirm"]);
	}
	if( empty($errors["nEmail"]) && ($nEmail != $nEmail_confirm) ){
		$errors["nEmail_confirm"] = "The emails do not match";
	}		
/************************* Password validation *************************/ 	
	if( empty($_POST["nPassword"]) ){
		$errors["nPassword"] = "Password is required"; 
	}else{		
		$nPassword = trim($_POST["nPassword"]);
	}
	if( empty($errors["nPassword"]) && strlen($nPassword)<8 ){
		$errors["nPassword"] = "Password must be at least 8 characters"; 
	}		
	//Password Confirmation	
	if( empty($_POST["nPassword_confirm"]) ){
		$errors["nPassword_confirm"] = "Please confirm your password";		
	}else{		
		$nPassword_confirm = trim($_POST["nPassword_confirm"]);
	}
	if( empty($errors["nPassword"]) && ($nPassword != $nPassword_confirm) ){
		$errors["nPassword_confirm"] = "The passwords do not match";
	}	
/************************* Other fields validation *************************/	
	$nFirstname = trim($_POST["nFirstname"]);
	$nLastname = trim($_POST["nLastname"]);
	$nPhone = trim($_POST["nPhone"]);
	if( !empty($_POST["nFirstname"]) && (preg_match("/^[\w\p{L}\p{N}\p{Pd}]+$/u", $nFirstname) != 1) ){
		$errors["nFirstname"] = "Name can't contain special characters";
	}
	if( !empty($_POST["nLastname"]) && (preg_match("/^[\w\p{L}\p{N}\p{Pd}]+$/u", $nLastname) != 1) ){
		$errors["nLastname"] = "Name can't contain special characters";
	}	
	if( !empty($_POST["nPhone"]) && !preg_match("/^[0-9]+$/", $nPhone) ){
		$errors["nPhone"] = "Phones must only contain numbers";
	}
	if( empty($errors["nPhone"]) && !empty($_POST["nPhone"]) && strlen($nPhone)!= 10 ){
		$errors["nPhone"] = "Valid phone numbers are 10 numbers long";
	}	
	
	if( count($errors) == 0 ){ //All form fields are ok!
		$nEmail = mysqli_real_escape_string($dblink, $_POST["nEmail"]);
		$nPassword = mysqli_real_escape_string($dblink, $_POST["nPassword"]);
		$nPassword = crypt_password($nPassword);
		$nFirstname = mysqli_real_escape_string($dblink, $_POST["nFirstname"]);
		$nLastname = mysqli_real_escape_string($dblink, $_POST["nLastname"]);
		$nPhone = mysqli_real_escape_string($dblink, $_POST["nPhone"]);
		if($nPhone == ""){
			$nPhone = "NULL";
		}else{
			$nPhone = "'" . $nPhone . "'";
		}
		
		$register_query = "INSERT INTO member (email,password,firstname,lastname,phone)  
		VALUES (". "'".$nEmail."','".$nPassword."','".$nFirstname."','".$nLastname."',".$nPhone.")"; 
		
		if( isset($_SESSION["data"]) ){
			unset($_SESSION["data"]);
		}
		if( mysqli_query($dblink, $register_query) ){
			$_SESSION["register_ok"] = true;
		}else{
			$_SESSION["register_ok"] = false;		
		}
		header("Location: register_done.php");
		exit;
	}else{
		$_SESSION["data"] = array();
		$_SESSION["data"]["nEmail"]            = $nEmail;
		$_SESSION["data"]["nEmail_confirm"]    = $nEmail_confirm;
		$_SESSION["data"]["nPassword"]         = $nPassword;
		$_SESSION["data"]["nPassword_confirm"] = $nPassword_confirm;
		$_SESSION["data"]["nFirstname"]        = $nFirstname;
		$_SESSION["data"]["nLastname"]         = $nLastname;
		$_SESSION["data"]["nPhone"]            = $nPhone;
		$_SESSION["data"]["errors"]             = $errors;
		$_SESSION["data"]["first_redirect"]    = 1;
		// Make a GET redirect to this page to avoid re-submission errors (from reloading, etc)
		header("Location: register.php", true, 303); 
		exit;
	}
}else if( isset($_SESSION["data"]) && is_array($_SESSION["data"]) ){
	if( isset($_SESSION["data"]["first_redirect"]) ){
		unset($_SESSION["data"]["first_redirect"]);
		$nEmail            = $_SESSION["data"]["nEmail"];
		$nEmail_confirm    = $_SESSION["data"]["nEmail_confirm"];
		$nPassword         = $_SESSION["data"]["nPassword"];
		$nPassword_confirm = $_SESSION["data"]["nPassword_confirm"];
		$nFirstname        = $_SESSION["data"]["nFirstname"]; 
		$nLastname         = $_SESSION["data"]["nLastname"];
		$nPhone            = $_SESSION["data"]["nPhone"];
		$errors            = $_SESSION["data"]["errors"];
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
			<h1> Δημιουργήστε νέο λογαριασμό </h1>
		</div>
		<div class="main_div form_box" id="register_box">
			<form id="register_form" action="register.php" name="register_form" method="post">				
				<div class="form_field">
					<label for name="nEmail">
						<strong>Εισάγετε το email σας</strong>
						<input required type="text" name="nEmail" id="nEmail" value ="<?php echo $nEmail;?>" autocomplete="off"></input>
					</label>
					<span class="error"><?php show($errors, "nEmail"); ?></span>
				</div>
				<div class="form_field">
					<label for name="nEmail_confirm">
						<strong>Επιβεβαίωση email</strong>
						<input type="text" name="nEmail_confirm" id="confirm-email" value ="<?php echo $nEmail_confirm;?>" autocomplete="off"></input>
					</label>
					<span class="error"><?php show($errors, "nEmail_confirm"); ?></span>
				</div>
				<div class="form_field">
					<label for name="nPassword">
						<strong>Δώστε κωδικό</strong>
						<input required type="password" name="nPassword" id="password" value ="<?php echo $nPassword;?>"></input>
					</label>
					<span class="error"><?php show($errors, "nPassword"); ?></span>
				</div>
				<div class="form_field">
					<label for name="nPassword_confirm">
						<strong>Επιβεβαίωση κωδικού</strong>
						<input type="password" name="nPassword_confirm" id="confirm-password" value ="<?php echo $nPassword_confirm;?>"></input>
					</label>
					<span class="error"><?php show($errors, "nPassword_confirm"); ?></span>
				</div>
				<div class="form_field">
					<label for name="nFirstname">
						<strong>Όνομα</strong>
						<input type="text" name="nFirstname" id="firstname" value ="<?php echo $nFirstname;?>" autocomplete="off" maxlength="20"></input>
					</label>
					<span class="error"><?php show($errors, "nFirstname"); ?></span>
				</div>
				<div class="form_field">
					<label for name="nLastname">
						<strong>Επώνυμο</strong>
						<input type="text" name="nLastname" id="lastname" value ="<?php echo $nLastname;?>" autocomplete="off" maxlength="20"></input>
					</label>
					<span class="error"><?php show($errors, "nLastname"); ?></span>
				</div>
				<div class="form_field">
					<label for name="nPhone">
						<strong>Τηλέφωνο</strong>
						<input type="text" name="nPhone" id="phone" value ="<?php echo $nPhone;?>" autocomplete="off" maxlength="15"></input>
					</label>
					<span class="error"><?php show($errors, "nPhone"); ?></span>
				</div>			
				<div class="form_field">
					<input type="submit" name="submit" id="submit_but" value ="Εγγραφή" />
				</div>
			</form>
		</div>
	
	<?php include "resources/php/footer.php"; ?>
	</div>
	
	<script src="resources/js/init.js"></script>
</body>

</html>