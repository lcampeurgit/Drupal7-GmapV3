<?php
/**
 * Hook_menu callback via drupal_get_form.
 */
function google_maps_api_configurations($form, &$form_state) {
  if (isset($form_state['storage']['gmcids'])) {
    return google_maps_api_configurations_confirm($form_state);
  }
  
  
  $header = array(
      'name' => array('data' => t('Name'), 'field' => 'name'),
      'config_name' => array('data' => t('Config Name'), 'field' => 'config_name'),
      'actions' => array()
  );
  
  $gmcids = db_select('google_maps_configurations', 'configs')
          ->fields('configs', array('gmcid'))
          ->extend('TableSort')
          ->orderByHeader($header)
          ->execute()
          ->fetchCol();
  $configurations = google_maps_api_configurations_load_multiple($gmcids);
  $default_configuration = google_maps_api_variables_select('default_configuration');
  
  $options = array();
  foreach ($configurations as $configuration) {
    $options[$configuration->gmcid] = (array)$configuration;
    $update_link = 'admin/structure/googlemaps/configurations/' . $configuration->config_name . '/edit';
    $options[$configuration->gmcid]['name'] = l($options[$configuration->gmcid]['name'], $update_link, array('html' => TRUE));
    $options[$configuration->gmcid]['name'] .= $configuration->gmcid == $default_configuration ? ' - <em>' . t('Default configuration') . '</em>' : '';
    $options[$configuration->gmcid]['config_name'] = l($options[$configuration->gmcid]['config_name'], $update_link);
    
    $actions = array(
        l(t('Modify'), $update_link)
    );
    
    $options[$configuration->gmcid]['actions'] = implode(' - ', $actions);
  }
  
  $form['configurations'] = array(
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => $options,
      '#empty' => t('No configurations set')
  );
  
  $form['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete')
  );
  
  return $form;
}

/**
 * Return confirm form for delete configurations
 * 
 * @param array $form_state
 * 
 * @return array
 */
function google_maps_api_configurations_confirm($form_state) {
  $form = array(
    'gmcids' => array(
      '#type' => 'value',
      '#value' => $form_state['storage']['gmcids']
    )
  );
  $question = t("Are you sure to delete these configurations ?");
  $path = 'admin/structure/googlemaps';
  $description = t("This action cannot be undone.");
  $yes = t("Delete");
  $no = t("Cancel");
  return confirm_form($form, $question, $path, $description, $yes, $no);
}

/**
 * Submit callback for google_maps_api_configurations
 * 
 * If configurations are select for delete, rebuild the form to show confirm form
 * If it's confirm form submit, delete the configurations 
 */
function google_maps_api_configurations_submit($form, &$form_state) {
  // Simple form submit
  if (isset($form_state['values']['configurations'])) {
    $gmcids = $form_state['values']['configurations'];
    $gmcids_to_delete = array();
    foreach ($gmcids as $gmcid) {
      if ($gmcid !== 0) {
        $gmcids_to_delete[] = $gmcid;
      }
    }

    if (! empty($gmcids_to_delete)) {
      $form_state['rebuild'] = TRUE;
      $form_state['storage'] = array(
        'gmcids' => $gmcids_to_delete
      );
    }
    else {
      drupal_set_message(t('No configurations selected'), 'warning');
    }
  }
  // Confirm form submit
  elseif (isset($form_state['values']['gmcids'])) {
    $result = google_maps_api_configurations_delete($form_state['values']['gmcids']);
    if ($result) {
      drupal_set_message(t('Configurations deleted'));
    }
    else {
      drupal_set_message(t('Error while deleting configurations'), 'error');
    }
  }
}

/**
 * Hook_menu callback via drupal_get_form.
 */
