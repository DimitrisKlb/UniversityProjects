var reports = [];

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
	xmlhttp.open("GET", "get_my_reports.php", true);
	xmlhttp.send();
}

function updateData(answer, firstTime){
	if(firstTime){
		var xml = answer.responseXML;	
		reports = [];
		reports = xml.documentElement.getElementsByTagName("report");
	}	
	var table = document.getElementById("my_reports_table")
	table.removeChild(table.getElementsByTagName("tbody")[0]);
	tableBody = document.createElement("tbody");
	table.appendChild(tableBody);
	
	var i = (selectedPage-1)*pagNumber;
	var max = selectedPage*pagNumber
	for(; i<reports.length && i<max; i++){
		var row = document.createElement("tr");
		var content;
		content = document.createElement("td");
		content.appendChild( document.createTextNode(getFromXML(reports, i, "entry_date")) );
		row.appendChild(content);
		content = document.createElement("td");
		content.appendChild( document.createTextNode(getFromXML(reports, i, "category")) );
		row.appendChild(content);
		content = document.createElement("td");
		content.appendChild( document.createTextNode(getFromXML(reports, i, "description")) );
		row.appendChild(content);
		
		var solved = getFromXML(reports, i, "solved_date");
		var comments = getFromXML(reports, i, "comments");
		if(solved == ""){
			solved = "-";
			comments = "-";
		}
		content = document.createElement("td");
		content.appendChild( document.createTextNode(solved) );
		row.appendChild(content);
		content = document.createElement("td");
		content.appendChild( document.createTextNode(comments) );
		row.appendChild(content);
		
		tableBody.appendChild(row);
	}

	pages = Math.ceil( reports.length / pagNumber );
	if(pages > 1){		
		makePageNav();
	}
}

getData();