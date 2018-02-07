/*----------------------------------------Google Map----------------------------------------*/
var updateInterval = 500;
var map;
var markers = [];
var xml;
var reports = [];
var statistics = [];
var infoWindow = null;

google.maps.Marker.prototype.theInfoText = "";
google.maps.Marker.prototype.theReportId = "";

function initialize(){
	var mapDiv = document.getElementById("google_map");
	mapDiv.style.height = window.innerHeight - 270 + "px";
	var mapOptions = {
		center: new google.maps.LatLng(38.284417, 21.788072),
		zoom: 16,
		mapTypeId: google.maps.MapTypeId.ROADMAP
	};
	map = new google.maps.Map(mapDiv, mapOptions);
}
google.maps.event.addDomListener(window, 'load', initialize);


function showMarkerInfo(theMarker){
	if(infoWindow != null){
		infoWindow.close();
	}
	infoWindow = new google.maps.InfoWindow({
		content: theMarker.theInfoText
	});
	infoWindow.open(map, theMarker);
}

function jumpToMarker(id, theMarker){
	if(typeof theMarker !== "string"){
		id = theMarker.theReportId;
	}
	// Make the Selected Table Row Glow
	var reportsTable = document.getElementById("reports_table").getElementsByTagName("tbody")[0];
	var rows = reportsTable.getElementsByTagName("tr");
	for(var i=0; i<rows.length; i++){
		rows[i].removeAttribute("class");
		if( markers[i].theReportId == id){
			// Pan to the associated marker
			map.panTo(markers[i].getPosition());
			map.setZoom(16);			
			showMarkerInfo(markers[i]);
		}
	}
	document.getElementById(id).setAttribute("class", "glow");
}

function getFromReportsXML(from, key){
	var element = reports[from].getElementsByTagName(key)[0];
	if( element.childNodes.length !=0 ){
		return reports[from].getElementsByTagName(key)[0].childNodes[0].nodeValue;
	}else{
		return "";
	}
}

function addMarker(from){
	// Create the Markers
	var lat = parseFloat( getFromReportsXML(from, "lat"));
	var lng = parseFloat( getFromReportsXML(from, "lng"));
	var location = new google.maps.LatLng(lat, lng);
	var newMarker = new google.maps.Marker({
		map: map,
		animation: google.maps.Animation.DROP,
		position: location
	});	
	newMarker.theReportId = getFromReportsXML(from, "report_id");	
	// Create the Info Windows' Text and Add it to the marker
	var infoText = "<b>Καταχωρήθηκε: </b>";	
	infoText += getFromReportsXML(from, "entry_date");
	infoText += "<br/><b>Κατηγορία: </b>";
	infoText += getFromReportsXML(from, "category");
	infoText += "<br/><b>από: </b>";
	infoText += getFromReportsXML(from, "firstname");
	infoText += " ";
	infoText += getFromReportsXML(from, "lastname");

	newMarker.theInfoText = infoText;	
	google.maps.event.addListener(newMarker, "click", function(){
		jumpToMarker(0, this);
	});	
	// Add the new marker into the markers array
	markers.unshift(newMarker);
	// Create the corresponding entry for the reports' table
	var reportsTable = document.getElementById("reports_table").getElementsByTagName("tbody")[0];
	var row = document.createElement("tr");
	var content;
	
	content = document.createElement("td");
	content.appendChild( document.createTextNode(getFromReportsXML(from, "entry_date")) );
	row.appendChild(content);
	content = document.createElement("td");
	content.appendChild( document.createTextNode(getFromReportsXML(from, "category")) );
	row.appendChild(content);
	content = document.createElement("td");
	var name = getFromReportsXML(from, "firstname") +" "+ getFromReportsXML(from, "lastname");
	content.appendChild( document.createTextNode(name) );
	row.appendChild(content);
	
	content = document.createElement("td");
	var maxLength = 70;	
	var description = getFromReportsXML(from, "description");
	if( description.length > maxLength ){
		description = description.substr(0, maxLength) + "..";	
	}	
	content.appendChild( document.createTextNode(description) );
	row.appendChild(content);		

	row.setAttribute("id", newMarker.theReportId );
	row.setAttribute("onclick","jumpToMarker(" + newMarker.theReportId + ", '')");
	reportsTable.insertBefore(row, reportsTable.firstChild);	
}

function removeMarker(){
	markers.pop().setMap(null);
	var reportsTable = document.getElementById("reports_table").getElementsByTagName("tbody")[0];
	var rows = reportsTable.getElementsByTagName("tr");	
	reportsTable.removeChild( rows[rows.length-1] );
}

function getLastReports(firstTime){
	firstTime = typeof firstTime !== "undefined" ? firstTime : false ;
	if(window.XMLHttpRequest){   
		xmlhttp = new XMLHttpRequest();
	}else{ // IE6 and below
		xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
	}
	xmlhttp.onreadystatechange = function(){
		if( xmlhttp.readyState == 4 && xmlhttp.status == 200 ){
			updateLastReports(xmlhttp, firstTime);
		}
	};
	xmlhttp.open("GET", "get_last_reports.php", true);
	xmlhttp.send();
}

function updateLastReports(answer, firstTime){		
	xml = answer.responseXML;
	reports = [];
	reports = xml.documentElement.getElementsByTagName("report");
	statistics = xml.documentElement.getElementsByTagName("statistics")[0];
	if( firstTime == true ){
		for(var i=0; i<markers.length; i++){
			markers[i].setMap(null);
		}
		markers = [];
		for(var i=reports.length-1; i>=0; i--){
			addMarker(i);
		}
	}else{
		var firstReportId = markers[0].theReportId; //ReportId of the most-recent last report
		var newReports = 0;
		for(var i=0; i<reports.length; i++){	
			var currentId = getFromReportsXML(i, "report_id");
			if( currentId != firstReportId ){ //The is a new, more recent report
				newReports++;
			}else{
				break;
			}
		}
		for(var i=newReports-1; i>=0; i--){
			addMarker(i);
			removeMarker();
		}
	}
	
	updateChart();
}

$(document).ready(function(){		
	getLastReports(true); 
	setInterval(getLastReports, updateInterval);
});
