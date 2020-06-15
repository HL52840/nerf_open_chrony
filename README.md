# nerf_open_chrony

(Placeholder)

# Prequesites

Software Required (On Local system)

  - Arduino IDE

Libraries that are required:
  
  - Adafruit-GFX (Graphics Driver)
  - Adafruit-SSD1306 (LCD Driver)
  
# Features added (So Far)

  - Reset by holding gate #1
  - Readout in Joules

# The DO NOT CHANGE Section in settings.

  - The gate[I/O pin]pin are pins that the sensors that are connected to. If you are using the Open Chrony V4 board from @legodei, you don't need to change these #defines.
  - SCREEN_WIDTH/SCREEN_HEIGHT/OLED_RESET don't need to be changed. These #defines corrospond to an SSD1306 OLED on the I2C bus.