<?php include_once "resources/php/db_connect.php"; ?>
<?php
if( !isset($_SESSION["admin"]) ){   //User is not logged in. Redirect to home page
	header("Location: index.php");
	exit;
}
?>
<?php
	
	function create_reports_xml($dblink){
		$dom = new DOMDocument("1.0", "UTF-8");
		$dom->formatOutput = true;
		$reports = $dom->createElement("reports");
		$reports = $dom->appendChild($reports);
		// Closed reports!
		$query = "SELECT report.*, _MEMBER.user_id, _MEMBER.email AS reporter, category.*, report_solved.*, 
					_ADMIN.user_id, _ADMIN.email AS admin
					FROM report
					INNER JOIN member AS _MEMBER ON report.user_id = _MEMBER.user_id
					INNER JOIN category ON report.category_id = category.category_id
					INNER JOIN report_solved ON report.report_id = report_solved.report_id
					INNER JOIN member AS _ADMIN ON report_solved.admin_id = _ADMIN.user_id
					ORDER BY report_solved.solved_date DESC";
		$result = mysqli_query($dblink, $query);
	
		header("Content-type: text/xml");
		while( $row = @mysqli_fetch_assoc($result) ){
			$report = $dom->createElement("report");
			$report = $reports->appendChild($report);
			
			$entry_date = $dom->createElement("entry_date",$row["entry_date"]);
			$entry_date = $report->appendChild($entry_date);
			$category = $dom->createElement("category",$row["name"]);
			$category = $report->appendChild($category);	
			$user = $dom->createElement("user",$row["reporter"]);
			$user = $report->appendChild($user);
			$description = $dom->createElement("description",$row["description"]);
			$description = $report->appendChild($description);	
			$solved_date = $dom->createElement("solved_date",$row["solved_date"]);
			$solved_date = $report->appendChild($solved_date);	
			$admin = $dom->createElement("admin",$row["admin"]);
			$admin = $report->appendChild($admin);			
			$comments = $dom->createElement("comments",$row["comments"]);
			$comments = $report->appendChild($comments);							
		}		
		echo $dom->saveXML();
	}
	
	create_reports_xml($dblink);
?>
