/**
 * @file auto_shoot.h
 * @brief Auto Shoot system - TF-Luna LiDAR + Camera trigger
 * @date 2026-07-01
 */

#pragma once
#include <Arduino.h>

// ============ CONFIG STRUCTURE ============
struct AutoShootConfig {
    float rangeMin = 0.5f;      // meters
    float rangeMax = 4.0f;      // meters
    uint8_t burstShots = 1;     // 1-10 shots
    uint16_t cooldownMs = 500;  // milliseconds
};

// ============ STATE STRUCTURE ============
struct AutoShootState {
    bool isRunning = false;
    bool wasInRange = false;
    unsigned long lastTrigger = 0;
    unsigned long lastUpdate = 0;
    
    float currentDistance = 0.0f;
    uint16_t currentStrength = 0;
    bool objectDetected = false;
};

// ============ EDIT MODE ============
struct EditMode {
    bool active = false;
    uint8_t selectedIndex = 0;  // 0: rangeMin, 1: rangeMax, 2: burst, 3: cooldown
    unsigned long enterTime = 0;
    
    enum EditState {
        IDLE = 0,
        SELECTING = 1,
        EDITING = 2
    } state = IDLE;
};

// ============ AUTO SHOOT CLASS ============
class AutoShoot {
public:
    AutoShootConfig config;
    AutoShootState state;
    EditMode editMode;
    
    AutoShoot();
    ~AutoShoot() = default;
    
    // Initialization
    void init();
    void loadConfig();
    void saveConfig();
    
    // Main update loop
    void update();
    
    // Camera trigger
    void triggerCamera();
    void triggerBurst(uint8_t count);
    
    // Sensor reading
    void updateSensorData();
    
    // Auto-shoot logic
    void checkAndTrigger();
    
    // UI interaction
    void handleEncoderRotate(int delta);
    void handleButtonPress();
    void handleButtonLongPress();
    
    // Getters
    const char* getSelectedItemName();
    float getSelectedValue();
    const char* getStatusString();
    
private:
    void validateConfig();
};

// Global instance
extern AutoShoot autoShoot;
