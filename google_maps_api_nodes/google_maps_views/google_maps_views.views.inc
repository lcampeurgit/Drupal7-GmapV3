<?php
/**
 * Implement hook_views_plugins
 */
function google_maps_views_views_plugins() {
	$plugins = array(
		'style' => array(
			'google_maps_views_plugins' => array(
				'parent' => 'table',
				'title' => t('Show google map'),
				'help' => t("Show nodes as marker in a google map"),
				'path' => drupal_get_path('module', 'google_maps_views') . '/plugins',
				'handler' => 'views_plugin_style_google_maps_views_plugins',
				'theme' => 'google_maps_views',
				'theme path' => drupal_get_path('module', 'google_maps_views'),
				'uses fields' => FALSE,
				'uses options' => TRUE,
				'type' => 'normal',
				'even empty' => TRUE
			)
		)
	);
	return $plugins;
}
