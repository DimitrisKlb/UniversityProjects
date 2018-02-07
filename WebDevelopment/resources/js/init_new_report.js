var map;
var marker = null;
var listener;
var photosNum = 0;

function updateFields(){
	document.getElementById("lat").value = marker.getPosition().lat();
	document.getElementById("lng").value = marker.getPosition().lng();
}

function createTheMarker(location){
	map.panTo(location);
	marker = new google.maps.Marker({
		map: map,
		position:  location,
		animation: google.maps.Animation.DROP,
		draggable: true
	});	
	updateFields();
	google.maps.event.addListener(marker, "dragend", updateFields);	
}

function showPosition(position){
	if(marker!=null){
		marker.setMap(null);
	}
	var geoPos = new google.maps.LatLng(position.coords.latitude, position.coords.longitude);
	createTheMarker(geoPos);	
	google.maps.event.removeListener(listener);	
}

function showError(error){
	switch(error.code){
	case error.PERMISSION_DENIED:
		alert("The request for geolocation was denied.");
		break;
	case error.POSITION_UNAVAILABLE:
		alert("Location information is not available right now.");
		break;
	case error.TIMEOUT:
		alert("The request to get user location timed out.");
		break;
	case error.UNKNOWN_ERROR:
		alert("An unkown error occured.");
		break;
	}
}

function geolocate(){
	if(navigator.geolocation){
		navigator.geolocation.getCurrentPosition(showPosition,showError);
	}else{
		alert("Geolocation is not supported by your browser.");
	}	
}

function initialize(){
	var mapDiv = document.getElementById("google_map");
	mapDiv.style.height = window.innerHeight - 100 + "px";
	var mapOptions = {
		center: new google.maps.LatLng(38.284417, 21.788072),
		zoom: 17,
		mapTypeId: google.maps.MapTypeId.ROADMAP
	};
	map = new google.maps.Map(mapDiv, mapOptions);
	
	var previousLat = document.getElementById("lat").value;
	var previousLng = document.getElementById("lng").value;
	if( previousLat != ""){		
		createTheMarker(new google.maps.LatLng(previousLat, previousLng));		
	}else{	
		listener = google.maps.event.addListener(map, "click", function(event) {
			createTheMarker(event.latLng);				
			google.maps.event.removeListener(listener);
		});	
	}
}
google.maps.event.addDomListener(window, 'load', initialize);

$(document).ready(function(){
	$("input[type=file]").hide();
	$("#nPhoto0").show();
	$("#add_file").click(function(){
		photosNum++;
		$("#nPhoto"+photosNum).show();
	});
	
}); 

