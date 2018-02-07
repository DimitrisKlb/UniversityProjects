<?php include_once "resources/php/db_connect.php"; ?>
<?php
if( !isset($_SESSION["loggedIn"]) ){   //User is not logged in. Redirect to home page
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
		
		$query = "SELECT report.*, member.user_id, category.*, report_solved.* FROM report 
			INNER JOIN member ON report.user_id = member.user_id
			INNER JOIN category ON report.category_id = category.category_id
			LEFT JOIN report_solved ON report.report_id = report_solved.report_id
			WHERE member.user_id = ". $_SESSION["user_id"] . "
			ORDER BY report.entry_date DESC";
		$result = mysqli_query($dblink, $query);
	
		header("Content-type: text/xml");
		while( $row = @mysqli_fetch_assoc($result) ){
			$report = $dom->createElement("report");
			$report = $reports->appendChild($report);
			
			$entry_date = $dom->createElement("entry_date",$row["entry_date"]);
			$entry_date = $report->appendChild($entry_date);
			$category = $dom->createElement("category",$row["name"]);
			$category = $report->appendChild($category);	
			$description = $dom->createElement("description",$row["description"]);
			$description = $report->appendChild($description);	
			$solved_date = $dom->createElement("solved_date",$row["solved_date"]);
			$solved_date = $report->appendChild($solved_date);		
			$comments = $dom->createElement("comments",$row["comments"]);
			$comments = $report->appendChild($comments);				
		}		
		echo $dom->saveXML();
	}
	
	create_reports_xml($dblink);
?>
