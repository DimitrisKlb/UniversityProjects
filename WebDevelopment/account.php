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

$nEmail = $nEmail_confirm  = "";
$nOldPassword = $nPassword = $nPassword_confirm = "";
$nFirstname = $nLastname = $nPhone = "";
$errors = array();
$choice = "";
$change_message = "";

if( !isset($_SESSION["loggedIn"]) ){   //User is not logged in. Redirect to home page
	header("Location: index.php");
	exit;
}else{
	/************************* Get original Data *************************/		
	$query = "SELECT * FROM member WHERE user_id = '".$_SESSION["user_id"]."'";
	$checkUser = mysqli_query($dblink, $query);
	if(mysqli_num_rows($checkUser) == 1){	
		$userInfo   = mysqli_fetch_array($checkUser);
		$nEmail     = $userInfo["email"];
		$nFirstname = $userInfo["firstname"];
		$nLastname  = $userInfo["lastname"];
		$nPhone     = $userInfo["phone"];
	}
}
if( isset($_SESSION["change_ok"]) ){
	if($_SESSION["change_ok"] == true){
		$change_message = "Οι αλλαγές αποθηκεύτηκαν επιτυχώς";
	}else{
		$change_message = "Υπήρχε ένα σφάλμα με την αποθήκευση των αλλαγών. Παρακαλώ προσπαθήστε ξανά.";
	}
	unset($_SESSION["change_ok"]);
}
if( $_SERVER["REQUEST_METHOD"] == "POST" ){
	$choice =  $_POST["choice"];
	switch($choice){
/*************************     Email validation    ************************/	
		case "new_email":
			//New Email Validation
			if( empty($_POST["nEmail"]) ){
				$errors["nEmail"] = "Εισάγετε νέο email.";
			}else{
				$nEmail = trim($_POST["nEmail"]);		
			}
			if( empty($errors["nEmail"]) && !filter_var($nEmail, FILTER_VALIDATE_EMAIL) ){
				$errors["nEmail"] = "Μη έγκυρος τύπος email.";
			}
			$query = "SELECT * FROM member WHERE email = '".$nEmail."'";	
			$unique = mysqli_num_rows(mysqli_query($dblink, $query));
			if( empty($errors["nEmail"]) && $unique!=0 ){
				$errors["nEmail"] = "Το email αυτό ανήκει σε άλλο χρήστη.";
			}
			//New Email Confirmation	
			if( empty($_POST["nEmail_confirm"]) ){
				$errors["nEmail_confirm"] = "Παρακαλούμε επιβεβαιώστε το email σας.";
			}else{		
				$nEmail_confirm = trim($_POST["nEmail_confirm"]);
			}
			if( empty($errors["nEmail"]) && ($nEmail != $nEmail_confirm) ){
				$errors["nEmail_confirm"] = "Τα email δεν είναι ίδια.";
			}					
			break;		
/*************************   Password validation   ************************/	
		case "new_password":
			//Old Password Validation
			if( empty($_POST["nOldPassword"]) ){
				$errors["nOldPassword"] = "Παρακαλούμε εισάγετε τον τωρινό κωδικό σας";
			}else{		
				$nOldPassword = trim($_POST["nOldPassword"]);
			}
			if( empty($errors["nOldPassword"]) ){
				$query = "SELECT password FROM member WHERE user_id = '".$_SESSION["user_id"]."'";
				$checkUser = mysqli_query($dblink, $query);
				$userInfo = mysqli_fetch_array($checkUser);
				$password_hash = $userInfo["password"];
				if( crypt($nOldPassword, $password_hash) !== $password_hash ){
					$errors["nOldPassword"] = "Ο κωδικός που δώσατε είναι λανθασμένος. Παρακαλούμε προσπαθήστε ξανά.";
				}
			}
			//New Password Validation
			if( empty($_POST["nPassword"]) ){
				$errors["nPassword"] = "Παρακαλούμε εισάγετε νέο κωδικό.";
			}else{		
				$nPassword = trim($_POST["nPassword"]);
			}
			if( empty($errors["nPassword"]) && strlen($nPassword)<8 ){
				$errors["nPassword"] = "Ο κωδικός πρέπει να είναι τουλάχιστον 8 ψηφία.";
			}	
			//New Password Confirmation		
			if( empty($_POST["nPassword_confirm"]) ){
				$errors["nPassword_confirm"] = "Παρακαλούμε επιβεβαιώστε τον νέο σας κωδικό.";
			}else{		
				$nPassword_confirm = trim($_POST["nPassword_confirm"]);
			}
			if( empty($errors["nPassword"]) && ($nPassword != $nPassword_confirm) ){
				$errors["nPassword_confirm"] = "Οι κωδικοί δεν είναι ίδιοι.";
			}	
			break;
/************************* Other fields validation ************************/			
		case "new_other":
			$nFirstname = trim($_POST["nFirstname"]);
			$nLastname = trim($_POST["nLastname"]);
			$nPhone = trim($_POST["nPhone"]);
			if( !empty($_POST["nFirstname"]) && (preg_match("/^[\w\p{L}\p{N}\p{Pd}]+$/u", $nFirstname) != 1) ){
				$errors["nFirstname"] = "Το όνομα δεν πρέπει να περιέχει ειδικούς χαρακτήρες.";
			}
			if( !empty($_POST["nLastname"]) && (preg_match("/^[\w\p{L}\p{N}\p{Pd}]+$/u", $nLastname) != 1) ){
				$errors["nLastname"] = "Το επώνυμο δεν πρέπει να περιέχει ειδικούς χαρακτήρες.";
			}	
			if( !empty($_POST["nPhone"]) && !preg_match("/^[0-9]+$/", $nPhone) ){
				$errors["nPhone"] = "Τα τηλέφωνα πρέπει να αποτελούνται μόνο από αριθμούς.";
			}
			if( empty($errors["nPhone"]) && !empty($_POST["nPhone"]) && strlen($nPhone)!= 10 ){
				$errors["nPhone"] = "Οι τηλεφωνικοί αριθμοί αποτελούνται από 10 ψηφία";
			}
			break;
	}	
	if( count($errors) == 0 ){ //All form fields are ok!
		switch($choice){
			case "new_email":
				$nEmail = mysqli_real_escape_string($dblink, $_POST["nEmail"]);
				$query_set_part = "SET email='" . $nEmail . "'";
				break;
			case "new_password":
				$nPassword = mysqli_real_escape_string($dblink, $_POST["nPassword"]);
				$nPassword = crypt_password($nPassword);
				$query_set_part = "SET password='" . $nPassword . "'";
				break;
			case "new_other":
				$nFirstname = mysqli_real_escape_string($dblink, $_POST["nFirstname"]);
				$nLastname = mysqli_real_escape_string($dblink, $_POST["nLastname"]);
				$nPhone = mysqli_real_escape_string($dblink, $_POST["nPhone"]);
				if($nPhone == ""){
					$nPhone = "NULL";
				}else{
					$nPhone = "'" . $nPhone . "'";
				}
				$query_set_part = "SET firstname='".$nFirstname."',"."lastname='".$nLastname."',phone=".$nPhone;
				break;
		}
		$change_query = "UPDATE member " .$query_set_part. " WHERE user_id='" .$_SESSION["user_id"]."'";
		if( isset($_SESSION["data"]) ){
			unset($_SESSION["data"]);
		}
		if( mysqli_query($dblink, $change_query) ){
			$_SESSION["change_ok"] = true;
		}else{
			$_SESSION["change_ok"] = false;		
		}
		header("Location: account.php");
		exit;
		
	}else{
		$_SESSION["data"] = array();
		$_SESSION["data"]["nEmail"]            = $nEmail;
		$_SESSION["data"]["nEmail_confirm"]    = $nEmail_confirm;
		$_SESSION["data"]["nOldPassword"]      = $nOldPassword;
		$_SESSION["data"]["nPassword"]         = $nPassword;
		$_SESSION["data"]["nPassword_confirm"] = $nPassword_confirm;
		$_SESSION["data"]["nFirstname"]        = $nFirstname;
		$_SESSION["data"]["nLastname"]         = $nLastname;
		$_SESSION["data"]["nPhone"]            = $nPhone;
		$_SESSION["data"]["errors"]            = $errors;
		$_SESSION["data"]["choice"]            = $choice;
		$_SESSION["data"]["first_redirect"]    = 1;
		// Make a GET redirect to this page to avoid re-submission errors (from reloading, etc)
		header("Location: account.php", true, 303); 
		exit;
	}	
}elseif( isset($_SESSION["data"]) && is_array($_SESSION["data"]) ){
	if( isset($_SESSION["data"]["first_redirect"]) ){
		unset($_SESSION["data"]["first_redirect"]);
		$nEmail            = $_SESSION["data"]["nEmail"];
		$nEmail_confirm    = $_SESSION["data"]["nEmail_confirm"];
		$nOldPassword      = $_SESSION["data"]["nOldPassword"];
		$nPassword         = $_SESSION["data"]["nPassword"];
		$nPassword_confirm = $_SESSION["data"]["nPassword_confirm"];
		$nFirstname        = $_SESSION["data"]["nFirstname"]; 
		$nLastname         = $_SESSION["data"]["nLastname"];
		$nPhone            = $_SESSION["data"]["nPhone"];
		$errors            = $_SESSION["data"]["errors"];
		$choice            = $_SESSION["data"]["choice"];
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
	<link type="text/css" rel="stylesheet" href="account.css">
</head>
<body>
	<?php include "resources/php/header.php"; ?>
	
	<div id="main">
		<div class="header_message">
			<h1> Προσαρμογή λογαριασμού </h1>
		</div>	
		<div class="main_div form_box" id="account_box">
			<form id="account_form" action="account.php" name="account_form" method="post"> 
				<input class="change_but" type="button" id="new_email_but" value="Αλλαγή Email"></input>
				<input class="change_but" type="button" id="new_password_but" value="Αλλαγή Κωδικού"></input>
				<input class="change_but" type="button" id="new_other_but" value="Αλλαγή Προσ. Στοιχείων"></input>
				<div id="change_message"><?php echo $change_message; ?></div>
				<fieldset id="new_email">
					<div class="form_field">
						<label for name="nEmail">
							<strong>Νέο Email</strong>
							<input type="text" name="nEmail" id="nEmail" value ="<?php echo $nEmail;?>" autocomplete="off"/>
						</label>
						<span class="error"><?php show($errors, "nEmail"); ?></span>
					</div>
					<div class="form_field">
						<label for name="nEmail_confirm">
							<strong>Επιβεβαίωση νέου Email</strong>
							<input type="text" name="nEmail_confirm" id="nEmail_confirm" value ="<?php echo $nEmail_confirm;?>" autocomplete="off"/>
						</label>
						<span class="error"><?php show($errors, "nEmail_confirm"); ?></span>
					</div>
				</fieldset>	
				
				<fieldset id="new_password">
					<div class="form_field">
						<label for name="nOldPassword">
							<strong>Τρέχων Κωδικός</strong>
							<input type="password" name="nOldPassword" id="nOldPassword" value ="<?php echo $nOldPassword;?>" />
						</label>
						<span class="error"><?php show($errors, "nOldPassword"); ?></span>
					</div>
					<div class="form_field">
						<label for name="nPassword">
							<strong>Νέος κωδικός</strong>
							<input type="password" name="nPassword" id="nPassword" value ="" />
						</label>
						<span class="error"><?php show($errors, "nPassword"); ?></span>
					</div>
					<div class="form_field">
						<label for name="nPassword_confirm">
							<strong>Επιβεβαίωση νέου κωδικού</strong>
							<input type="password" name="nPassword_confirm" id="nPassword_confirm" value ="" />
						</label>
						<span class="error"><?php show($errors, "nPassword_confirm"); ?></span>
					</div>
				</fieldset>		
				
				<fieldset id="new_other">
					<div class="form_field">
						<label for name="nFirstname">
							<strong>Όνομα</strong>
							<input type="text" name="nFirstname" id="nFirstname" value ="<?php echo $nFirstname;?>" autocomplete="off" maxlength="20" />
						</label>
						<span class="error"><?php show($errors, "nFirstname"); ?></span>
					</div>
					<div class="form_field">
						<label for name="nLastname">
							<strong>Επώνυμο</strong>
							<input type="text" name="nLastname" id="nLastname" value ="<?php echo $nLastname;?>" autocomplete="off" maxlength="20"/>
						</label>
						<span class="error"><?php show($errors, "nLastname"); ?></span>
					</div>
					<div class="form_field">
						<label for name="nPhone">
							<strong>Τηλέφωνο</strong>
							<input type="text" name="nPhone" id="nPhone" value ="<?php echo $nPhone;?>" autocomplete="off" maxlength="15" />
						</label>
						<span class="error"><?php show($errors, "nPhone"); ?></span>
					</div>
				</fieldset>				
				<div class="form_field">
					<input type="hidden" name="choice" id="choice" value ="<?php echo $choice; ?>" />
				</div>
				<div class="form_field">
					<input type="submit" name="submit_but" id="submit_but" value ="Αποθήκευση αλλαγών" />
				</div>
			</form>
		</div>
	<?php include "resources/php/footer.php"; ?>
	</div>	
	
	<script src="resources/js/init.js"></script>
	<script src="http://ajax.googleapis.com/ajax/libs/jquery/1.11.1/jquery.min.js"></script>
	<script src="resources/js/manage_account_form.js"></script>
</body>
</html>
