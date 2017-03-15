
$( document ).on( "mobileinit", function() {
  	// Optimize JQuery Mobile
//	$.mobile.page.prototype.options.domCache = true;
	$.mobile.useFastClick = false;
	$.mobile.buttonMarkup.hoverDelay = 0;
	$.mobile.defaultPageTransition = 'none';
	$.mobile.defaultDialogTransition = 'none';
	$.mobile.allowCrossDomainPages = true;
	$.mobile.hoverDelay = 0;

	//Doesn't work
//	console.log("Animations:" + $.fx.off);
//	$.fx.off = false;
//	console.log("Animations:" + $.fx.off);


});