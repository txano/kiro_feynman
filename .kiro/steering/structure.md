# Project Structure

## Directory Layout

```
├── include/                    # Public header files
│   ├── led_matrix.h           # LED matrix interface
│   └── wifi_provisioning.h    # WiFi provisioning interface
│
├── src/                       # Implementation files
│   ├── main.cpp              # Main application entry point
│   ├── led_matrix.cpp        # LED matrix implementation
│   └── wifi_provisioning.cpp # WiFi provisioning implementation
│
├── arduino/                   # Arduino IDE backup (not used)
│   └── libraries/            # Library copies for reference
│
├── .pio/                     # PlatformIO build artifacts (generated)
│   ├── build/                # Compiled binaries
│   └── libdeps/              # Downloaded dependencies
│
├── platformio.ini            # PlatformIO configuration
├── partitions.csv            # Flash partition table
├── sdkconfig.defaults        # ESP-IDF default configuration
│
└── Documentation files:
    ├── README.md             # Main project documentation
    ├── ARCHITECTURE.md       # System architecture details
    ├── COMMANDS.md           # Quick command reference
    ├── QUICKSTART.md         # Getting started guide
    ├── TESTING.md            # Testing procedures
    └── TODO.md               # Future work items
```

## Code Organization

### Header Files (include/)
- Define public interfaces and function prototypes
- Use include guards (`#ifndef`, `#define`, `#endif`)
- Keep minimal - only expose what's needed

### Implementation Files (src/)
- **main.cpp**: Application entry point with `setup()` and `loop()`
- **led_matrix.cpp**: LED matrix control, scrolling text, status icons
- **wifi_provisioning.cpp**: WiFi provisioning, event handling, NVS management

## Naming Conventions

### Files
- Headers: `module_name.h`
- Implementation: `module_name.cpp`
- Use snake_case for filenames

### Functions
- Public API: `module_action()` format (e.g., `led_matrix_init()`, `wifi_prov_start()`)
- Static helpers: `action_noun()` format (e.g., `draw_wifi_icon()`, `get_device_service_name()`)
- Use snake_case for all functions

### Variables
- Global state: `snake_case` with descriptive names
- Constants: `UPPER_SNAKE_CASE` for defines
- Static variables: prefix with `static` keyword

### Types
- Enums: `typedef enum { ... } name_t;` format
- Use `_t` suffix for type definitions

## Code Style

### Logging
- Use ESP-IDF logging macros: `ESP_LOGI()`, `ESP_LOGW()`, `ESP_LOGE()`
- Define TAG constant per file: `static const char *TAG = "module_name";`
- Log important state changes and errors

### Comments
- Use `//` for single-line comments
- Document complex logic and event flows
- Explain "why" not "what" when code is clear

### Formatting
- Indentation: 4 spaces (no tabs)
- Braces: K&R style (opening brace on same line)
- Line length: Keep reasonable (~100 chars)

## Module Responsibilities

### main.cpp
- Initialize all subsystems
- Manage main event loop
- Coordinate between modules
- Handle status callbacks

### led_matrix module
- Initialize NeoMatrix hardware
- Display status icons (WiFi, BLE, checkmark, error)
- Scroll text with configurable colors
- Manage brightness and display updates

### wifi_provisioning module
- Initialize WiFi and NVS
- Handle SoftAP provisioning flow
- Manage WiFi events and state
- Provide status callbacks to main application

## Event-Driven Architecture
- Use ESP-IDF event system for WiFi/provisioning events
- Callbacks for status updates to main application
- Non-blocking operations in main loop
- FreeRTOS event groups for synchronization
