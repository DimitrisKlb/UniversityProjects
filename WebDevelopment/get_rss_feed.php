<?php include_once "resources/php/db_connect.php"; ?>
<?php
	$num_reports = 20;
	
	function create_rss_feed($dblink, $num_reports){
		$dom = new DOMDocument("1.0", "UTF-8");
		$dom->formatOutput = true;
		header("Content-type: text/xml");
		
		$rss = $dom->createElement("rss");
		$rss = $dom->appendChild($rss);
		$rss->setAttribute("version", "2.0");
		$channel = $dom->createElement("channel");
		$channel = $rss->appendChild($channel);
		
		$title = $dom->createElement("title", "Damage Report");
		$title = $channel->appendChild($title);
		$link = $dom->createElement("link", "http://localhost/wd/index.php");
		$link = $channel->appendChild($link);
		$description = $dom->createElement("description", "A CEID project for the Web Development course, 2014");
		$description = $channel->appendChild($description);
		
		$query = "SELECT member.firstname, member.lastname, category.name, report.* FROM member 
			INNER JOIN report ON member.user_id = report.user_id
			INNER JOIN category ON category.category_id = report.category_id
			ORDER BY report.entry_date DESC LIMIT " . $num_reports;
		$result = mysqli_query($dblink, $query);
		
		while( $row = @mysqli_fetch_assoc($result) ){
			$item = $dom->createElement("item");
			$item = $channel->appendChild($item);
			
			$entry_date = $dom->createElement("entry_date",$row["entry_date"]);
			$entry_date = $item->appendChild($entry_date);
			$category = $dom->createElement("category",$row["name"]);
			$category = $item->appendChild($category);	
			
			$name = $row["firstname"] . " " . $row["lastname"];
			$uploader = $dom->createElement("uploader", $name);
			$uploader = $item->appendChild($uploader);	
			
			$lat = $dom->createElement("lat",$row["lat"]);
			$lat = $item->appendChild($lat);			
			$lng = $dom->createElement("lng",$row["lng"]);
			$lng = $item->appendChild($lng);			
			$description = $dom->createElement("description",$row["description"]);
			$description = $item->appendChild($description);	
			
					
		}		
		echo $dom->saveXML();
	}
	
	create_rss_feed($dblink, $num_reports);
?>