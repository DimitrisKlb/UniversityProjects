<?php include_once "resources/php/db_connect.php"; ?>
<?php include_once "resources/php/site_info.php"; ?>
<!DOCTYPE html>
<html lang="el">
<head> 
	<meta charset="utf-8">
	<title><?php echo "$site_name"; ?></title> 
	<link type="text/css" rel="stylesheet" href="general.css">
	<link type="text/css" rel="stylesheet" href="tables.css">
	<link type="text/css" rel="stylesheet" href="index.css">
</head>
<body>
	<?php include "resources/php/header.php"; ?>
	
	<div id="main">
		<div class="main_div" id="google_map">
			
		</div>
		<div class="header_message">
			<h1> Τελευταίες 20 Αναφορές </h1>
		</div>
		<div class="main_div table_div" id="reports_div">
			<table id="reports_table" class="rounded">				
				<thead>
					<tr>
						<th>Ημ/νία Καταχώρησης</th>
						<th>Κατηγορία</th>
						<th>Χρήστης</th>
						<th>Περιγραφή</th>
					</tr>
				</thead>
				<tfoot>
					<tr>
						<td colspan="4"></td>
					</tr>
				</tfoot>
				<tbody>
				</tbody>
			</table>
		</div>
		<div class="main_div" id="statistics_div">
	
		</div>	
	<?php include "resources/php/footer.php"; ?>
	</div>	
	
	<script src="resources/js/init.js"></script>
	<script src="http://ajax.googleapis.com/ajax/libs/jquery/1.11.1/jquery.min.js"></script>
	<script src="https://maps.googleapis.com/maps/api/js?sensor=false"></script>
	<script src="https://www.google.com/jsapi"></script>
	<script src="resources/js/init_index_gmap.js"></script>
	<script src="resources/js/init_index_chart.js"></script>
	<script src="resources/js/popup.js"></script>
</body>
</html>
