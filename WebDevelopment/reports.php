<?php include_once "resources/php/db_connect.php"; ?>
<?php include_once "resources/php/site_info.php"; ?>
<?php

function show($errors, $code){
	if(isset($errors[$code]) ){
		echo $errors[$code];
	}
}

$nComments = $nReport_id = "";
$nCategory_name = $nCategory_id = "";
$errors = array();

if( !isset($_SESSION["admin"]) ){   //User is not an admin. Redirect to home page
	header("Location: index.php");
	exit;
}elseif( $_SERVER["REQUEST_METHOD"] == "POST" ){ 
	if( isset($_POST["submit_report"])){
		$nComments  = trim($_POST["nComments"]);
		$nReport_id = trim($_POST["nReport_id"]);	
		$nComments  = mysqli_real_escape_string($dblink, $nComments);
		$nReport_id = mysqli_real_escape_string($dblink, $nReport_id);
		
		$query = "INSERT INTO report_solved (report_id,admin_id,solved_date,comments)  
			VALUES('".$nReport_id."','".$_SESSION["user_id"]."', now()".",'".$nComments."')";	
	}else{
		if( $_POST["action"] != "delete"){
			if( empty($_POST["nCategory_name"]) ){
				$errors["nCategory_name"] = "Please enter a category name";
			}else{
				$nCategory_name = trim($_POST["nCategory_name"]);		
			}
		}
		$nCategory_id = trim($_POST["nCategory_id"]);	
		$nCategory_name = mysqli_real_escape_string($dblink, $nCategory_name);
		$nCategory_id = mysqli_real_escape_string($dblink, $nCategory_id);
		switch($_POST["action"]){
			case "edit":				
				$query = "UPDATE category SET name='".$nCategory_name."' WHERE category_id='".$nCategory_id."'";
				break;
			case "delete":
				$query = "DELETE FROM category WHERE category_id='" . $nCategory_id . "'";
				break;
			case "new";
				$query = "INSERT INTO category (name)  VALUES ('" . $nCategory_name . "')";
				break;
		}
	}	
	if( count($errors) == 0 ){ //All form fields are ok!
		if( isset($_SESSION["data"]) ){
			unset($_SESSION["data"]);
		}
		mysqli_query($dblink, $query);
		header("Location: reports.php");			
		exit;
	}else{
		$_SESSION["data"] = array();
		$_SESSION["data"]["nCategory_id"]   = $nCategory_id;
		$_SESSION["data"]["nCategory_name"] = $nCategory_name;
		$_SESSION["data"]["errors"]         = $errors;
		$_SESSION["data"]["first_redirect"]    = 1;
		header("Location: reports.php", true, 303); 
		exit;
	}	
}elseif( isset($_SESSION["data"]) && is_array($_SESSION["data"]) ){
	if( isset($_SESSION["data"]["first_redirect"]) ){
		unset($_SESSION["data"]["first_redirect"]);
		$nCategory_id   = $_SESSION["data"]["nCategory_id"]; 
		$nCategory_name = $_SESSION["data"]["nCategory_name"]; 
		$errors = $_SESSION["data"]["errors"];
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
	<link type="text/css" rel="stylesheet" href="tables.css">
</head>
<body>
	<?php include "resources/php/header.php"; ?>
	
	<div id="main">
		<div class="main_div table_div" id="reports_div">
			<table id="reports_table">
				<caption>
					<div id="tabs_div">
						<div id="open" class="tab selected">Ανοιχτές</div>
						<div id="solved" class="tab">Λυμένες</div>
					</div>
				</caption>
				<thead>
					<tr>
					</tr>
				</thead>
				<tfoot>
					<tr>
						<td colspan="7">
							<div id="page_nav"></div>
						</td>
					</tr>
				</tfoot>
				<tbody>
				</tbody>
			</table>
		</div>	
		
		<div class="main_div table_div" id="categories_div">	
			<table id="categories_table" class="rounded">
				<thead>
					<tr>
						<th colspan="3">Όνομα</th>
					</tr>
				</thead>
				<tfoot>
					<tr id="foot">
						<td id="add_cat_but" colspan="3" class="edit_message">Νέα κατηγορία</td>
					</tr>
				</tfoot>
				<tbody>
				</tbody>
			</table>
		</div>
	
	<?php include "resources/php/footer.php"; ?>
	</div>
	
	<div id="reports_popup" class="tinted">
		<div class="form_box popup_box">	
			<div class="x_but_div">
				<div class="x_but">X</div>
			</div>
			<form id="solveit_form" action="reports.php" name="solveit_form" method="post">
				<div class="form_field">
					<label for name="nComments">
						<strong>Σχόλια: </strong>
						<textarea cols="50" rows="10" name="nComments" id="nComments" maxlength="500" ></textarea>
					</label>
					<span class="error"></span>
				</div>
				<div class="form_field">
					<input type="hidden" name="nReport_id" id="nReport_id" value=""></input>
				</div>
				<div class="form_field">
					<input type="submit" name="submit_report" id="submit_report" value ="Solve It" />
				</div>				
			</form>
		</div>
	</div>
	
	<div id="categories_popup" class="tinted">
		<div class="form_box popup_box">	
			<div class="x_but_div">
				<div class="x_but">X</div>
			</div>
			<form id="edit_category" action="reports.php" name="edit_category" method="post">
				<div id="new_name" class="form_field">
					<strong>Category Name: </strong>
					<input type="text" name="nCategory_name" id="nCategory_name" value=""></input>
					<span class="error"><?php show($errors, "nCategory_name"); ?></span>
				</div>
				<div id="delete_msg" class="form_field">
					<strong>Delete this Category?</strong>
				</div>
				<div class="form_field">
					<input type="hidden" name="nCategory_id" id="nCategory_id" value="<?php echo $nCategory_id; ?>"></input>
				</div>
				<div class="form_field">
					<input type="hidden" name="action" id="action" value=""></input>
				</div>
				<div class="form_field">
					<input type="submit" name="submit_category" id="submit_category" value ="Save Changes" />
				</div>				
			</form>
		</div>
	</div>
	
	<script src="resources/js/init.js"></script>
	<script src="http://ajax.googleapis.com/ajax/libs/jquery/1.11.1/jquery.min.js"></script>
	<script src="resources/js/page_navigation.js"></script>
	<script src="resources/js/init_reports.js"></script>
	<script src="resources/js/popup.js"></script>
</body>
</html>
