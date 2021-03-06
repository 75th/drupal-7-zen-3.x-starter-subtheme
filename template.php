<?php

// ==============================================================
// ALERT! There are many useful helper functions in template.inc!
// ==============================================================

include_once(__DIR__ . '/template.inc');

/**
 * Overrides theme_all_day_label().
 *
 * Gets rid of the annoying "(All day)" suffix on all-day dates.
 */
function copy_me_date_all_day_label() {
	return '';
}

function copy_me_preprocess_page(&$variables) {

	// Since HTML5 outline structure didn't work out, let's not have the site
	// name be the <h1> anywhere except the home page.

	$variables['site_name_element'] = $variables['is_front'] ? 'h1' : 'div';

	// Inject the insane responsive breakpoint calculation variables into
	// JavaScript for use in matchMedia/mediaCheck

	drupal_add_js(array('breakpoints' => _copy_me_get_breakpoint_dimensions()), 'setting');

	// Add preprocess function "suggestions" for copy_me_preprocess_page__node()
	// and copy_me_preprocess_page__node__NODETYPE()
	if(!empty($variables['node'])) {
		$function_names = array(
			__FUNCTION__ . '__node',
			__FUNCTION__ . '__node__' . $variables['node']->type
		);

		foreach($function_names as $function_name) {
			if(function_exists($function_name)) {
				$function_name($variables);
			}
		}
	}
}

function copy_me_preprocess_entity(&$variables, $hook) {

	if('paragraphs_item' === $variables['entity_type']) {

		// Put id attribute on all paragraph sections
		$variables['attributes_array']['id'] = 'paragraph-' . $variables['paragraphs_item']->item_id;
		$variables['eid'] = $variables['attributes_array']['id'];

		if('sharable_graphic' === $variables['elements']['#bundle']) {

			// Force correct Open Graph image on pages with Sharable Graphics

			$img = file_create_url($variables['field_image'][0]['uri']);
			$eid = $variables['elements']['#entity']->item_id;
			$mid = 'eversity_sharable_graphic_img_' . $eid; // FIXME!
			$tag = array(
				'#id' => $mid,
				'#tag' => 'meta',
				'#attributes' => array(
					'property' => 'og:image',
					'content' => $img,
				),
			);

			drupal_add_html_head($tag, $mid);

			// Construct default tweet text

			if(!empty($variables['field_description'])) {
				$settings = array(
					'tweet_text' => $variables['field_description'][0]['safe_value'],
				);

				// Add tweet text to Drupal.settings.social_sharing.tweet_text
				drupal_add_js(array('social_sharing' => $settings), 'setting');
			}
		}
	}
}


function copy_me_preprocess_node(&$variables, $hook) {
	$variables['submitted'] = date('F j, Y', $variables['created']);

	$variables['theme_hook_suggestions'][] = 'node__' . $variables['view_mode'];
	$variables['theme_hook_suggestions'][] = 'node__' . $variables['type'] . '__' . $variables['view_mode'];

	// Add preprocess function "suggestion" for copy_me_preprocess_node__NODETYPE()
	$function_name = __FUNCTION__ . '__' . $type;

	if(function_exists($function_name)) {
		$function_name($variables, $hook);
	}
}

function copy_me_preprocess_field(&$variables, $hook) {
	// Fix Field Collection theming
	// Issue and workaround code: https://www.drupal.org/node/1137024#comment-8687491
	if ($variables['element']['#entity_type'] == 'field_collection_item') {
		// Check if the bundle name (i.e. the field collection field name) is
		// among the theme hook suggestions.
		$index = array_search('field__' . $variables['element']['#bundle'], $variables['theme_hook_suggestions']);

		// Remove the bundle theme hook suggestion.
		if ($index !== false) {
			unset($variables['theme_hook_suggestions'][$index]);
		}
	}
}


/*
 * Removes ugly "See map:" text from Location module map links
 */
function copy_me_preprocess_location(&$variables) {
	if(!empty($variables['map_link'])) {
		$variables['map_link'] = location_map_link($variables['location'], '');
	}
}

function copy_me_menu_local_tasks(&$variables) {
	$output = '';

	// Add theme hook suggestions for tab type.
	foreach (array('primary', 'secondary') as $type) {
		if (!empty($variables[$type])) {
			foreach (array_keys($variables[$type]) as $key) {
				if (isset($variables[$type][$key]['#theme']) && ($variables[$type][$key]['#theme'] == 'menu_local_task' || is_array($variables[$type][$key]['#theme']) && in_array('menu_local_task', $variables[$type][$key]['#theme']))) {
					$variables[$type][$key]['#theme'] = array('menu_local_task__' . $type, 'menu_local_task');
				}
			}
		}
	}

	if (!empty($variables['primary'])) {
		$variables['primary']['#prefix'] = '<h2 class="element-invisible">' . t('Primary tabs') . '</h2>';
		$variables['primary']['#prefix'] .= '<ul class="tabs primary">';
		$variables['primary']['#suffix'] = '</ul>';
		$output .= drupal_render($variables['primary']);
	}
	if (!empty($variables['secondary'])) {
		$variables['secondary']['#prefix'] = '<h2 class="element-invisible">' . t('Secondary tabs') . '</h2>';
		$variables['secondary']['#prefix'] .= '<ul class="tabs secondary">';
		$variables['secondary']['#suffix'] = '</ul>';
		$output .= drupal_render($variables['secondary']);
	}

	return $output;
}

function copy_me_menu_local_task($variables) {
	$class = '';

	$link = $variables['element']['#link'];
	$link_text = $link['title'];

	if (!empty($variables['element']['#active'])) {
		// Add text to indicate active tab for non-visual users.
		$active = ' <span class="element-invisible">' . t('(active tab)') . '</span>';

		// If the link does not contain HTML already, check_plain() it now.
		// After we set 'html'=TRUE the link will not be sanitized by l().
		if (empty($link['localized_options']['html'])) {
			$link['title'] = check_plain($link['title']);
		}

		$link['localized_options']['html'] = TRUE;
		$link_text = t('!local-task-title!active', array('!local-task-title' => $link['title'], '!active' => $active));

		$class = 'active';

	}

	return '<li' . ($class ? ' class="' . $class . '"' : '') . '>' . l($link_text, $link['href'], $link['localized_options']) . "</li>\n";
}

/**
 * Provide the breakpoint variables to Less.
 *
 * Obsolete with new Sass method.
 */
function copy_me_less_variables_alter(&$less_variables, $system_name) {
	$new_variables = array();

	$dimensions = _copy_me_get_breakpoint_dimensions();

	foreach($dimensions as $breakpoint => $vars) {
		foreach($vars as $var_name => $value) {
			$new_variables['@' . $breakpoint . '-' . $var_name] = sprintf('%spx', $value);
		}
	}

	$less_variables = array_merge($less_variables, $new_variables);
}