var reports = [];
var categories = [];
var reportsType = "open"; //Can be either "open" or "solved"

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
	xmlhttp.open("GET", "get_reports_"+ reportsType +".php", true);
	xmlhttp.send();
}

function updateData(answer, firstTime){
	if(firstTime){
		var xml = answer.responseXML;	
		reports = [];
		reports = xml.documentElement.getElementsByTagName("report");
	}	
	var table = document.getElementById("reports_table");
	table.removeChild(table.getElementsByTagName("thead")[0]);
	var tableHeader = document.createElement("thead");
	tableHeader.appendChild(document.createElement("tr"));
	table.appendChild(tableHeader);
	tableHeader = tableHeader.getElementsByTagName("tr")[0];
	
	table.removeChild(table.getElementsByTagName("tbody")[0]);
	var tableBody = document.createElement("tbody");
	table.appendChild(tableBody);	
	
	var content;
	content = document.createElement("th");
	content.appendChild( document.createTextNode("Date/Time Posted") );
	tableHeader.appendChild(content);
	content = document.createElement("th");
	content.appendChild( document.createTextNode("Category") );
	tableHeader.appendChild(content);
	content = document.createElement("th");
	content.appendChild( document.createTextNode("Reported by") );
	tableHeader.appendChild(content);
	content = document.createElement("th");
	content.appendChild( document.createTextNode("Description") );
	tableHeader.appendChild(content);
	if( reportsType == "solved"){
		content = document.createElement("th");
		content.appendChild( document.createTextNode("Date/Time Solved") );
		tableHeader.insertBefore(content, tableHeader.firstChild);
		content = document.createElement("th");
		content.appendChild( document.createTextNode("Solved By") );
		tableHeader.appendChild(content);
		content = document.createElement("th");
		content.appendChild( document.createTextNode("Comments") );
		tableHeader.appendChild(content);
	}
	
	var i = (selectedPage-1)*pagNumber;
	var max = selectedPage*pagNumber
	for(; i<reports.length && i<max; i++){
		var row = document.createElement("tr");
		
		content = document.createElement("td");
		content.appendChild( document.createTextNode(getFromXML(reports, i, "entry_date")) );
		row.appendChild(content);
		content = document.createElement("td");
		content.appendChild( document.createTextNode(getFromXML(reports, i, "category")) );
		row.appendChild(content);
		content = document.createElement("td");
		content.appendChild( document.createTextNode(getFromXML(reports, i, "user")) );
		row.appendChild(content);
		content = document.createElement("td");
		content.appendChild( document.createTextNode(getFromXML(reports, i, "description")) );
		row.appendChild(content);
		
		if( reportsType == "solved"){		
			content = document.createElement("td");
			content.appendChild( document.createTextNode(getFromXML(reports, i, "solved_date")) );
			row.insertBefore(content, row.firstChild);
			content = document.createElement("td");
			content.appendChild( document.createTextNode(getFromXML(reports, i, "admin")) );
			row.appendChild(content);
			content = document.createElement("td");
			content.appendChild( document.createTextNode(getFromXML(reports, i, "comments")) );
			row.appendChild(content);
		}
		if( reportsType == "open"){
			row.setAttribute("id", getFromXML(reports, i, "report_id") );
		}
		tableBody.appendChild(row);
	}
	pages = Math.ceil( reports.length / pagNumber );
	if(pages > 1){		
		makePageNav();
	}
	if( reportsType == "open"){
		$("#reports_div tbody tr").click(function(){
			$("#reports_popup").css("display", "flex");		
			$("#nReport_id").val($(this).attr("id"));
		});
	}
	
	getDataCat();
}

function getDataCat(){
	if(window.XMLHttpRequest){   
		xmlhttp = new XMLHttpRequest();
	}else{ // IE6 and below
		xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
	}
	xmlhttp.onreadystatechange = function(){
		if( xmlhttp.readyState == 4 && xmlhttp.status == 200 ){
			updateDataCat(xmlhttp, true);
		}
	};
	xmlhttp.open("GET", "get_categories.php", true);
	xmlhttp.send();
}

function updateDataCat(answer, firstTime){
	if(firstTime){
		var xml = answer.responseXML;	
		categories = [];
		categories = xml.documentElement.getElementsByTagName("category");
	}	
	var table = document.getElementById("categories_table");
	table.removeChild(table.getElementsByTagName("tbody")[0]);
	var tableBody = document.createElement("tbody");
	table.appendChild(tableBody);	
	for(var i=0; i<categories.length ; i++){
		var row = document.createElement("tr");
		var content;
		
		content = document.createElement("td");
		content.appendChild( document.createTextNode(getFromXML(categories, i, "name")) );
		row.appendChild(content);
		row.setAttribute("id", getFromXML(categories, i, "category_id") );
		
		if(getFromXML(categories, i, "category_id") != "1"){
			content = document.createElement("td");
			content.appendChild( document.createTextNode("Edit"));
			content.setAttribute("class", "edit_message edit_but" );
			row.appendChild(content);
			
			content = document.createElement("td");
			content.appendChild( document.createTextNode("Delete"));
			content.setAttribute("class", "edit_message delete_but" );
			row.appendChild(content);
		}else{
			content = document.createElement("td");
			content.appendChild( document.createTextNode(""));			
			row.appendChild(content);
			
			content = document.createElement("td");
			content.appendChild( document.createTextNode(""));
			row.appendChild(content);
		}
		
		tableBody.appendChild(row);	
	}
	
	$("#categories_div tr").click(function(){	
		$("#nCategory_id").val($(this).attr("id"));
	});	
	$(".edit_but").click(function(){
		$("#delete_msg").hide();
		$("#new_name").show();
		$("#action").val("edit");
		$("#categories_popup").css("display", "flex");		
	});
	$("#add_cat_but").click(function(){
		$("#delete_msg").hide();
		$("#new_name").show();
		$("#action").val("new");
		$("#categories_popup").css("display", "flex");	
	});
	$(".delete_but").click(function(){
		$("#delete_msg").show();
		$("#new_name").hide();
		$("#action").val("delete");
		$("#categories_popup").css("display", "flex");	
	});
}

function changeTabs(newType){
	reportsType = newType;
	getData(reportsType);
	resetPageNav();
}

$(document).ready(function(){
	if($("#nCategory_id").val() != ""){
		$("#categories_popup").css("display", "flex");
	}
	$("body").keydown(function(){
		if(event.which == 27){  //If Escape was pressed
			$("#nCategory_id").val("");
			$(".error").text("");			
		}
	});
	$(".x_but").click(function(){			
		$("#nCategory_id").val("");
		$(".error").text("");
	});
	$(".tab").click(function(){
		$(".tab").removeClass("selected");
		$(this).addClass("selected");
		if(reportsType != $(this).attr("id")){
			changeTabs($(this).attr("id"));
		}
	});
	getData();
}); 

