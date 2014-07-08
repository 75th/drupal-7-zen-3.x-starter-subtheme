<?php

/**
 * Code to make promo links use background-images instead of <img> elements.
 */
?>

<?php if (!empty($title)): ?>
	<h3><?php print $title; ?></h3>
<?php endif; ?>
<?php foreach ($rows as $id => $row): ?>
	<div<?php if ($classes_array[$id]) { print ' class="' . $classes_array[$id] .'"';  } ?>>
		<?php print $row; ?>
	</div>

	<?php 
		$image = file_create_url($view->result[$id]->field_field_image[0]['raw']['uri']);
		$style = sprintf('.views-row-%s .views-field-field-image { background-image: url(%s)} ', ((int)$id + 1), $image);
		$options = array(
			'type' => 'inline',
		);

		drupal_add_css($style, $options);
	?>
<?php endforeach; ?>