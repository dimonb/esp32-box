#ifndef HELPERS_H
#define HELPERS_H

#include "lvgl.h"
#include "esphome/core/color.h"
#include "esphome/core/defines.h"

lv_color_t lv_color_make(esphome::Color color) {
  return lv_color_make(color.r, color.g, color.b);
}

template<typename T>
lv_color_t co2_color(T co2) {
  return co2 > 1500 ? lv_color_make(id(red)) : co2 > 1000 ? lv_color_make(id(amber)) : lv_color_make(id(green));
}

std::string get_weather_icon(const std::string &condition) {
  if(condition == "clear-night") {
    return "\U000F0594";  // weather_night
  } else if(condition == "cloudy") {
    return "\U000F0590";  // weather_cloudy
  } else if(condition == "exceptional") {
    return "\U000F0599";  // weather_sunny
  } else if(condition == "fog") {
    return "\U000F0591";  // weather_fog
  } else if(condition == "hail") {
    return "\U000F0592";  // weather_hail
  } else if(condition == "lightning-rainy") {
    return "\U000F0593";  // weather_lightning
  } else if(condition == "lightning") {
    return "\U000F0593";  // weather_lightning
  } else if(condition == "partlycloudy") {
    return "\U000F0595";  // weather_partly_cloudy
  } else if(condition == "pouring") {
    return "\U000F0596";  // weather_pouring
  } else if(condition == "rainy") {
    return "\U000F0597";  // weather_rainy
  } else if(condition == "snowy-rainy") {
    return "\U000F0598";  // weather_snowy
  } else if(condition == "snowy") {
    return "\U000F0598";  // weather_snowy
  } else if(condition == "sunny") {
    return "\U000F0599";  // weather_sunny
  } else if(condition == "windy-variant") {
    return "\U000F059E";  // weather_windy_variant
  } else if(condition == "windy") {
    return "\U000F059D";  // weather_windy
  } else {
    return "\U000F0599";  // weather_sunny
  }    
}

std::vector<lv_point_t> get_weather_points() {
  return {
    {5, 5},
    {70, 70},
    {120, 10},
  };
}

static float points[24] = {3, 11.4, 12.1, 13.4, 23.4, 22.4, 25.4, 23.4, 23.4, 23.4, 23.4, 16.4, 
                               std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(),
                               13.8, 13.4, std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(),
                               std::numeric_limits<float>::quiet_NaN(), 13.2, 11.5, 10.5, 9.5, 8.5};


lv_color_t get_temperature_color(float value) {
   if (std::isnan(value)) return lv_color_make(128, 128, 128); // Gray for NaN

    // Define temperature range
    constexpr float min_t = -50.0f;
    constexpr float max_t = 50.0f;
    
    // Clamp value to range
    value = std::max(min_t, std::min(max_t, value));

    // Normalize value to [0, 1]
    float ratio = (value - min_t) / (max_t - min_t);

    int red, green, blue;

    if (ratio < 0.25f) {
        // -50°C to -25°C → Blue to Cyan
        float t = ratio / 0.25f;
        red   = static_cast<int>(0 * (1 - t) + 0 * t);
        green = static_cast<int>(0 * (1 - t) + 255 * t);
        blue  = static_cast<int>(255 * (1 - t) + 255 * t);
    } else if (ratio < 0.5f) {
        // -25°C to 0°C → Cyan to Green
        float t = (ratio - 0.25f) / 0.25f;
        red   = static_cast<int>(0 * (1 - t) + 0 * t);
        green = static_cast<int>(255 * (1 - t) + 255 * t);
        blue  = static_cast<int>(255 * (1 - t) + 0 * t);
    } else if (ratio < 0.75f) {
        // 0°C to 25°C → Green to Yellow
        float t = (ratio - 0.5f) / 0.25f;
        red   = static_cast<int>(0 * (1 - t) + 255 * t);
        green = static_cast<int>(255 * (1 - t) + 255 * t);
        blue  = static_cast<int>(0 * (1 - t) + 0 * t);
    } else {
        // 25°C to 50°C → Yellow to Red
        float t = (ratio - 0.75f) / 0.25f;
        red   = static_cast<int>(255 * (1 - t) + 255 * t);
        green = static_cast<int>(255 * (1 - t) + 0 * t);
        blue  = static_cast<int>(0 * (1 - t) + 0 * t);
    }

    return lv_color_make(red, green, blue);
}

