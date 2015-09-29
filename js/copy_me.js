(function($) {

	$(document).ready(function() {

		// The mediaCheck library lets you execute code when entering and/or leaving media queries.
		//
		// The same breakpoints used by Less are available as JavaScript variables under Drupal.settings.breakpoints.

		mediaCheck({
			media: '(max-width: ' + Drupal.settings.breakpoints.mobile.max + 'px)',
			entry: function() {

			},
			exit: function() {

			},
			both: function() {

			}
		});

	});
})(jQuery);

