#pragma once
#include <Arduino.h>
#include <esp_display_panel.hpp>
#include <lvgl.h>
#include "lvgl_v8_port.h"
#include "CAN_Bus/VehicleData.h"

using namespace esp_panel::drivers;
using namespace esp_panel::board;

class DashboardUI
{
public:
    void begin(esp_panel::board::Board *board, VehicleData *vehicleData);
    void ProcessTurnSignals();
    void LeftCells();
    void RightCells();
    void render();

private:
    esp_panel::board::Board *_board;

    //The container that sets the visible area of the UI. All other objects are children of this container.
    lv_obj_t * _visibleContainer;


    // Text labels
    lv_obj_t *_rpmLabel;
    lv_obj_t *_batteryLabel;
    lv_obj_t *_tpsLabel;
    lv_obj_t *_coolantLabel;
    lv_obj_t *_pwLabel;
    lv_obj_t *_afrLabel;
    lv_obj_t *_mapLabel;
    lv_obj_t *_fuelLabel;
    lv_obj_t *_speedLabel;

    // Value labels
    lv_obj_t *_batteryValueLabel;
    lv_obj_t *_tpsValueLabel;
    lv_obj_t *_coolantValueLabel;
    lv_obj_t *_pwValueLabel;
    lv_obj_t *_afrValueLabel;
    lv_obj_t *_rpmValueLabel;
    lv_obj_t *_mapValueLabel;
    lv_obj_t *_matValueLabel;
    lv_obj_t *barRPM;
    lv_obj_t *fuelBar;

    //Images
    lv_obj_t *tachometerImg;
    lv_obj_t *fuelImg;
    lv_obj_t *highBeamImg;
    lv_obj_t *turnSignalLeftImg;
    lv_obj_t *turnSignalRightImg;
    lv_obj_t *vwLogoImg;

    // Styles
    lv_style_t style_Speed;
    lv_style_t style_ValueLabels;
    lv_style_t style_Labels;

    lv_obj_t *_speedValueLabel;
    VehicleData *_localVehicleData = nullptr;
    VehicleData previousVehicleData;
    static void scale_label_format_cb(lv_event_t *e);
    static void ui_timer_cb(lv_timer_t *timer);
};