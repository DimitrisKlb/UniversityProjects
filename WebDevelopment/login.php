<?php include_once "resources/php/db_connect.php"; ?>
<?php include_once "resources/php/site_info.php"; ?>
<?php
$err_login = "";
$nEmail = "";

if( isset($_SESSION["loggedIn"]) ){   //User is logged in. Redirect to home page
	header("Location: index.php");
	exit;
}elseif( $_SERVER["REQUEST_METHOD"] == "POST" ){	
	$nEmail = mysqli_real_escape_string($dblink, $_POST["nEmail"]);
	$nPassword = mysqli_real_escape_string($dblink, $_POST["nPassword"]);
	
	$query = "SELECT * FROM member WHERE email = '".$nEmail."'";
	$checkUser = mysqli_query($dblink, $query);
	
	if(mysqli_num_rows($checkUser)==1){	
		$userInfo = mysqli_fetch_array($checkUser);
		$password_hash = $userInfo["password"];
		if( crypt($nPassword,$password_hash) === $password_hash ){
			$_SESSION["loggedIn"] = 1;
			$_SESSION["user_id"] = $userInfo["user_id"];			
			$_SESSION["email"] = $userInfo["email"];	
			if($userInfo["acc_type"] == "admin"){
				$_SESSION["admin"] = 1;
			}
			if( isset($_SESSION["data"]) ){
				unset($_SESSION["data"]);
			}
			header("Location: index.php");	 //User logged in. Redirect to home page
			exit;
		}else{
			$err_login = "The password you entered is incorrect. Please try again.";
		}
	}else{
		$err_login = "The email you entered does not belong to any account!";
	}	
	if( $err_login != ""){
		$_SESSION["data"] = array();
		$_SESSION["data"]["nEmail"] = $nEmail; 
		$_SESSION["data"]["error"]  = $err_login; 
		// Make a GET redirect to this page to avoid re-submission errors (from reloading, etc)
		header("Location: login.php", true, 303); 
		exit;
	}
}else if( isset($_SESSION["data"]) && is_array($_SESSION["data"]) ){
	$nEmail    = $_SESSION["data"]["nEmail"]; 
	$err_login = $_SESSION["data"]["error"]; 
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
		<div class="main_div form_box" id="login_box">			
			<form id="login_form" action="login.php" name="login_form" method="post"> 
				<div class="form_field">
					<label for name="nEmail">
						<strong>Email:</strong>
						<input type="text" name="nEmail" id="nEmail" value ="<?php echo $nEmail; ?>" autofocus="on"></input>
					</label>
				</div>
				<div class="form_field">
					<label for name="nPassword">
						<strong>Κωδικός:</strong>
						<input type="password" name="nPassword" id="nPassword" value =""></input>
					</label>
				</div>
				<div class="form_field">
					<input type="submit" name="login_but" id="login_but" value ="Σύνδεση"></input>
				</div>				
				<span class="error"><?php echo $err_login; ?></span>
			</form>
		</div>
	
	<?php include "resources/php/footer.php"; ?>
	</div>	
	
	<script src="resources/js/init.js"></script>
</body>
</html>