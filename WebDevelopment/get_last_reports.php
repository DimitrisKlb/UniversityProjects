<?php include_once "resources/php/db_connect.php"; ?>
<?php
	$num_reports = 20;
	
	function create_last_reports_xml($dblink, $num_reports){
		$dom = new DOMDocument("1.0", "UTF-8");
		$dom->formatOutput = true;
		header("Content-type: text/xml");
		$reports = $dom->createElement("reports");
		$reports = $dom->appendChild($reports);
		$reports->setAttribute("number", $num_reports);
/*------------------------------ Satisticks Info ------------------------------*/	
		$statistics = $dom->createElement("statistics");
		$statistics = $reports->appendChild($statistics);

		$query = "SELECT COUNT(*) FROM report";
		$row = @mysqli_fetch_assoc(mysqli_query($dblink, $query));
		$total_reports = $dom->createElement("total_reports", $row["COUNT(*)"]);
		$total_reports = $statistics->appendChild($total_reports);
		
		$query = "SELECT COUNT(*) FROM report_solved";
		$row = @mysqli_fetch_assoc(mysqli_query($dblink, $query));
		$solved_reports = $dom->createElement("solved_reports",  $row["COUNT(*)"]);
		$solved_reports = $statistics->appendChild($solved_reports);
		
		$query = "SELECT AVG(DATEDIFF (solved_date, entry_date)) AS time_average FROM report
					INNER JOIN report_solved ON report.report_id = report_solved.report_id";
		$row = @mysqli_fetch_assoc(mysqli_query($dblink, $query));
		$average_solve_time = $dom->createElement("average_solve_time", $row["time_average"]);
		$average_solve_time = $statistics->appendChild($average_solve_time);
/*------------------------------ Reports Info ------------------------------*/		
		$query = "SELECT member.firstname, member.lastname, category.name, report.* FROM member 
			INNER JOIN report ON member.user_id = report.user_id
			INNER JOIN category ON category.category_id = report.category_id
			ORDER BY report.entry_date DESC LIMIT " . $num_reports;
		$result = mysqli_query($dblink, $query);
		
		
		while( $row = @mysqli_fetch_assoc($result) ){
			$report = $dom->createElement("report");
			$report = $reports->appendChild($report);
			$report->setAttribute("id", $row["report_id"]);			

			$report_id = $dom->createElement("report_id",$row["report_id"]);
			$report_id = $report->appendChild($report_id);
			$entry_date = $dom->createElement("entry_date",$row["entry_date"]);
			$entry_date = $report->appendChild($entry_date);
			$category = $dom->createElement("category",$row["name"]);
			$category = $report->appendChild($category);	
			$firstname = $dom->createElement("firstname",$row["firstname"]);
			$firstname = $report->appendChild($firstname);		
			$lastname = $dom->createElement("lastname",$row["lastname"]);
			$lastname = $report->appendChild($lastname);					
			$lat = $dom->createElement("lat",$row["lat"]);
			$lat = $report->appendChild($lat);			
			$lng = $dom->createElement("lng",$row["lng"]);
			$lng = $report->appendChild($lng);			
			$description = $dom->createElement("description",$row["description"]);
			$description = $report->appendChild($description);			
		}			
		echo $dom->saveXML();
	}
	
	create_last_reports_xml($dblink, $num_reports);
?>