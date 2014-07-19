(function ($) {
  Drupal.behaviors.google_maps_views = {
    attach: function (context, settings) {
      var map = new Drupal.google_maps_api.GoogleMap("map-container", 1);
      
      if (settings.google_maps_views.configuration) {
       map.SetConfigByObject(settings.google_maps_views.configuration);
      }
      
      map.Initialize();
      
      if (settings.google_maps_views.markers) {
    	  map.AddMarkers(settings.google_maps_views.markers, 'node_marker');
    	  map.FitBoundsFromMarkers();
      }
    }
  };
  
})(jQuery);