$ = jQuery.noConflict(true);

$(document).ready(function() {

	responder = jRespond([
		{
			'label': 'desktop-large',
			'enter': 1200,
			'exit': 100000
		},{
			'label': 'desktop-small',
			'enter': 960,
			'exit': 1199
		},{
			'label': 'tablet',
			'enter': 768,
			'exit': 959
		},{
			'label': 'mobile-large',
			'enter': 500,
			'exit': 767
		},{
			'label': 'mobile-small',
			'enter': 0,
			'exit': 499
		}
	]);
	
});