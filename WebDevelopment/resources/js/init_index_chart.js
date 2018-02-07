/*----------------------------------------Statisticks----------------------------------------*/
var chart;
var data;
var options;
var reportsTotal;
var reportsSolved;
var averageSolveTime;

function updateChartInfo(){
	var timeUnits = "days"; 
	reportsTotal = parseInt(statistics.getElementsByTagName("total_reports")[0].childNodes[0].nodeValue);
	reportsSolved = parseInt(statistics.getElementsByTagName("solved_reports")[0].childNodes[0].nodeValue);
	averageSolveTime = parseFloat(statistics.getElementsByTagName("average_solve_time")[0].childNodes[0].nodeValue);
	
	if(averageSolveTime < 1){
		averageSolveTime *= 24;
		timeUnits = "hours";
	}
	
	data = google.visualization.arrayToDataTable([
		["Reports", "Number"],
		["Λυμένες", reportsSolved],
		["Ανοιχτές", reportsTotal-reportsSolved]
	]);
	options = {
		legend: {
			position: 'top', 
			textStyle: {color: "black", fontSize: 16}			
		},
		backgroundColor: "transparent",			
		slices:{
            0: { color: "#0E1ECF" },
            1: { color: "#A00D0D" }
		},
		titleTextStyle:{color:"black", fontSize: 16},
		title: "Συνολικές Αναφορές: " + reportsTotal + "\n" +
			" Μέσος χρόνος επίλυσης: " + averageSolveTime + " " + timeUnits
	};
}

function updateChart(){
	updateChartInfo();
	chart.draw(data, options);
}

function makeChart() {	
	updateChartInfo();
	chart = new google.visualization.PieChart(document.getElementById("statistics_div"));
	chart.draw(data, options);
}

$(document).ready(function(){
	setTimeout(function(){ 
		google.load("visualization", "1",{"packages":["corechart"], "callback" : makeChart} );  
	}, 1); 
}); 


