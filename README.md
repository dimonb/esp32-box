# ESP32-S3-Box3-Custom Firmware (Refactored)

## Overview

This is a fully refactored ESPHome firmware for the ESP32-S3-Box3, utilizing **yglu** for templating to eliminate code duplication. All visual controls are now modularly described in a separate section, allowing easier customization and maintenance. This project is based on the original implementation available at [ESP32-S3-Box3-Custom-ESPHome](https://github.com/BigBobbas/ESP32-S3-Box3-Custom-ESPHome).

<img src="https://github.com/user-attachments/assets/0b3fa258-6e06-4375-8820-7dff9db609e1" width="320" height="240">


### Key Features

- **Home Assistant Dashboard**: The device functions as a touch-enabled dashboard for Home Assistant.
- **Media Player**: Improved volume control and the ability to stream media.
- **Voice Assistant**: Supports both on-device wake word detection and Home Assistant wake word engines.
- **Sensor Display & Control**: Integrated temperature, humidity, and motion sensors.
- **Customizable Touch Controls**: Define buttons and layouts dynamically using templates.

## Template-Based UI Configuration

The refactored configuration introduces structured UI elements:

```yaml
buttons: !-
  - id: status_bar_title <not clickable>
    draw:
      - "it.printf(123, 10, id(my_font3), blue, COLOR_OFF, TextAlign::LEFT, \"%.1f°%.0f\", id(temperature_bedroom).has_state() ? id(temperature_bedroom).state : 0,  id(co2_bedroom).has_state() ? id(co2_bedroom).state : 0);"

  - id: status_bar_mute_switch <clickable>
    x: 80
    y: 5
    w: 35
    h: 35
    font: icon_font_35
    on_click:
      - switch.toggle: mute_switch
      - component.update: s3_box_lcd
    draw:
      - "if(id(mute_switch).state) {"
      - !? ($_.draw_icon)({id=>"status_bar_mute_switch", color=>"red", icon=>$_.icon_glyphs.mic_off})
      - "} else {"
      - !? ($_.draw_icon)({id=>"status_bar_mute_switch", color=>"lime", icon=>$_.icon_glyphs.mic_on})
      - "}"
```

### Page Layouts

Using a structured approach, pages are now defined with reusable buttons:

```yaml
touch_pages: !-
  - id: idle_page
    buttons:
      - status_bar_title
      - status_bar_mute_switch
      - status_bar_api_connection
      - status_bar_wifi_connection
      - status_bar_settings_button
      - music_button
      - scene_button
      - current_time

  - id: info_page
    buttons:
      - status_bar_title
      - status_bar_mute_switch
      - status_bar_api_connection
      - status_bar_wifi_connection
```

## Installation Guide

### Requirements

- **ESPHome**: Installed via Home Assistant add-on, CLI, or Docker.
- **Python Virtual Environment**: Required for template processing.

### Steps

If you have `make` installed, simply run `make run`. This command will automatically set up a Python virtual environment in the `.venv` folder and execute all necessary steps to build and deploy the firmware.

## Getting Started

To configure and use this firmware effectively:

- Ensure your Home Assistant setup is ready.
- Modify the button mappings according to your needs.
- Use the built-in modular templates to define new controls easily.

## Contributions

Contributions, feedback, and feature requests are welcome! Feel free to submit issues or pull requests.


## Buy Me a Coffee

If you find this project useful and would like to support future development:

<a href="https://www.buymeacoffee.com/dimonb3"><img src="https://img.buymeacoffee.com/button-api/?text=Buy me a coffee&emoji=&slug=dimonb3&button_colour=FFDD00&font_colour=000000&font_family=Cookie&outline_colour=000000&coffee_colour=ffffff" /></a>