function google_maps_api_configuration_generic($form, &$form_state) {
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save')
  );
  
  $form['api'] = array(
    '#type' => 'fieldset',
    '#title' => 'API Informations',
    '#collapsible' => TRUE,
    '#collasped' => FALSE
  );
  
  $form['api']['key'] = array(
      '#type' => 'textfield',
      '#title' => 'Key',
      '#required' => TRUE,
      '#default_value' => google_maps_api_variables_select('key')
  );
  
  $form['configuration'] = array(
    '#type' => 'fieldset',
    '#title' => 'Configurations settings',
    '#collapsible' => TRUE,
    '#collasped' => FALSE
  );
  $configurations = db_select('google_maps_configurations', 'configs')
          ->fields('configs', array('gmcid', 'name'))
          ->orderBy('name')
          ->execute()
          ->fetchAllKeyed();
  $form['configuration']['default_configuration'] = array(
      '#type' => 'select',
      '#title' => 'Default configuration',
      '#required' => TRUE,
      '#options' => $configurations,
      '#default_value' => google_maps_api_variables_select('default_configuration')
  );
  
  $form['submit2'] = array(
    '#type' => 'submit',
    '#value' => t('Save')
  );
  return $form;
}

/**
 * Validate form callback for google_maps_api_configuration_generic
 */
function google_maps_api_configuration_generic_validate($form, &$form_state) {
  
}

/**
 * Submit form callback for google_maps_api_configuration_generic
 */
function google_maps_api_configuration_generic_submit($form, &$form_state) {
  $variables = array(
      'key' => array(
          'name' => 'key',
          'value' => $form_state['values']['key'],
          'type' => 'string'
      ),
      'default_configuration' => array(
          'name' => 'default_configuration',
          'value' => $form_state['values']['default_configuration'],
          'type' => 'integer'
      ),
  );
  $results = google_maps_api_variables_save($variables);
  
  foreach ($results as $name => $result) {
    if ($result) {
      drupal_set_message(t('Variable "@name" saved', array('@name' => $name)));
    } 
    else {
      drupal_set_message(t('An error occured while saving @name', array('@name' => $name)), 'error');
    }
  }
  
}

/**
 * Hook_menu callback via drupal_get_form.
 */
