(function ($) {
  Drupal.behaviors.gmaps_field_node_edit = {
    attach: function (context, settings) {
      var map = new Drupal.google_maps_api.GoogleMap("google_map", 1);
      
      if (settings.google_maps_api_configurations.configuration) {
       map.SetConfigByObject(settings.google_maps_api_configurations.configuration);
      }
      
      map.Initialize();
      map.gmaps_field_node_edit_getCenterByClick();
      
      $('.gmaps_fields_lat_lng_center_button').click(function(event) {
        event.preventDefault();
        map.SetCenter($('.gmaps_fields_lat_lng_latitude').val(), $('.gmaps_fields_lat_lng_longitude').val());
      });
    }
  };
  
  Drupal.google_maps_api.GoogleMap.prototype.gmaps_field_node_edit_getCenterByClick = function () {
    var behavior = this;
    google.maps.event.addListener(this.map, 'click', function(event) {
      $('.gmaps_fields_lat_lng_latitude').val(event.latLng.lat());
      $('.gmaps_fields_lat_lng_longitude').val(event.latLng.lng());
    });
  };
})(jQuery);