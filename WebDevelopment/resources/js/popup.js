
$(document).ready(function(){	
	$("body").keydown(function(){
		if(event.which == 27){  //If Escape was pressed
			$(".tinted").css("display", "none");
		}
	});
	$(".x_but").click(function(){			
		$(".tinted").css("display", "none");	
	});
});