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
$nPassword = $nPassword_confirm = "";
$nFirstname = $nLastname = $nPhone = "";
$errors = array();
$choice = "";
$change_message = "";
$nUser_id = "";

if( !isset($_SESSION["admin"]) ){   //User is not an admin. Redirect to home page
	header("Location: index.php");
	exit;
}

if( isset($_SESSION["change_ok"]) ){
	if($_SESSION["change_ok"] == true){
		$change_message = "Your changes have been successfully saved";
	}else{
		$change_message = "Sorry, your changes could not be saved. Please try again.";
	}
	unset($_SESSION["change_ok"]);
}
if( $_SERVER["REQUEST_METHOD"] == "POST" ){
	$choice =  $_POST["choice"];
	$nUser_id  =  $_POST["nUser_id"];
	switch($choice){
		case "delete":
			if($nUser_id == $_SESSION["user_id"]){
				$errors["nDeletedUser"] = "You cannot delete yourshelf";
			}
			break;
/*************************     Email validation    ************************/	
		case "new_email":
			//New Email Validation
			if( empty($_POST["nEmail"]) ){
				$errors["nEmail"] = "Please enter a new email";
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
			//New Email Confirmation	
			if( empty($_POST["nEmail_confirm"]) ){
				$errors["nEmail_confirm"] = "Please confirm the email";
			}else{		
				$nEmail_confirm = trim($_POST["nEmail_confirm"]);
			}
			if( empty($errors["nEmail"]) && ($nEmail != $nEmail_confirm) ){
				$errors["nEmail_confirm"] = "The emails do not match";
			}					
			break;		
/*************************   Password validation   ************************/	
		case "new_password":
			
			//New Password Validation
			if( empty($_POST["nPassword"]) ){
				$errors["nPassword"] = "Please enter a new password"; 
			}else{		
				$nPassword = trim($_POST["nPassword"]);
			}
			if(strlen($nPassword)<8 ){
				$errors["nPassword"] = "Password must be at least 8 characters"; 
			}	
			//New Password Confirmation		
			if( empty($_POST["nPassword_confirm"]) ){
				$errors["nPassword_confirm"] = "Please confirm the new password";		
			}else{		
				$nPassword_confirm = trim($_POST["nPassword_confirm"]);
			}
			if( empty($errors["nPassword"]) && ($nPassword != $nPassword_confirm) ){
				$errors["nPassword_confirm"] = "The passwords do not match";
			}	
			break;
/************************* Other fields validation ************************/			
		case "new_other":
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
		if($choice == "delete"){
			$change_query = "INSERT INTO member_deleted (user_id) VALUES (". "'".$nUser_id."')"; 
		}else{
			$change_query = "UPDATE member " .$query_set_part. " WHERE user_id='" .$nUser_id."'";
		}
		if( isset($_SESSION["data"]) ){
			unset($_SESSION["data"]);
		}
		if( mysqli_query($dblink, $change_query) ){
			$_SESSION["change_ok"] = true;
		}else{
			$_SESSION["change_ok"] = false;		
		}		
		header("Location: users.php");
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
		$_SESSION["data"]["errors"]            = $errors;
		$_SESSION["data"]["choice"]            = $choice;
		$_SESSION["data"]["nUser_id"]          = $nUser_id;
		$_SESSION["data"]["first_redirect"]    = 1;
		// Make a GET redirect to this page to avoid re-submission errors (from reloading, etc)
		header("Location: users.php", true, 303); 
		exit;
	}	
}elseif( isset($_SESSION["data"]) && is_array($_SESSION["data"]) ){
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
		$choice            = $_SESSION["data"]["choice"];
		$nUser_id          = $_SESSION["data"]["nUser_id"];
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
	<link type="text/css" rel="stylesheet" href="tables.css">
	<link type="text/css" rel="stylesheet" href="forms.css">
	<link type="text/css" rel="stylesheet" href="account.css">
</head>
<body>
	<?php include "resources/php/header.php"; ?>
	
	<div id="main">
		<div id="change_msg"><?php echo $change_message; ?></div>
		<div class="main_div table_div" id="users_div">
			<table id="users_table" class="rounded">
				<thead>
					<tr>
						<th>Email</th>
						<th>Όνομα</th>
						<th>Τηλέφωνο</th>
					</tr>
				</thead>
				<tfoot>
					<tr>
						<td colspan="3">
							<div id="page_nav"></div>
						</td>
					</tr>
				</tfoot>
				<tbody>
				</tbody>
			</table>
		</div>
	
	<?php include "resources/php/footer.php"; ?>
	</div>	
	
	<div class="tinted">
		<div class="form_box popup_box">	
			<div class="x_but_div">
				<div class="x_but">X</div>
			</div>
			<form id="account_form" action="users.php" name="account_form" method="post"> 
				<input type="button" id="delete_but" value="Διαγραφή Χρήστη"></input>
				<span class="error"><?php show($errors, "nDeletedUser"); ?></span>
				<input class="change_but" type="button" id="new_email_but" value="Email"></input>
				<input class="change_but" type="button" id="new_password_but" value="Κωδικός"></input>
				<input class="change_but" type="button" id="new_other_but" value="Προσωπικά Στοιχεία"></input>
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
							<strong>Επιβεβαίωση νέου email </strong>
							<input type="text" name="nEmail_confirm" id="nEmail_confirm" value ="<?php echo $nEmail_confirm;?>" autocomplete="off"/>
						</label>
						<span class="error"><?php show($errors, "nEmail_confirm"); ?></span>
					</div>
				</fieldset>	
				
				<fieldset id="new_password">
					<div class="form_field">
						<label for name="nPassword">
							<strong>Νέος Κωδικός</strong>
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
					<input type="hidden" name="nUser_id" id="nUser_id" value ="<?php echo $nUser_id; ?>" />
				</div>
				<div class="form_field">
					<input type="submit" name="submit_but" id="submit_but" value ="Αποθήκευση αλλαγών" />
				</div>
			</form>
		</div>
	</div>
	
	<script src="resources/js/init.js"></script>
	<script src="http://ajax.googleapis.com/ajax/libs/jquery/1.11.1/jquery.min.js"></script>
	<script src="resources/js/page_navigation.js"></script>
	<script src="resources/js/popup.js"></script>
	<script src="resources/js/manage_account_form.js"></script>
	<script src="resources/js/init_users.js"></script>
</body>
</html>
