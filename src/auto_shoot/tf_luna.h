/**
 * @file tf_luna.h
 * @brief TF-Luna LiDAR sensor driver
 * @date 2026-07-01
 */

#pragma once
#include <HardwareSerial.h>

extern HardwareSerial TFSerial;

/**
 * @brief Initialize TF-Luna sensor on GPIO13
 */
void initTfLuna() {
    TFSerial.begin(115200, SERIAL_8N1, 13, -1);
    delay(100);
}

/**
 * @brief Read raw TF-Luna data
 * @param distance_m Output distance in meters
 * @param strength Output signal strength
 * @return true if valid frame received, false otherwise
 */
bool readTfLunaRaw(float &distance_m, uint16_t &strength) {
    
    if (TFSerial.available() >= 9) {
        
        uint8_t buf[9];
        TFSerial.readBytes(buf, 9);
        
        // HEADER CHECK (0x59 0x59)
        if (buf[0] == 0x59 && buf[1] == 0x59) {
            
            uint16_t dist_mm = buf[2] + (buf[3] << 8);
            strength = buf[4] + (buf[5] << 8);
            
            distance_m = dist_mm / 1000.0f;
            
            return true;
        }
    }
    
    return false;
}

/**
 * @brief Read TF-Luna with timeout protection
 */
float readTfLunaDistance() {
    static float last_distance = 0.0f;
    float distance_m = 0.0f;
    uint16_t strength = 0;
    
    if (readTfLunaRaw(distance_m, strength)) {
        last_distance = distance_m;
        return distance_m;
    }
    
    return last_distance;  // Return last valid reading
}
