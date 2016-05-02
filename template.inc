<?php

/**
 * @file
 * Helper functions that are not used on every site.
 *
 * The contents of many functions here were copied and pasted from the bodies of
 * hook implementations on the sites they were written for. Documentation of
 * some of those functions' parameters are found on the hooks they were
 * originally implementations of; those hooks are noted by name in the docblock
 * for those functions.
 */

/**
 * Returns an array with my mad responsive breakpoint calculations.
 *
 * Probably soon to be obsoleted by my switch from the less module to
 * Sass + CodeKit.
 *
 * @param int[] $maxima
 *   The largest number of pixels you possibly want for each breakpoint's
 *   smallest width. In other words, "the maximum for each minimum". DO NOT
 *   include the smallest breakpoint; it goes from 0 to one pixel less than
 *   your smallest breakpoint's minimum.
 * @param string
 *   The name of the smallest breakpoint.
 * @param int $columns
 *   The number of columns in the normal layout.
 * @param int $column_factor
 *   The number by which you want all column widths to be divisible.
 *
 * @return array
 *   Data structure representing every breakpoint and their smallest and largest
 *   widths.
 */
function _copy_me_get_breakpoint_dimensions(
	$maxima = array('desktop-large' => 1200, 'desktop-small' => 980, 'tablet' => 768), // The largest that each breakpoint minimum can POSSIBLY go
	$smallest_breakpoint_name = 'mobile', // The breakpoint that begins at zero and extends to the lowest breakpoint in $maxima
	$columns = 12, // Number of columns in the grid layout
	$column_factor = 5 // Number by which all column widths must be divisible
) {
	arsort($maxima); // Sort the breakpoints, largest first

	$breakpoints = array_keys($maxima); // List of breakpoints
	$dimensions = array();

	// The fun begins. For each breakpoint, find the highest column width that is divisible by
	// $column_factor, and also results in a number less than or equal to the $maxima entry
	// when you multiply the column width by $columns.

	foreach($maxima as $breakpoint => $maximum) {
		$dimensions[$breakpoint]['column-x'] = floor($maximum / $columns) - (floor($maximum / $columns) % $column_factor);
	}

	// Now, given the calculated column widths, compute the boundaries of each breakpoint.

	for($i = 0; $i < count($breakpoints); $i++) {
		$boundary = $dimensions[$breakpoints[$i]]['column-x'] * $columns; // The minimum for this breakpoint is just the column width times the number of columns
		$dimensions[$breakpoints[$i]]['min'] = $boundary; // Put it in the actual result array

		if(!empty($breakpoints[$i + 1])) { // If there's another breakpoint with a non-zero minimum
			$dimensions[$breakpoints[$i + 1]]['max'] = $boundary - 1; // The next-largest breakpoint's upper boundary is, of course, one pixel less
		} else { // If this is the smallest breakpoint with non-zero minimum
			$dimensions[$smallest_breakpoint_name]['max'] = $boundary - 1; //
		}
	}

	return $dimensions;
}


/**
 * Gets a value out of a Drupal entity object's field property. Needs work.
 *
 * @param string $entity_type
 *   E.g. 'node'.
 * @param object $entity
 *   Entity object.
 * @param string
 *   Machine name of the field you want to retrieve one of the values of.
 * @param int $delta
 *   Which entry in a multi-value field you want.
 * @param string $img_prop
 *    Which property of an image you want to return. This should be abstracted
 *    out to apply to any field type, not just images.
 *
 * @return string
 *   The raw value or rendered output of the requested field.
 */
function _copy_me_get_field_item_value($entity_type, $entity, $field_name, $delta = 0, $img_prop = 'uri') {
	$field_info = field_info_field($field_name);
	$result = field_get_items($entity_type, $entity, $field_name);

	if($field_info['type'] === 'datetime') {
		return strtotime($result[$delta]['value'] . 'UTC');
	}

	if($field_info['type'] === 'taxonomy_term_reference') {
		return $result[$delta]['tid'];
	}

	$result = field_view_value($entity_type, $entity, $field_name, $result[$delta]);

	if(!empty($result['#theme']) && $result['#theme'] === 'image_formatter') {
		if(empty($result['#item'])) {
			return FALSE;
		}

		if($img_prop === 'uri') {
			$uri = $result['#item']['uri'];

			if(!empty($result['#image_style'])) {
				$uri = image_style_url($result['#image_style'],
				                       image_style_path($result['#image_style'], $result['#item']['uri'])
				);
			}

			return file_create_url($uri);
		} else {
			return $result['#item'][$img_prop];
		}
	}

	return render($result);
}


/**
 * Makes Location module proximity searches more user-friendly, after liberal
 * CSS application
 *
 * @param object &$form
 *   Form object.
 * @param string $el
 *   Machine name of the element of the exposed form to modify.
 */
function _copy_me_prettify_location_search(&$form, $el = 'zip') {
	$form[$el]['postal_code']['#title'] = t('ZIP code');
	$form[$el]['postal_code']['#size'] = 5;
	$form[$el]['search_units']['#type'] = 'hidden';
	$form[$el]['search_distance']['#prefix'] = 'Show locations within ';
	$form[$el]['search_distance']['#suffix'] = ' miles of ';
	$form[$el]['search_distance']['#size'] = 3;
	$form[$el]['search_distance']['#weight'] = 0;
}


/**
 * Wraps menu block submenus in an additional div. This function does nothing
 * without being called in an actual copy_me_menu_link function.
 *
 * Remember to return this function's return value when you call it!
 *
 * @param array $variables
 *   Variables array from theme_menu_link().
 *
 * @return string
 *   Constructed menu link suitable for theme_menu_link() return value.
 *
 * @see theme_menu_link()
 */
