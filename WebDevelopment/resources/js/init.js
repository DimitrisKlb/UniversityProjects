var container = document.getElementById("navs_container");
var dropDown = document.getElementById("drop");	
var menuBar = document.getElementById("general_bar");
var otherBar = document.getElementById("user_bar");
if(otherBar == null){
	otherBar = document.getElementById("connect_bar");
}	

function showDropMenu(mouserOver){
	var menuBar = document.getElementById("general_bar");
	if(mouserOver){	
		menuBar.style.display = "block";
	}else{
		menuBar.style.display = "none";
	}	
}

function calcLimit(){
	var newLimit = 1024;        //Default limit value
	if( dropDown.offsetWidth == 0 ){
		newLimit = menuBar.offsetWidth + otherBar.offsetWidth + 155; 
	}
	return newLimit;
}

var limit = calcLimit();

function resize(){
	if( window.innerWidth <= limit ){	
		menuBar.style.display = "none";
		dropDown.style.display = "block";
		dropDown.appendChild(menuBar);
	}else{
		menuBar.style.display = "flex";
		dropDown.style.display = "none";		
		container.insertBefore(menuBar, container.firstChild);	
		limit = calcLimit();
	}
}

resize();
document.body.setAttribute("onresize", "resize()");
s = document.URL;
s = s.slice(s.lastIndexOf("/")+1,-4);	
document.getElementById(s).setAttribute("class","button active");