<?php include_once "resources/php/db_connect.php"; ?>
<?php
if( !isset($_SESSION["admin"]) ){   //User is not logged in. Redirect to home page
	header("Location: index.php");
	exit;
}
?>
<?php
	
	function create_categories_xml($dblink){
		$dom = new DOMDocument("1.0", "UTF-8");
		$dom->formatOutput = true;
		$categories = $dom->createElement("categories");
		$categories = $dom->appendChild($categories);
		
		$query = "SELECT * FROM category ORDER BY category_id ASC";
		$result = mysqli_query($dblink, $query);
	
		header("Content-type: text/xml");
		while( $row = @mysqli_fetch_assoc($result) ){
			$category = $dom->createElement("category");
			$category = $categories->appendChild($category);
			
			$category_id = $dom->createElement("category_id",$row["category_id"]);
			$category_id = $category->appendChild($category_id);
			$name = $dom->createElement("name",$row["name"]);
			$name = $category->appendChild($name);							
		}		
		echo $dom->saveXML();
	}
	
	create_categories_xml($dblink);
?>