function google_maps_api_configurations_edit($form, &$form_state, $configuration = NULL) {
  $form['#attached'] = array(
      'js' => array(
        'https://maps.googleapis.com/maps/api/js?key=' . google_maps_api_variables_select('key') => array('type' => 'external'),
        drupal_get_path('module', 'google_maps_api') . '/api/google_maps_api.js' => array('type' => 'file'),
        drupal_get_path('module', 'google_maps_api') . '/scripts/admin_edit_configuration.js' => array('type' => 'file'),
        array('data' => array('google_maps_api_configurations' => array('configuration' => $configuration)), 'type' => 'setting')
      ),
      'css' => array(
        drupal_get_path('module', 'google_maps_api') . '/css/admin_edit_configuration.css' => array('type' => 'file'),
      )
  );
  
  $form['configuration'] = array(
      '#type' => 'value',
      '#value' => $configuration
  );
  
  $form['name'] = array(
      '#type' => 'textfield',
      '#title' => t('Name'),
      '#required' => TRUE,
      '#default_value' => $configuration === NULL ? NULL : $configuration->name
  );
  $form['config_name'] = array(
      '#type' => 'machine_name',
      '#title' => t('Machine name'),
      '#machine_name' => array(
        'exists' => 'google_maps_api_configurations_config_name_exists',
        'source' => array('name'),
      ),
      '#maxlength' => 32,
      '#required' => TRUE,
      '#default_value' => $configuration === NULL ? NULL : $configuration->config_name,
      '#disabled' => $configuration === NULL ? FALSE : TRUE
  );
  $default_configuration = google_maps_api_get_default_configuration();
  if ($configuration === NULL) {
    $configuration = $default_configuration;
  }
  elseif($configuration->gmcid !=  google_maps_api_variables_select('default_configuration') && $default_configuration !== NULL) {
    $form['erase'] = array(
        '#type' => 'submit',
        '#value' => t('Erase with default configuration'),
    );
  }
  
  $form['google_name_fieldset'] = array(
      '#type' => 'fieldset',
      '#title' => t('Example'),
      '#collapsible' => FALSE,
      '#collasped' => FALSE
  );
  $form['google_name_fieldset']['google_name'] = array(
      '#type' => 'markup',
      '#markup' => '<div id="google_map"></div>'
  );
  $form['position'] = array(
    '#type' => 'fieldset',
    '#title' => t('Positions'),
    '#collapsible' => TRUE,
    '#collasped' => FALSE
  );
  $form['position']['latitude'] = array(
      '#type' => 'textfield',
      '#title' => t('Latitude'),
      '#required' => TRUE,
      '#default_value' => $configuration === NULL ? NULL : $configuration->latitude
  );
  $form['position']['longitude'] = array(
      '#type' => 'textfield',
      '#title' => t('Longitude'),
      '#required' => TRUE,
      '#default_value' => $configuration === NULL ? NULL : $configuration->longitude
  );
  $form['position']['maj_center_button'] = array(
      '#type' => 'button',
      '#value' => t('Update center'),
  );
  
  $form['position']['get_center_button'] = array(
      '#type' => 'button',
      '#value' => t('Get center from example map'),
  );
  
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Options'),
    '#collapsible' => TRUE,
    '#collasped' => FALSE
  );
  $form['options']['zoom'] = array(
      '#type' => 'select',
      '#options' => range(0, 18),
      '#title' => t('Default Zoom'),
      '#prefix' => '<div class="zoom options">',
      '#suffix' => '</div>',
      '#required' => TRUE,
      '#default_value' => $configuration === NULL ? 10 : $configuration->options['zoom']
  );
  $form['options']['map_type_id'] = array(
      '#type' => 'select',
      '#options' => array(
        0 => t('Roadmap'),
        1 => t('Satellite'),
        2 => t('Hybrid'),
        3 => t('Terrain')
      ),
      '#title' => t('Default Map Type'),
      '#required' => TRUE,
      '#prefix' => '<div class="map-type options">',
      '#suffix' => '</div>',
      '#default_value' => $configuration === NULL ? 0 : $configuration->options['map_type_id']
  );
  
  $form['options']['maj_options_button'] = array(
      '#type' => 'button',
      '#value' => t('Update options'),
      '#prefix' => '<div class="button">',
  );
  $form['options']['get_options_button'] = array(
      '#type' => 'button',
      '#value' => t('Get options from example map'),
      '#suffix' => '</div>',
  );
  
  $form['controls'] = array(
    '#type' => 'fieldset',
    '#title' => t('Controls'),
    '#collapsible' => TRUE,
    '#collasped' => FALSE
  );
  $form['controls']['zoom_control'] = array(
      '#type' => 'radios',
      '#options' => array(0 => t('No'), 1 => t('Yes')),
      '#title' => t('Zoom Control'),
      '#required' => TRUE,
      '#prefix' => '<div class="zoom control">',
      '#default_value' => $configuration === NULL ? 0 : $configuration->controls['zoom_control']
  );
  $form['controls']['zoom_control_type'] = array(
      '#type' => 'radios',
      '#options' => array(0 => t('Mini'), 1 => t('Large')),
      '#title' => t('Zoom Control Type'),
      '#required' => TRUE,
      '#suffix' => '</div>',
      '#default_value' => $configuration === NULL ? 0 : $configuration->controls['zoom_control_type']
  );
  $form['controls']['map_type_control'] = array(
      '#type' => 'radios',
      '#options' => array(0 => t('No'), 1 => t('Yes')),
      '#title' => t('Map Type Control'),
      '#required' => TRUE,
      '#prefix' => '<div class="map-type control">',
      '#default_value' => $configuration === NULL ? 0 : $configuration->controls['map_type_control']
  );
  $form['controls']['map_type_control_styles'] = array(
      '#type' => 'radios',
      '#options' => array(0 => t('Select list'), 1 => t('Bar')),
      '#title' => t('Map Type Control Style'),
      '#required' => TRUE,
      '#suffix' => '</div>',
      '#default_value' => $configuration === NULL ? 0 : $configuration->controls['map_type_control_styles']
  );
  $form['controls']['pan_control'] = array(
      '#type' => 'radios',
      '#options' => array(0 => t('No'), 1 => t('Yes')),
      '#title' => t('D-Pad Control'),
      '#required' => TRUE,
      '#prefix' => '<div class="pan control">',
      '#suffix' => '</div>',
      '#default_value' => $configuration === NULL ? 0 : $configuration->controls['pan_control']
  );
  $form['controls']['scale_control'] = array(
      '#type' => 'radios',
      '#options' => array(0 => t('No'), 1 => t('Yes')),
      '#title' => t('Scale Control'),
      '#required' => TRUE,
      '#prefix' => '<div class="scale control">',
      '#suffix' => '</div>',
      '#default_value' => $configuration === NULL ? 0 : $configuration->controls['scale_control']
  );
  $form['controls']['street_view_control'] = array(
      '#type' => 'radios',
      '#options' => array(0 => t('No'), 1 => t('Yes')),
      '#title' => t('Street View Control'),
      '#required' => TRUE,
      '#prefix' => '<div class="street-view control">',
      '#suffix' => '</div>',
      '#default_value' => $configuration === NULL ? 0 : $configuration->controls['street_view_control']
  );
  $form['controls']['overview_map_control'] = array(
      '#type' => 'radios',
      '#options' => array(0 => t('No'), 1 => t('Yes')),
      '#title' => t('Overview Map Control'),
      '#required' => TRUE,
      '#prefix' => '<div class="overview-map control">',
      '#suffix' => '</div>',
      '#default_value' => $configuration === NULL ? 0 : $configuration->controls['overview_map_control']
  );
  
  $form['controls']['maj_controls_button'] = array(
      '#prefix' => '<div class="button">',
      '#suffix' => '</div>',
      '#type' => 'button',
      '#value' => t('Update controls'),
  );
  
  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
  );
  return $form;
}

