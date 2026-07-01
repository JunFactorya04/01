/**
 * @file auto_shoot_ui.cpp
 * @brief Auto Shoot UI rendering with LovyanGFX
 * @date 2026-07-01
 */

#include "auto_shoot.h"
#include "../factory_test/factory_test.h"
#include <smooth_ui_toolkit.h>

extern FactoryTest* _ft;

// ============ UI CONSTANTS ============
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 135

// Colors
#define COLOR_BG 0x0000        // Black
#define COLOR_TEXT 0xFFFF      // White
#define COLOR_GREEN 0x07E0     // Green
#define COLOR_RED 0xF800       // Red
#define COLOR_YELLOW 0xFFE0    // Yellow
#define COLOR_BORDER 0x4208    // Dark gray
#define COLOR_HIGHLIGHT 0x27E0 // Bright green

// Layout
#define ITEM_HEIGHT 28
#define ITEM_Y_START 20
#define ITEM_INDENT 12

// ============ UI STATE ============
static unsigned long blinkTimer = 0;
static bool blinkState = false;

// ============ HELPER FUNCTIONS ============
void updateBlink() {
    if (millis() - blinkTimer > 500) {
        blinkState = !blinkState;
        blinkTimer = millis();
    }
}

// ============ RENDER FUNCTIONS ============
void renderAutoShootUI() {
    if (!_ft) return;
    
    updateBlink();
    
    // Clear screen
    _ft->_canvas->fillScreen(COLOR_BG);
    
    // Header
    renderHeader();
    
    // Settings panel
    renderSettingsPanel();
    
    // Status panel
    renderStatusPanel();
    
    // Buttons
    renderControlButtons();
    
    // Push to display
    _ft->_canvas_update();
}

// ============ HEADER ============
void renderHeader() {
    _ft->_canvas->setFont(&fonts::efontCN_24);
    _ft->_canvas->setTextDatum(top_center);
    _ft->_canvas->setTextColor(COLOR_GREEN);
    _ft->_canvas->drawString("AUTO MODE", SCREEN_WIDTH / 2, 2);
    
    // Back button
    _ft->_canvas->setTextDatum(top_left);
    _ft->_canvas->setTextColor(COLOR_TEXT);
    _ft->_canvas->drawString("<", 5, 2);
}

// ============ SETTINGS PANEL ============
void renderSettingsPanel() {
    // Panel border
    _ft->_canvas->drawRoundRect(8, 18, 224, 75, 5, COLOR_BORDER);
    
    _ft->_canvas->setFont(&fonts::efontCN_16);
    _ft->_canvas->setTextDatum(top_left);
    
    // Item 0: Range Min
    renderSettingsItem(0, "Range Min", autoShoot.config.rangeMin, " m");
    
    // Item 1: Range Max
    renderSettingsItem(1, "Range Max", autoShoot.config.rangeMax, " m");
    
    // Item 2: Burst Shots
    renderSettingsItem(2, "Burst Shots", (float)autoShoot.config.burstShots, " shots");
    
    // Item 3: Cooldown
    renderSettingsItem(3, "Cooldown", (float)autoShoot.config.cooldownMs, " ms");
}

void renderSettingsItem(uint8_t index, const char* label, float value, const char* unit) {
    int y = ITEM_Y_START + (index * ITEM_HEIGHT);
    bool isSelected = (autoShoot.editMode.selectedIndex == index);
    bool isEditing = (autoShoot.editMode.state == EditMode::EDITING && isSelected);
    
    // Background highlight
    if (isSelected) {
        _ft->_canvas->fillRoundRect(10, y - 2, 220, 24, 3, 
                                   isEditing ? COLOR_HIGHLIGHT : COLOR_BORDER);
    }
    
    // Text color
    uint16_t textColor = isSelected ? COLOR_BG : COLOR_TEXT;
    _ft->_canvas->setTextColor(textColor);
    
    // Label
    _ft->_canvas->drawString(label, ITEM_INDENT + 8, y + 4);
    
    // Value
    _ft->_canvas->setTextDatum(top_right);
    char valueStr[32];
    
    if (index == 2) {  // Burst Shots (integer)
        snprintf(valueStr, sizeof(valueStr), "%d%s", (int)value, unit);
    } else if (index == 3) {  // Cooldown (integer)
        snprintf(valueStr, sizeof(valueStr), "%d%s", (int)value, unit);
    } else {  // Range (float)
        snprintf(valueStr, sizeof(valueStr), "%.2f%s", value, unit);
    }
    
    // Blinking effect when editing
    if (isEditing && blinkState) {
        _ft->_canvas->setTextColor(COLOR_BG);
    }
    
    _ft->_canvas->drawString(valueStr, 210, y + 4);
    
    // Arrow indicator
    _ft->_canvas->setTextDatum(top_right);
    _ft->_canvas->setTextColor(COLOR_GREEN);
    _ft->_canvas->drawString(">", 225, y + 4);
    
    _ft->_canvas->setTextDatum(top_left);
}

