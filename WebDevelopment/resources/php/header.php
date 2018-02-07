	<div id="header">
		<div id="left">	
			<div id="drop" onmouseover="showDropMenu(true)" onmouseout="showDropMenu(false)">
				<div class="button" id="drop_icon">
					<div id="drop_image">				
					</div>
				</div>
			</div>
		</div>
		<div id="navs_container">
			<div class="navbar" id="general_bar">
				<a href="index.php">
					<div class="button" id="index">Αρχική</div>
				</a>
<?php
if( !empty($_SESSION["loggedIn"]) ){
?>		
				<a href="my_reports.php">
					<div class="button" id="my_reports"> Οι αναφορές μου </div>
				</a>
				<a href="new_report.php">
					<div class="button" id="new_report"> Νέα αναφορά</div>
				</a>
				<?php
				if(!empty($_SESSION["admin"])){
				?>
				<a href="reports.php">
					<div class="button" id="reports"> Αναφορές </div>				
				</a>
				<a href="users.php">
					<div class="button" id="users"> Χρήστες </div>				
				</a>
				<?php
				}
				?>
			</div>
			<div class="navbar" id="user_bar">				
				<a href="account.php"> 
					<div class="button" id="account">  
					<?php
					if(!empty($_SESSION["admin"])){ //elegxos gia admin!!
						echo "Sir " . $_SESSION["email"];
					}else{
						echo $_SESSION["email"];
					}					
					?>
					</div>
				</a>	 
				<a href="logout.php">
					<div class="button" id="logout"> Αποσύνδεση </div>				
				</a>
			</div>
<?php
}else{
?>	
			</div>
			<div class="navbar" id="connect_bar">
				<a href="login.php">
					<div class="button" id="login"> Σύνδεση </div>
				</a>
				<a href="register.php">
					<div class="button" id="register"> Εγγραφή </div>
				</a>
			</div>
<?php
}
?>	
		</div>
	</div>