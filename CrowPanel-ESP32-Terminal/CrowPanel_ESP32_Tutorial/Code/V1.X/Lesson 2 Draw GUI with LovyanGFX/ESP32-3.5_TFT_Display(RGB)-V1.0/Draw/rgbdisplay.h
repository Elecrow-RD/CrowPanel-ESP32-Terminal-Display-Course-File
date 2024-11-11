#include <LovyanGFX.hpp>

#define LCD_CS -1
#define LCD_BLK 46

class LGFX : public lgfx::LGFX_Device
{
    //lgfx::Panel_ILI9341 _panel_instance;
    lgfx::Panel_ILI9488 _panel_instance;
    lgfx::Bus_Parallel16 _bus_instance; // 8-bit parallel Bus instance (ESP32 only)

public:
    // Create a constructor and configure it.
    // If you change the class name, specify the same name as the constructor.
    LGFX(void)
    {
        {                                      // Configure the bus control Settings.
            auto cfg = _bus_instance.config(); // Get the structure for bus configuration.

            // 16位设置
            cfg.port = 0;  // Select the I2S port to use (0 or 1) (using the I2S LCD mode of ESP32)
            cfg.freq_write = 16000000; // Transmission clock (rounded up to 20mhz, 80mhz divided by an integer)
            cfg.pin_wr = 18;           // Pin number connecting the WR.
            cfg.pin_rd = 48;           // Pin number connecting RD.
            cfg.pin_rs = 45;           // Pin number connecting RS(D/C)

            cfg.pin_d0 = 47;
            cfg.pin_d1 = 21;
            cfg.pin_d2 = 14;
            cfg.pin_d3 = 13;
            cfg.pin_d4 = 12;
            cfg.pin_d5 = 11;
            cfg.pin_d6 = 10;
            cfg.pin_d7 = 9;
            cfg.pin_d8 = 3;
            cfg.pin_d9 = 8;
            cfg.pin_d10 = 16;
            cfg.pin_d11 = 15;
            cfg.pin_d12 = 7;
            cfg.pin_d13 = 6;
            cfg.pin_d14 = 5;
            cfg.pin_d15 = 4;

            _bus_instance.config(cfg);              //The Settings are reflected on the bus.
            _panel_instance.setBus(&_bus_instance); // Set the bus to the panel.
        }

        {                                        // Set the display panel control.
            auto cfg = _panel_instance.config(); // Retrieves the structure for the display panel configuration.

            cfg.pin_cs = -1;   // CS to pull down
            cfg.pin_rst = -1;  // The RST is connected to the development board RST
            cfg.pin_busy = -1; // Pin number to which BUSY is connected (-1 = disable)

            // Please comment out the unknown items and try it.

            cfg.memory_width = 321;   // Maximum width supported by the driver IC
            cfg.memory_height = 481;  // Maximum height supported by the driver IC
            cfg.panel_width = 320;    // The actual width that can be displayed
            cfg.panel_height = 480;   // The height that you can actually display
            cfg.offset_x = 0;         // The amount of offset in the X direction of the panel
            cfg.offset_y = 0;         // The offset amount of panel in the Y direction
            cfg.offset_rotation = 0;  // Offset 0~7 of the value of the rotation direction (4~7 is upside down)
            cfg.dummy_read_pixel = 8; // Number of bits of dummy reads before pixel reads
            cfg.dummy_read_bits = 1;  // Number of bits of dummy read before reading non-pixel data
            cfg.readable = true;      // Set to true if data reading is possible
            cfg.invert = false;       // Set to true if the light/dark of the panel is reversed
            cfg.rgb_order = false;    // Set to true when panel red and blue are switched
            cfg.dlen_16bit = true;    // Set to true for panels that transmit data length in 16-bit units
            cfg.bus_shared = true;    // Set to true when sharing the bus with the SD card (control the bus by drawJpgFile etc.)
            _panel_instance.config(cfg);
        }

        setPanel(&_panel_instance); // Set the panel you want to use.
    }
};

LGFX lcd;
LGFX_Sprite sprite(&lcd);
