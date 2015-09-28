<?php

// ==============================================================
// ALERT! There are many useful helper functions in template.inc!
// ==============================================================

include_once('./template.inc');

function copy_me_date_all_day_label() {
	return '';
}

function copy_me_preprocess_page(&$variables) {
	$variables['site_name_element'] = $variables['is_front'] ? 'h1' : 'div';
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
			$mid = 'eversity_sharable_graphic_img_' . $eid;
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