void draw_event_cb(lv_event_t *e) {
    lv_obj_t *obj = lv_event_get_target(e);
    lv_obj_draw_part_dsc_t *dsc = static_cast<lv_obj_draw_part_dsc_t*>(lv_event_get_param(e));

    if (dsc->part == LV_PART_TICKS && dsc->id == LV_CHART_AXIS_PRIMARY_X) {
        const char* month[] = {"00", "04", "08", "12", "16", "20", "24"};
        // Copy the time string to dsc->text (which is char[16])
        if (dsc->value >= 0 && dsc->value < 7) {
            lv_snprintf(dsc->text, dsc->text_length, "%s", month[dsc->value]);
        }
        return;
    }

    if (dsc->part != LV_PART_ITEMS) return;
    if (!dsc->p1 || !dsc->p2) return;
    
    if (!std::isnan(points[dsc->id])) {
        dsc->line_dsc->color = get_temperature_color(points[dsc->id]);
    }
        lv_draw_mask_line_param_t line_mask_param;
    lv_draw_mask_line_points_init(&line_mask_param, dsc->p1->x, dsc->p1->y, dsc->p2->x, dsc->p2->y, LV_DRAW_MASK_LINE_SIDE_BOTTOM);
    int32_t line_mask_id = lv_draw_mask_add(&line_mask_param, NULL);
    
    lv_draw_mask_fade_param_t fade_mask_param;
    lv_area_t coords;
    lv_obj_get_coords(obj, &coords);
    lv_draw_mask_fade_init(&fade_mask_param, &coords, LV_OPA_COVER, coords.y1 + (coords.y2 - coords.y1) / 8, LV_OPA_TRANSP, coords.y2);
    int32_t fade_mask_id = lv_draw_mask_add(&fade_mask_param, NULL);
    
    lv_draw_rect_dsc_t draw_rect_dsc;
    lv_draw_rect_dsc_init(&draw_rect_dsc);
    draw_rect_dsc.bg_opa = LV_OPA_80;
    draw_rect_dsc.bg_color = dsc->line_dsc->color;
    
    lv_area_t a;
    a.x1 = dsc->p1->x;
    a.x2 = dsc->p2->x - 1;
    a.y1 = std::min(dsc->p1->y, dsc->p2->y);
    a.y2 = coords.y2;
    lv_draw_ctx_t *draw_ctx = dsc->draw_ctx;
    lv_draw_rect(draw_ctx, &draw_rect_dsc, &a);

    lv_draw_mask_remove_id(line_mask_id);
    lv_draw_mask_remove_id(fade_mask_id);
}

static lv_obj_t *chart = nullptr;
static lv_chart_series_t *ser1 = nullptr;
static lv_obj_t* weather_icons[] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

static void draw_weather_chart(lv_obj_t *obj) {
  if(chart == nullptr) {
    chart = lv_chart_create(obj);
    lv_obj_add_style(chart, weather_chart, 0);
    lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR);
    //lv_obj_set_style_bg_color(chart, lv_color_make(50, 50, 50), LV_PART_MAIN); // Dark grey background
    lv_chart_set_point_count(chart, 24);
    //lv_obj_set_size(chart, 230, 150);
    //lv_obj_center(chart);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);   /*Show lines and points too*/

    lv_obj_set_style_line_color(chart, lv_color_make(70, 70, 70), LV_PART_TICKS); // Less contrast grid lines
    lv_obj_set_style_line_color(chart, lv_color_make(70, 70, 70), LV_PART_MAIN | LV_STATE_DEFAULT);

    /*Add two data series*/
    ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_SECONDARY_Y);
    //ser2 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_SECONDARY_Y);
    lv_obj_add_event_cb(chart, draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
    for(int i = 0; i < 13; i++) {
      weather_icons[i] = lv_label_create(obj);
      lv_obj_add_style(weather_icons[i], weather_small_icon, 0);
      lv_obj_set_pos(weather_icons[i], 24 + i * 19, 10);
      lv_label_set_text_fmt(weather_icons[i], get_weather_icon("clear-night").c_str());
    }
  }

  
  // float max_t = *std::max_element(points, points + 24, safe_cmp);
  // float min_t = *std::min_element(points, points + 24, safe_cmp);


  float v_prev = std::numeric_limits<float>::quiet_NaN();
  int skip_count = 0;

  for (int k = 0; k < 24; ++k) {
    if (!std::isnan(points[k])) {
        for (int skip = 0; skip < skip_count; ++skip) {
            if (std::isnan(v_prev)) {
                v_prev = points[k];
            }
            float med = (points[k] - v_prev) / (skip_count + 1);
            float interpolated_value = round(v_prev + med * (skip + 1));
            lv_chart_set_next_value(chart, ser1, interpolated_value);
        }
        skip_count = 0;
        lv_chart_set_next_value(chart, ser1, round(points[k]));
        v_prev = points[k];
    } else {
        skip_count++;
    }
  }

  for (int skip = 0; skip < skip_count; ++skip) {
      lv_chart_set_next_value(chart, ser1, round(v_prev));
  }
  
  auto _cmp = [](float a, float b) { return !(std::isnan(a)) && (std::isnan(b) || a < b); };
  float max_t = *std::max_element(points, points + 24, _cmp);
  float min_t = *std::min_element(points, points + 24, _cmp);

  min_t = std::min(static_cast<int>(min_t / 5) * 5, 0);
  max_t = min_t + std::max(static_cast<int>((max_t - min_t) / 5) * 5 + 5, 35);

  lv_chart_set_div_line_count(chart, (max_t - min_t) / 5 + 1, 4); // Horizontal every 10, Vertical every 4
  
  lv_chart_set_range(chart, LV_CHART_AXIS_SECONDARY_Y, min_t, max_t);
  lv_chart_set_axis_tick(chart, LV_CHART_AXIS_SECONDARY_Y, 5, 3, (max_t - min_t) / 5 + 1, 5, true, 40);

  lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 5, 3, 7, 4, true, 20);

  lv_chart_refresh(chart); /*Required after direct set*/
}



#endif
