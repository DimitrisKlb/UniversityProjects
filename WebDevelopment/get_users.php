<?php include_once "resources/php/db_connect.php"; ?>
<?php
if( !isset($_SESSION["admin"]) ){   //User is not an admin. Redirect to home page
	header("Location: index.php");
	exit;
}
?>
<?php
	
	function create_users_xml($dblink){
		$dom = new DOMDocument("1.0", "UTF-8");
		$dom->formatOutput = true;
		$users = $dom->createElement("users");
		$users = $dom->appendChild($users);
		
		$query = "SELECT *, member.user_id AS the_id FROM member
					LEFT JOIN member_deleted ON member.user_id = member_deleted.user_id
					WHERE member_deleted.user_id IS NULL";
		$result = mysqli_query($dblink, $query);
		
		header("Content-type: text/xml");
		while( $row = @mysqli_fetch_assoc($result) ){
			$user = $dom->createElement("user");
			$user = $users->appendChild($user);
			
			$user_id = $dom->createElement("user_id",$row["the_id"]);
			$user_id = $user->appendChild($user_id);
			$email = $dom->createElement("email",$row["email"]);
			$email = $user->appendChild($email);
			$firstname = $dom->createElement("firstname",$row["firstname"]);
			$firstname = $user->appendChild($firstname);
			$lastname = $dom->createElement("lastname",$row["lastname"]);
			$lastname = $user->appendChild($lastname);
			$phone = $dom->createElement("phone",$row["phone"]);
			$phone = $user->appendChild($phone);		
		}		
		echo $dom->saveXML();
	}
	
	create_users_xml($dblink);
?>
