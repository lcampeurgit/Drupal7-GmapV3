(function ($) {
  Drupal.google_maps_api = {};
  
  Drupal.google_maps_api.Markers = function() {
    this.markers = [];
  }
  
  Drupal.google_maps_api.Markers.prototype.AddMarker = function(marker, name) {
    var new_marker = {
      id : name,
      element : marker
    }
    this.markers.push(new_marker);
  }
  
  Drupal.google_maps_api.GoogleMap = function(dom_id, config) {
    this.dom_id = dom_id;
    this.$dom_element = $('#' + dom_id)[0];
    this.config_id = config;
    this.map;
    this.latitude = -34.397;
    this.longitude = 150.644;
    this.zoom = 8;
    this.pan_control = false;
    this.zoom_control = false;
    this.zoom_control_style = google.maps.ZoomControlStyle.LARGE;
    this.map_type_control = false;
    this.map_type_control_styles = google.maps.MapTypeControlStyle.DROPDOWN_MENU; // HORIZONTAL_BAR
    this.scale_control = false;
    this.street_view_control = false;
    this.overview_map_control = false;
    this.map_type_id = google.maps.MapTypeId.ROADMAP;
    
    this.markers = new Drupal.google_maps_api.Markers();
  };
  
  Drupal.google_maps_api.GoogleMap.prototype.SetConfigByObject = function (configuration) {
    var configs = {
       latitude: configuration.latitude,
       longitude: configuration.longitude,
       zoom: configuration.options.zoom,
       map_type_id: configuration.options.map_type_id,
       pan_control: configuration.controls.pan_control,
       zoom_control: configuration.controls.zoom_control,
       zoom_control_style: configuration.controls.zoom_control_type,
       map_type_control: configuration.controls.map_type_control,
       map_type_control_styles: configuration.controls.map_type_control_styles,
       scale_control: configuration.controls.scale_control,
       street_view_control: configuration.controls.street_view_control,
       overview_map_control: configuration.controls.overview_map_control,
     };
     this.SetConfig(configs);
  }
  
  Drupal.google_maps_api.GoogleMap.prototype.SetConfig = function(configs) {
    if (configs.latitude != undefined) {
      this.latitude = configs.latitude;
    }
    if (configs.longitude != undefined) {
      this.longitude = configs.longitude;
    }
    
    if (configs.zoom != undefined) {
      this.zoom = parseInt(configs.zoom);
    }
    
    if (configs.map_type_id != undefined) {
      if (configs.map_type_id == 0) {
        this.map_type_id = google.maps.MapTypeId.ROADMAP;
      } 
      else if (configs.map_type_id == 1) {
        this.map_type_id = google.maps.MapTypeId.SATELLITE;
      }
      else if (configs.map_type_id == 2) {
        this.map_type_id = google.maps.MapTypeId.HYBRID;
      }
      else if (configs.map_type_id == 3) {
        this.map_type_id = google.maps.MapTypeId.TERRAIN;
      }
    }
    
    if (configs.pan_control != undefined) {
      this.pan_control = configs.pan_control;
    }
    
    if (configs.zoom_control != undefined) {
      this.zoom_control = configs.zoom_control;
    }

    if (configs.zoom_control_style != undefined) {
      if (configs.zoom_control_style == 0) {
        this.zoom_control_style = google.maps.ZoomControlStyle.SMALL;
      } 
      else if (configs.zoom_control_style == 1) {
        this.zoom_control_style = google.maps.ZoomControlStyle.LARGE;
      }
    }
    
    if (configs.map_type_control != undefined) {
      this.map_type_control = configs.map_type_control;
    }
    
    if (configs.map_type_control_styles != undefined) {
      if (configs.map_type_control_styles == 0) {
        this.map_type_control_styles = google.maps.MapTypeControlStyle.DROPDOWN_MENU;
      } 
      else if (configs.map_type_control_styles == 1) {
        this.map_type_control_styles = google.maps.MapTypeControlStyle.HORIZONTAL_BAR;
      }
    }
    
    if (configs.scale_control != undefined) {
      this.scale_control = configs.scale_control;
    }
    
    if (configs.street_view_control != undefined) {
      this.street_view_control = configs.street_view_control;
    }
    
    if (configs.overview_map_control != undefined) {
      this.overview_map_control = configs.overview_map_control;
    }
  };
  
  Drupal.google_maps_api.GoogleMap.prototype.Initialize = function() {
    var mapOptions = {
      center: new google.maps.LatLng(this.latitude, this.longitude),
      zoom: this.zoom,
      mapTypeId: this.map_type_id,
      panControl: this.pan_control,
      zoomControl: this.zoom_control,
      zoomControlOptions: {
        style: this.zoom_control_style
      },
      mapTypeControl: this.map_type_control,
      mapTypeControlOptions: {
        style: this.map_type_control_styles
      },
      scaleControl: this.scale_control,
      streetViewControl: this.street_view_control,
      overviewMapControl: this.overview_map_control
    };
    this.map = new google.maps.Map(this.$dom_element, mapOptions);
  };
  
  Drupal.google_maps_api.GoogleMap.prototype.SetCenter = function(latitude, longitude) {
    this.map.setCenter(new google.maps.LatLng(latitude, longitude));
  };
  
  Drupal.google_maps_api.GoogleMap.prototype.setCenterByAdress = function(address) {
    var that = this;
    geocoder = new google.maps.Geocoder();
    geocoder.geocode({
        address: address
    }, function(results, status) {
        if (status == google.maps.GeocoderStatus.OK) {
          that.map.setCenter(results[0].geometry.location);
        }
    });
  };
  
  Drupal.google_maps_api.GoogleMap.prototype.SetZoom = function(zoom_value) {
    this.map.setZoom(parseInt(zoom_value));
  };
  
  Drupal.google_maps_api.GoogleMap.prototype.SetMapType = function(map_type_value) {
    if (map_type_value == 0) {
      this.map.setMapTypeId(google.maps.MapTypeId.ROADMAP);
    } 
    else if (map_type_value == 1) {
      this.map.setMapTypeId(google.maps.MapTypeId.SATELLITE);
    }
    else if (map_type_value == 2) {
      this.map.setMapTypeId(google.maps.MapTypeId.HYBRID);
    }
    else if (map_type_value == 3) {
      this.map.setMapTypeId(google.maps.MapTypeId.TERRAIN);
    }
  };
  
  Drupal.google_maps_api.GoogleMap.prototype.GetMapType = function() {
    var map_type_id = this.map.getMapTypeId();;
    
    if (map_type_id == 'roadmap') {
      return 0;
    } 
    else if (map_type_id == 'satellite') {
      return 1;
    }
    else if (map_type_id == 'hybrid') {
      return 2;
    }
    else if (map_type_id == 'terrain') {
      return 3;
    }
    return false;
  };
  
  Drupal.google_maps_api.GoogleMap.prototype.DeleteMap = function() {
    delete this.map;
  };
  
  Drupal.google_maps_api.GoogleMap.prototype.AddMarker = function(marker) {
    var markers = [marker];
    this.AddMarkers(markers);
  }
  
  Drupal.google_maps_api.GoogleMap.prototype.AddMarkers = function(markers) {
    for (i in markers) {
      var marker = new google.maps.Marker({
        position: new google.maps.LatLng(markers[i].position.lat, markers[i].position.lng),
        map: this.map,
        title : markers[i].title
      });
      this.markers.AddMarker(marker, markers[i].name);
    }
  }
})(jQuery);