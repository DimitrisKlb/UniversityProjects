var pages;
var pagNumber;
var selectedPage;
var pageNavDiv; //The div containing the pagenav list
var pageNav; //The pagenav list

function createPageIndex(text, isHoverable, id){
	isHoverable = typeof isHoverable !== "undefined" ? isHoverable : true ;
	id = typeof id !== "undefined" ? id : "" ;
	var item = document.createElement("li");
	item.appendChild( document.createTextNode(text) );
	if(isHoverable){
		item.setAttribute("class", "clickable");
	}
	if( text == selectedPage.toString()){
		item.setAttribute("class", "selected clickable");
	}
	if(id != ""){
		item.setAttribute("id", id);
	}
	pageNav.appendChild(item);		
}

function addPageNavListeners(){	
	$(".clickable").click(function(){
		var val = $(this).attr("id");
		if( val == "next"){
			selectedPage++;
		}else if( val == "prev"){
			selectedPage--;
		}else{
			val = $(this).text();
			selectedPage = parseInt(val);
		}	
		updateData("" , false); //DEBUG!!
	});
}

function resetPageNav(){
	if(pageNavDiv.firstChild){
		pageNavDiv.removeChild(pageNavDiv.firstChild);
	}
	selectedPage = 1;
}

function makePageNav(){	
	pageNav = document.createElement("ul");
	pageNav.setAttribute("id", "pagenav_list");
	if(pageNavDiv.firstChild){
		pageNavDiv.removeChild(pageNavDiv.firstChild);
	}
	pageNavDiv.appendChild(pageNav);
	
	createPageIndex("Σελίδα:", false);
	if( selectedPage > 1){
		createPageIndex("Προηγ.", true, "prev");
	}
	createPageIndex("1");
	if( selectedPage > 3 && pages!=4){
		createPageIndex("...", false);
	}
	if( selectedPage > 2){
		if( selectedPage==pages && pages > 3){
			createPageIndex(selectedPage-2);
		}
		createPageIndex(selectedPage-1);
	}	
	if( selectedPage!=1 && selectedPage!=pages ){
		createPageIndex(selectedPage);
	}	
	if( selectedPage < pages-1){
		createPageIndex(selectedPage+1);
		if(selectedPage==1 && pages>3){
			createPageIndex(selectedPage+2);
		}
	}
	if(selectedPage<pages-2 && pages>4){
		createPageIndex("...", false);
	}	
	createPageIndex(pages);
	if( selectedPage < pages){
		createPageIndex("Επόμ.", true, "next");
	}
	addPageNavListeners();
}

function getFromXML(table, from, key){
	var element = table[from].getElementsByTagName(key)[0];
	if( element.childNodes.length != 0 ){
		return table[from].getElementsByTagName(key)[0].childNodes[0].nodeValue;
	}else{
		return "";
	}
}