// ============ STATUS PANEL ============
void renderStatusPanel() {
    int y = 96;
    
    // Panel border
    _ft->_canvas->drawRoundRect(8, y, 224, 32, 5, COLOR_BORDER);
    
    _ft->_canvas->setFont(&fonts::efontCN_16);
    _ft->_canvas->setTextDatum(top_left);
    _ft->_canvas->setTextColor(COLOR_TEXT);
    
    // Left side: TF-Luna Distance
    _ft->_canvas->drawString("TF-Luna:", 15, y + 6);
    _ft->_canvas->setTextColor(COLOR_GREEN);
    _ft->_canvas->setFont(&fonts::efontCN_24);
    char distStr[16];
    snprintf(distStr, sizeof(distStr), "%.2f m", autoShoot.state.currentDistance);
    _ft->_canvas->drawString(distStr, 85, y + 4);
    
    // Right side: Status
    _ft->_canvas->setFont(&fonts::efontCN_16);
    _ft->_canvas->setTextDatum(top_right);
    _ft->_canvas->setTextColor(COLOR_TEXT);
    _ft->_canvas->drawString("Status:", 155, y + 6);
    
    // Status indicator
    const char* status = autoShoot.getStatusString();
    uint16_t statusColor = COLOR_GREEN;
    if (strcmp(status, "IDLE") == 0) {
        statusColor = COLOR_TEXT;
    } else if (strcmp(status, "DETECTING") == 0) {
        statusColor = COLOR_YELLOW;
    }
    
    _ft->_canvas->setTextColor(statusColor);
    _ft->_canvas->setFont(&fonts::efontCN_16);
    _ft->_canvas->drawString(status, 225, y + 6);
    
    // Motion indicator
    _ft->_canvas->setFont(&fonts::efontCN_12);
    _ft->_canvas->setTextDatum(top_center);
    _ft->_canvas->setTextColor(autoShoot.state.objectDetected ? COLOR_GREEN : COLOR_TEXT);
    const char* motion = autoShoot.state.objectDetected ? "DETECTED" : "---";
    _ft->_canvas->drawString(motion, 120, y + 18);
}

// ============ CONTROL BUTTONS ============
void renderControlButtons() {
    // START button (left)
    int btnY = 120;
    int btnHeight = 12;
    
    _ft->_canvas->drawRoundRect(12, btnY, 100, btnHeight, 3, 
                               autoShoot.state.isRunning ? COLOR_GREEN : COLOR_BORDER);
    
    _ft->_canvas->setFont(&fonts::efontCN_12);
    _ft->_canvas->setTextDatum(top_center);
    _ft->_canvas->setTextColor(autoShoot.state.isRunning ? COLOR_BG : COLOR_GREEN);
    _ft->_canvas->drawString("START", 62, btnY + 1);
    
    // STOP button (right)
    _ft->_canvas->drawRoundRect(128, btnY, 100, btnHeight, 3, 
                               autoShoot.state.isRunning ? COLOR_RED : COLOR_BORDER);
    
    _ft->_canvas->setTextColor(autoShoot.state.isRunning ? COLOR_RED : COLOR_TEXT);
    _ft->_canvas->drawString("STOP", 178, btnY + 1);
}

// ============ INTERACTION HANDLER ============
void handleAutoShootInput() {
    // This will be called from main loop
    // Encoder rotation and button press handled via callbacks
}

// ============ INIT ============
void initAutoShootUI() {
    autoShoot.editMode.state = EditMode::SELECTING;
    autoShoot.editMode.selectedIndex = 0;
}