/**
 * Validate form callback for google_maps_api_configurations_edit
 */
function google_maps_api_configurations_edit_validate($form, &$form_state) {
}

/**
 * Submit form callback for google_maps_api_configurations_edit
 */
function google_maps_api_configurations_edit_submit($form, &$form_state) {
  if ($form_state['clicked_button']['#id'] === 'edit-submit') {
    $options = array();
    $options['zoom'] = $form_state['values']['zoom'];
    $options['map_type_id'] = $form_state['values']['map_type_id'];

    $controls = array();
    $controls['zoom_control'] = $form_state['values']['zoom_control'];
    $controls['zoom_control_type'] = $form_state['values']['zoom_control_type'];
    $controls['map_type_control'] = $form_state['values']['map_type_control'];
    $controls['map_type_control_styles'] = $form_state['values']['map_type_control_styles'];
    $controls['pan_control'] = $form_state['values']['pan_control'];
    $controls['scale_control'] = $form_state['values']['scale_control'];
    $controls['street_view_control'] = $form_state['values']['street_view_control'];
    $controls['overview_map_control'] = $form_state['values']['overview_map_control'];

    if (isset($form_state['values']['configuration'])) {
      $configuration = $form_state['values']['configuration'];
    }
    else {
      $configuration = new stdClass();
      $configuration->gmcid = NULL;
    }
    $configuration->name = $form_state['values']['name'];
    $configuration->config_name = $form_state['values']['config_name'];
    $configuration->latitude = $form_state['values']['latitude'];
    $configuration->longitude = $form_state['values']['longitude'];
    $configuration->options = $options;
    $configuration->controls = $controls;
  }
  elseif ($form_state['clicked_button']['#id'] === 'edit-erase') {
    if (isset($form_state['values']['configuration'])) {
      $configuration = $form_state['values']['configuration'];
    }
    else {
      $configuration = new stdClass();
      $configuration->gmcid = NULL;
      $configuration->name = $form_state['values']['name'];
    }
    $default_configuration = google_maps_api_get_default_configuration();
    google_maps_api_merge_configuration($configuration, $default_configuration, TRUE);
  }
  $presave = google_maps_api_configurations_presave($configuration);

  $save = FALSE;
  
  if ($presave) {
    $save = google_maps_api_configurations_save($configuration);
  }
  
  if (! $save) {
    drupal_set_message(t('An error occured while saving @name', array('@name' => $configuration->name)), 'error');
  }
  else {
    drupal_set_message(t('Configuration @name saved.', array('@name' => $configuration->name)));
    $form_state['redirect'] = 'admin/structure/googlemaps/configurations';
  }
}
