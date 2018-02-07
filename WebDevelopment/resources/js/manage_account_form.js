
$(document).ready(function(){
		var previousChoice = $("#choice").val();
		$("#" + previousChoice).show();
		if( previousChoice !== ""){
			$("#submit_but").show();
		}			
		$(".change_but").click(function(){
			$("#change_message").text("");
			var fieldset = $(this).attr("id").slice(0,-4);
			$("#choice").val(fieldset);
			$("fieldset").hide();
			$("#" + fieldset).show();
			$("#submit_but").show();
		});
	}); 
