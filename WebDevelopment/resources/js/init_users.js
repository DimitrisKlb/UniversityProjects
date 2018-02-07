var users = [];

pagNumber = 20;
selectedPage = 1;
pageNavDiv = document.getElementById("page_nav"); //The div containing the pagenav list

function getData(){
	if(window.XMLHttpRequest){   
		xmlhttp = new XMLHttpRequest();
	}else{ // IE6 and below
		xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
	}
	xmlhttp.onreadystatechange = function(){
		if( xmlhttp.readyState == 4 && xmlhttp.status == 200 ){
			updateData(xmlhttp, true);
		}
	};
	xmlhttp.open("GET", "get_users.php", true);
	xmlhttp.send();
}

function updateData(answer, firstTime){
	if(firstTime){
		var xml = answer.responseXML;	
		users = [];
		users = xml.documentElement.getElementsByTagName("user");
	}	
	var table = document.getElementById("users_table")
	table.removeChild(table.getElementsByTagName("tbody")[0]);
	tableBody = document.createElement("tbody");
	table.appendChild(tableBody);
	
	var i = (selectedPage-1)*pagNumber;
	var max = selectedPage*pagNumber
	for(; i<users.length && i<max; i++){
		var row = document.createElement("tr");
		var content;
		
		content = document.createElement("td");
		content.appendChild( document.createTextNode(getFromXML(users, i, "email")) );
		row.appendChild(content);
		content = document.createElement("td");
		var name = getFromXML(users, i, "firstname") +" "+ getFromXML(users, i, "lastname");
		content.appendChild( document.createTextNode(name) );
		row.appendChild(content);
		content = document.createElement("td");
		content.appendChild( document.createTextNode(getFromXML(users, i, "phone")) );
		row.appendChild(content);
		
		row.setAttribute("id", i );
		tableBody.appendChild(row);
	}

	pages = Math.ceil( users.length / pagNumber );
	if(pages > 1){		
		makePageNav();
	}
	
	$("tbody tr").click(function(){	
		$("#change_msg").text("");
		
		var pos = $(this).attr("id");	
		$("#nUser_id").val(getFromXML(users, pos, "user_id"));
		$("#nEmail").val(getFromXML(users, pos, "email"));
		$("#nFirstname").val(getFromXML(users, pos, "firstname"));
		$("#nLastname").val(getFromXML(users, pos, "lastname"));
		$("#nPhone").val(getFromXML(users, pos, "phone"));
		
		$(".tinted").css("display", "flex");	
	});
}

getData();
$(document).ready(function(){	
	if($("#nUser_id").val() != ""){
		$(".tinted").css("display", "flex");
	}
	if( $("#choice").val() == "delete" ){
		$("#submit_but").hide();
	}
	$("body").keydown(function(){
		if(event.which == 27){  //If Escape was pressed
			$("#nUser_id").val("");
			$(".error").text("");
			$("fieldset").hide();
			$("#submit_but").hide();
		}
	});
	$(".x_but").click(function(){			
		$("#nUser_id").val("");
		$(".error").text("");
	});

	$("#delete_but").click(function(){			
		if(true){
			var choice = $(this).attr("id").slice(0,-4);
			$("#choice").val(choice);
			$("#account_form").submit();
		}
	});	
	$(".change_but").click(function(){	
		$(".error").text("");	
	});
});