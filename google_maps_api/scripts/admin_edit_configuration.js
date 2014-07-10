(function ($) {
  Drupal.behaviors.google_map_api_admin_edit_configurations = {
    attach: function (context, settings) {
      var map = new Drupal.google_maps_api.GoogleMap("google_map", 1);
      
      if (settings.google_maps_api_configurations.configuration) {
       map.SetConfigByObject(settings.google_maps_api_configurations.configuration);
      }
      
      map.Initialize();
      
      $('#edit-maj-center-button').click(function(event) {
        event.preventDefault();
        map.SetCenter($('#edit-latitude').val(), $('#edit-longitude').val());
      });
      
      $('#edit-get-center-button').click(function(event) {
        event.preventDefault();
        var center = map.map.getCenter();
        $('#edit-latitude').val(center.lat());
        $('#edit-longitude').val(center.lng());
      });
      
      $('#edit-maj-options-button').click(function(event) {
        event.preventDefault();
        map.SetZoom($('#edit-zoom').val());
        map.SetMapType($('#edit-map-type-id').val());
      });
      $('#edit-get-options-button').click(function(event) {
        event.preventDefault();
        $('#edit-zoom').val(map.map.getZoom());
        $('#edit-map-type-id').val(map.GetMapType());
      });
      $('#edit-maj-controls-button').click(function(event) {
        event.preventDefault();
        map.DeleteMap();
        map = null;
        var configs = {
          latitude: $('#edit-latitude').val(),
          longitude: $('#edit-longitude').val(),
          zoom: $('#edit-zoom').val(),
          pan_control: $("input[name='pan_control']:checked").val(),
          zoom_control: $("input[name='zoom_control']:checked").val(),
          zoom_control_style: $("input[name='zoom_control_type']:checked").val(),
          map_type_control: $("input[name='map_type_control']:checked").val(),
          map_type_control_styles: $("input[name='map_type_control_styles']:checked").val(),
          scale_control: $("input[name='scale_control']:checked").val(),
          street_view_control: $("input[name='street_view_control']:checked").val(),
          overview_map_control: $("input[name='overview_map_control']:checked").val(),
        };
        map = new Drupal.google_maps_api.GoogleMap("google_map", 1);
        map.SetConfig(configs);
        map.Initialize();
      });
    }
  };
})(jQuery);