function _copy_me_wrap_menu_submenus($variables) {
	$element = $variables ['element'];
	$sub_menu = '';

	if ($element ['#below']) {
	  $sub_menu = '<div class="submenu">' . drupal_render($element ['#below']) . '</div>';
	}

	$options = $element['#localized_options'];
	$options['html'] = TRUE;

	$output = '<span>' . check_plain($element['#title']) . '</span>';

	$output = l($output, $element ['#href'], $options);
	return '<li' . drupal_attributes($element ['#attributes']) . '>' . $output . $sub_menu . "</li>\n";
}


/**
 * Does surgery on Illustrator's default SVG output.
 *
 * @param string $svg
 *   File path or Drupal URI to uploaded SVG file. Only works on local file
 *   systems at this time.
 * @param bool $is_uri
 *   Manual designation of whether a file path or a Drupal URI was supplied.
 *
 * @return string
 *   Final SVG string for document.
 *
 *  @todo Detect what kind of path is supplied, instead of having a parameter
 *    for it.
 *  @todo Change to work with remote file systems.
 */
function _copy_me_get_svg($svg, $is_uri = TRUE) {
	if($is_uri) {
		$svg = drupal_realpath($svg);
	}

	$svg = file_get_contents($svg);
	$svg = substr($svg, strpos($svg, "\n") + 1);
	$svg = preg_replace('/id="Layer_[0-9]*"/', sprintf('class="svg"'), $svg);
	$svg = str_replace('\n', '', $svg);
	$svg = '<!--[if gte IE 9]><!-->' . $svg . '<!--<![endif]-->';
	return $svg;
}


/**
 * Constructs grammatically pleasing titles for News archive views that accept
 * year, month, and day arguments.
 *
 * To be called in copy_me_views_pre_render(), naturally.
 *
 * @param object &$view
 *   $view object from hook_views_pre_render().
 */
function _copy_me_news_view_titles(&$view) {

	$date_parts = array(
		(int)$view->argument['created_year']->argument,
		(int)$view->argument['created_month']->argument,
		(int)$view->argument['created_day']->argument,
	);

	$date_parts = array_filter($date_parts);

	if(empty($date_parts)) {
		return;
	}

	$date_preposition = 'in'; // "No news items in January 2014" versus "No news items on January 8, 2014"

	if(!empty($date_parts[1])) {
		$date_obj = DateTime::createFromFormat('!m', (int)$date_parts[1]);
		$date_parts[1] = $date_obj->format('F') . ' ';
	}

	if(!empty($date_parts[2])) {
		$date_parts[2] = (string)$date_parts[2] . ', ';
		$date_preposition = 'on'; // See comment above
	}

	$date_parts = array_pad($date_parts, 3, '');

	$date_string = sprintf('%s%s%d', $date_parts[1], $date_parts[2], $date_parts[0]);

	if(empty($date_string)) {
		$date_string = '';
	}

	$view->empty['area']->options['content'] = sprintf('There were no news items %s %s.', $date_preposition, $date_string);
}


/**
 * Stops Drupal's default behavior of automatically expanding the children
 * of menu links pointing to your current page.
 *
 * Goes in copy_me_menu_block_tree_alter().
 *
 * @see hook_menu_block_tree_alter()
 */
function _copy_me_suppress_menu_auto_expansion(&$tree, &$config) {

	// Don't do anything if the entire menu block is set to be Expanded
	if($config['expanded']) {
		return;
	}

	foreach($tree as $link_key => &$link) {
		if(!empty($link['below'])) {
			if(!$link['link']['expanded']) { // Only hide children if this link is not manually set to expanded
				$link['below'] = array();
			} else {
				// Call this same function recursively on the children.
				_copy_me_suppress_menu_auto_expansion($link['below'], $config);
			}
		}
	}

	unset($link);
}


/**
 * Adds class to Views list containers (<ul>, <ol>) denoting how many rows are
 * within.
 *
 * Goes in copy_me_preprocess_views_view_list().
 *
 * @see template_preprocess_views_view_list()
 */
function _copy_me_list_view_container_quantity_class(&$variables) {
	$handler = $variables['view']->style_plugin;

	$variables['list_type_prefix'] = sprintf('<%s class="view-rows-%s">', $handler->options['type'], count($variables['rows']));
}

/**
 * Adds a notice to the bottom of the final page of a paginated View.
 *
 * Goes in copy_me_views_pre_render().
 *
 * @param object &$view
 *   View object.
 * @param string[] $list_displays
 *   Array of machine names of displays that show a list of rendered posts
 * @param string[] $nav_displays
 *   Array of machine names of displays that show a summary navigation
 */
function _copy_me_end_of_paged_view(&$view, $list_displays = array('page_1'), $nav_displays = array('attachment_1')) {

	if(in_array($view->current_display, $list_displays)) {

		$current_page = $view->query->pager->current_page + 1;
		$total = (int) $view->query->pager->total_items;
		$per_page = $view->query->pager->options['items_per_page'];

		if($current_page * $per_page >= $total)	{
			ob_start();
			include 'sites/default/themes/aedc/templates/old-newsroom-entries.php'; // FIXME FIXME!
			$notice = ob_get_clean();

			$view->attachment_after .= $notice;
			$view->attachment_before .= $notice;
		}
	}

	if(in_array($view->current_display, $nav_displays))	{
		$obj = new stdClass();
		$obj->node_created_year = '2012 and earlier'; // FIXME FIXME!
		$obj->node_created = '1341014400'; // FIXME FIXME!
		$view->result[] = $obj;
	}
}