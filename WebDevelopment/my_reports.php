<?php include_once "resources/php/db_connect.php"; ?>
<?php include_once  "resources/php/site_info.php"; ?>
<?php
if( !isset($_SESSION["loggedIn"]) ){   //User is not logged in. Redirect to home page
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
	<link type="text/css" rel="stylesheet" href="tables.css">
</head>
<body>
	<?php include "resources/php/header.php"; ?>
	
	<div id="main">
		<div class="main_div table_div" id="my_reports_div">
			<table id="my_reports_table" class="rounded">
				<thead>
					<tr>
						<th>Ημ/νία Δημοσίευσης</th>
						<th>Κατηγορία</th>
						<th>Περιγραφή</th>
						<th>Ημ/νία Επίλυσης</th>
						<th>Σχόλια</th>
					</tr>
				</thead>
				<tfoot>
					<tr>
						<td colspan="5">
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
	
	<script src="resources/js/init.js"></script>
	<script src="http://ajax.googleapis.com/ajax/libs/jquery/1.11.1/jquery.min.js"></script>
	<script src="resources/js/page_navigation.js"></script>
	<script src="resources/js/init_my_reports.js"></script>
</body>
</html>