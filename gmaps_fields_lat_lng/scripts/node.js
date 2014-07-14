(function ($) {
  Drupal.behaviors.gmaps_field_node_edit = {
    attach: function (context, settings) {
      var map = new Drupal.google_maps_api.GoogleMap("google_map", 1);
      
      if (settings.gmaps_fields_latlng.configuration) {
       map.SetConfigByObject(settings.gmaps_fields_latlng.configuration);
      }
      
      map.Initialize();
      
      if (settings.gmaps_fields_latlng.marker) {
       map.AddMarker(settings.gmaps_fields_latlng.marker, 'node_marker');
      }
    }
  };
  
})(jQuery);