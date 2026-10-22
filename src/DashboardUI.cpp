#include "DashboardUI.h"

extern "C"
{
    LV_IMG_DECLARE(Tachometer);
    LV_IMG_DECLARE(Fuel);
    LV_IMG_DECLARE(HighBeam);
    LV_IMG_DECLARE(TurnSignalRight);
    LV_IMG_DECLARE(TurnSignalLeft);
    LV_IMG_DECLARE(VWLogo);
    LV_FONT_DECLARE(Michroma_15);
    LV_FONT_DECLARE(Michroma_20);
    LV_FONT_DECLARE(Michroma_30);
    LV_FONT_DECLARE(Michroma_165);
}

void DashboardUI::begin(esp_panel::board::Board *board, VehicleData *vehicleData)
{
    // Store local reference to vehicle signals
    _localVehicleData = vehicleData;
    _board = board;

    // 1. Initialize the Porting Layer (Start the LVGL task)
    lvgl_port_init(_board->getLCD(), _board->getTouch());
    _board->getBacklight()->setBrightness(100);

    // Create the UI (Thread-safe)
    lvgl_port_lock(-1);

    // Physical screen background remains black
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_make(0, 0, 0), 0);

    // ##############################################################
    // 2. MASTER VISIBLE CONTAINER (Restricts UI to your viewable area)
    // ##############################################################
    _visibleContainer = lv_obj_create(lv_scr_act());
    lv_obj_set_size(_visibleContainer, 1010, 480);                // <-- CHANGE: Set to your actual visible width & height
    lv_obj_align(_visibleContainer, LV_ALIGN_CENTER, 5, 20);      // Centers X and Y
    lv_obj_clear_flag(_visibleContainer, LV_OBJ_FLAG_SCROLLABLE); // Disable scrollbars/overflow

    // Style the master container to be invisible/transparent
    lv_obj_set_style_bg_opa(_visibleContainer, 0, 0);
    lv_obj_set_style_border_width(_visibleContainer, 0, 0);
    lv_obj_set_style_pad_all(_visibleContainer, 0, 0);

    // Images (Parented to _visibleContainer instead of lv_scr_act())
    vwLogoImg = lv_img_create(_visibleContainer);
    lv_img_set_src(vwLogoImg, &VWLogo);
    lv_obj_align(vwLogoImg, LV_ALIGN_TOP_LEFT, 40, 7);

    tachometerImg = lv_img_create(_visibleContainer);
    lv_img_set_src(tachometerImg, &Tachometer);
    lv_obj_align(tachometerImg, LV_ALIGN_TOP_MID, 10, 60);

    turnSignalLeftImg = lv_img_create(_visibleContainer);
    lv_img_set_src(turnSignalLeftImg, &TurnSignalLeft);
    lv_obj_align(turnSignalLeftImg, LV_ALIGN_BOTTOM_MID, -100, -50);
    lv_obj_add_flag(turnSignalLeftImg, LV_OBJ_FLAG_HIDDEN);

    turnSignalRightImg = lv_img_create(_visibleContainer);
    lv_img_set_src(turnSignalRightImg, &TurnSignalRight);
    lv_obj_align(turnSignalRightImg, LV_ALIGN_BOTTOM_MID, 100, -50);
    lv_obj_add_flag(turnSignalRightImg, LV_OBJ_FLAG_HIDDEN);

    highBeamImg = lv_img_create(_visibleContainer);
    lv_img_set_src(highBeamImg, &HighBeam);
    lv_obj_align(highBeamImg, LV_ALIGN_BOTTOM_MID, 0, -50);
    lv_obj_add_flag(highBeamImg, LV_OBJ_FLAG_HIDDEN);

    // Create the RPM Bar
    barRPM = lv_bar_create(_visibleContainer);
    static lv_style_t style_bg_RPM;
    static lv_style_t style_indic_RPM;
    lv_obj_set_size(barRPM, 645, 40);
    lv_obj_align(barRPM, LV_ALIGN_TOP_MID, 10, 15);
    lv_obj_set_style_radius(barRPM, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(barRPM, 0, LV_PART_INDICATOR);
    lv_bar_set_range(barRPM, 0, 80);
    lv_bar_set_value(barRPM, 50, LV_ANIM_ON);
    lv_style_init(&style_bg_RPM);
    lv_style_set_border_color(&style_bg_RPM, lv_color_make(255, 255, 255));
    lv_style_set_border_width(&style_bg_RPM, 2);
    lv_style_set_pad_all(&style_bg_RPM, 4);
    lv_style_init(&style_indic_RPM);
    lv_style_set_bg_opa(&style_indic_RPM, LV_OPA_COVER);
    lv_style_set_bg_color(&style_indic_RPM, lv_color_white());
    lv_obj_add_style(barRPM, &style_bg_RPM, LV_PART_MAIN);
    lv_obj_add_style(barRPM, &style_indic_RPM, LV_PART_INDICATOR);

    // ##############################################################
    // Style definitions for labels
    // ##############################################################
    lv_style_init(&style_Labels);
    lv_style_set_text_font(&style_Labels, &Michroma_15);
    lv_style_set_text_color(&style_Labels, lv_color_white());
    lv_style_set_text_align(&style_Labels, LV_TEXT_ALIGN_CENTER);

    lv_style_init(&style_ValueLabels);
    lv_style_set_text_font(&style_ValueLabels, &Michroma_30);
    lv_style_set_text_color(&style_ValueLabels, lv_color_white());
    lv_style_set_text_align(&style_ValueLabels, LV_TEXT_ALIGN_CENTER);

    lv_style_init(&style_Speed);
    lv_style_set_text_font(&style_Speed, &Michroma_165);
    lv_style_set_text_color(&style_Speed, lv_color_white());
    lv_style_set_text_align(&style_Speed, LV_TEXT_ALIGN_CENTER);
    // ##############################################################

    LeftCells();
    RightCells();

    // Speed Container Parent
    lv_obj_t *dataCont = lv_obj_create(_visibleContainer);
    lv_obj_set_size(dataCont, 260, 240);
    lv_obj_set_flex_flow(dataCont, LV_FLEX_FLOW_COLUMN);
    lv_obj_align(dataCont, LV_ALIGN_TOP_MID, 0, 120);

    lv_obj_set_style_bg_opa(dataCont, 0, 0);
    lv_obj_set_style_border_width(dataCont, 0, 0);
    lv_obj_set_style_pad_all(dataCont, 0, 0);
    lv_obj_set_style_pad_row(dataCont, 15, 0);

    // Speed box
    lv_obj_t *box = lv_obj_create(dataCont);
    lv_obj_set_size(box, lv_pct(100), 240);
    lv_obj_set_flex_flow(box, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_bg_color(box, LV_COLOR_MAKE(0, 3, 91), 0);
    lv_obj_set_style_pad_all(box, 10, 0);
    lv_obj_set_style_pad_row(box, 25, 0);
    lv_obj_set_style_radius(box, 10, 0);

    lv_obj_t *l = lv_label_create(box);
    lv_obj_add_style(l, &style_Labels, 0);
    lv_obj_set_width(l, lv_pct(100));
    lv_label_set_text(l, "Speed (mph)");

    _speedValueLabel = lv_label_create(box);
    lv_obj_add_style(_speedValueLabel, &style_Speed, 0);
    lv_obj_set_width(_speedValueLabel, lv_pct(100));
    lv_label_set_text(_speedValueLabel, "");

    fuelImg = lv_img_create(_visibleContainer);
    lv_img_set_src(fuelImg, &Fuel);
    lv_obj_align(fuelImg, LV_ALIGN_BOTTOM_MID, 120, -8);

    // Fuel Bar
    static lv_style_t style_bg;
    static lv_style_t style_indic;
    fuelBar = lv_bar_create(_visibleContainer);
    lv_obj_set_size(fuelBar, 180, 30);
    lv_obj_align(fuelBar, LV_ALIGN_BOTTOM_MID, 0, -15);
    lv_obj_set_style_radius(fuelBar, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(fuelBar, 0, LV_PART_INDICATOR);
    lv_bar_set_range(fuelBar, 0, 8);
    lv_bar_set_value(fuelBar, 8, LV_ANIM_OFF);
    lv_style_init(&style_bg);
    lv_style_set_border_color(&style_bg, lv_color_white());
    lv_style_set_border_width(&style_bg, 2);
    lv_style_set_pad_all(&style_bg, 5);
    lv_style_init(&style_indic);
    lv_style_set_bg_opa(&style_indic, LV_OPA_COVER);
    lv_style_set_bg_color(&style_indic, lv_color_white());
    lv_obj_add_style(fuelBar, &style_bg, LV_PART_MAIN);
    lv_obj_add_style(fuelBar, &style_indic, LV_PART_INDICATOR);

    lvgl_port_unlock();
}

void DashboardUI::LeftCells()
{
    // Parented to _visibleContainer
    lv_obj_t *dataCont = lv_obj_create(_visibleContainer);
    lv_obj_set_size(dataCont, 180, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(dataCont, LV_FLEX_FLOW_COLUMN);
    lv_obj_align(dataCont, LV_ALIGN_TOP_LEFT, 20, 120);

    lv_obj_set_style_bg_opa(dataCont, 0, 0);
    lv_obj_set_style_border_width(dataCont, 0, 0);
    lv_obj_set_style_pad_all(dataCont, 0, 0);
    lv_obj_set_style_pad_row(dataCont, 15, 0);

    createCell(dataCont, "Coolant(F)", &_coolantValueLabel, &_coolantCell);
    createCell(dataCont, "IAT (F)", &_matValueLabel, &_matCell);
    createCell(dataCont, "Battery(V)", &_batteryValueLabel, &_batteryCell);
}

void DashboardUI::RightCells()
{
    // Parented to _visibleContainer
    lv_obj_t *dataCont = lv_obj_create(_visibleContainer);
    lv_obj_set_size(dataCont, 180, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(dataCont, LV_FLEX_FLOW_COLUMN);
    lv_obj_align(dataCont, LV_ALIGN_TOP_RIGHT, -20, 120);

    lv_obj_set_style_bg_opa(dataCont, 0, 0);
    lv_obj_set_style_border_width(dataCont, 0, 0);
    lv_obj_set_style_pad_all(dataCont, 0, 0);
    lv_obj_set_style_pad_row(dataCont, 15, 0);

    createCell(dataCont, "RPM", &_rpmValueLabel, &_rpmCell);
    createCell(dataCont, "MAP (kPa)", &_mapValueLabel, &_mapCell);
    createCell(dataCont, "AFR", &_afrValueLabel, &_afrCell);
}

void DashboardUI::createCell(lv_obj_t *parent, const char *title, lv_obj_t **val_label, lv_obj_t **box_container)
{
    // Create the background bounding box inside the target layout container
    lv_obj_t *box = lv_obj_create(parent);
    lv_obj_set_size(box, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(box, LV_FLEX_FLOW_COLUMN);

    // Default to transparent so your background color switches work properly
    lv_obj_set_style_bg_opa(box, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(box, 10, 0);
    lv_obj_set_style_pad_row(box, 2, 0);
    lv_obj_set_style_radius(box, 10, 0);

    // Title label (Coolant, Oil Temp, etc.)
    lv_obj_t *title_lbl = lv_label_create(box);
    lv_obj_add_style(title_lbl, &style_Labels, 0);
    lv_obj_set_width(title_lbl, lv_pct(100));
    lv_label_set_text(title_lbl, title);

    // Dynamic numeric value label
    *val_label = lv_label_create(box);
    lv_obj_add_style(*val_label, &style_ValueLabels, 0);
    lv_obj_set_width(*val_label, lv_pct(100));
    lv_label_set_text(*val_label, "--");

    // Pass the box reference back to the class pointer for structural style updates
    *box_container = box;
}

void DashboardUI::render()
{

    VehicleData &current = *_localVehicleData;
    char buf[32];

    lvgl_port_lock(-1);

    if (current.rpm.hasChanged())
    {
        lv_label_set_text_fmt(_rpmValueLabel, "%d", (int)current.rpm.value);
        lv_bar_set_value(barRPM, current.rpm.value / 100, LV_ANIM_OFF);

        switch (current.rpm.thresholdState())
        {
        case CANDataFieldThreshold::NOMINAL:
            lv_obj_set_style_bg_color(barRPM, lv_color_white(), LV_PART_INDICATOR);
            break;
        case CANDataFieldThreshold::ABOVE_THRESHOLD:
            lv_obj_set_style_bg_color(barRPM, lv_color_make(255, 0, 0), LV_PART_INDICATOR);
            break;
        }
    }

    if (current.map.hasChanged())
    {
        dtostrf(current.map.value, 0, 0, buf);
        lv_label_set_text_fmt(_mapValueLabel, "%s", buf);

        switch (current.map.thresholdState())
        {
        case CANDataFieldThreshold::NOMINAL:
            lv_obj_set_style_bg_opa(_mapCell, LV_OPA_TRANSP, 0);
            break;
        case CANDataFieldThreshold::BELOW_THRESHOLD:
            lv_obj_set_style_bg_opa(_mapCell, LV_OPA_COVER, 0);
            lv_obj_set_style_bg_color(_mapCell, lv_color_make(0, 0, 255), 0);
            break;
        case CANDataFieldThreshold::ABOVE_THRESHOLD:
            lv_obj_set_style_bg_opa(_mapCell, LV_OPA_COVER, 0);
            lv_obj_set_style_bg_color(_mapCell, lv_color_make(255, 0, 0), 0);
            break;
        }
    }

    if (current.fuelLevel.hasChanged())
    {
        uint8_t fuelLevel = map(current.fuelLevel.value, 0, 100, 0, 8); // Map fuel level to bar range
        lv_bar_set_value(fuelBar, fuelLevel, LV_ANIM_OFF);
    }

    if (current.coolant.hasChanged())
    {
        dtostrf((int)current.coolant.value, 0, 0, buf);
        lv_label_set_text_fmt(_coolantValueLabel, "%s", buf);

        switch (current.coolant.thresholdState())
        {
        case CANDataFieldThreshold::NOMINAL:
            lv_obj_set_style_bg_opa(_coolantCell, LV_OPA_TRANSP, 0);
            break;
        case CANDataFieldThreshold::BELOW_THRESHOLD:
            lv_obj_set_style_bg_opa(_coolantCell, LV_OPA_COVER, 0);
            lv_obj_set_style_bg_color(_coolantCell, lv_color_make(0, 0, 255), 0);
            break;
        case CANDataFieldThreshold::ABOVE_THRESHOLD:
            lv_obj_set_style_bg_opa(_coolantCell, LV_OPA_COVER, 0);
            lv_obj_set_style_bg_color(_coolantCell, lv_color_make(255, 0, 0), 0);
            break;
        }
    }

    if (current.iat.hasChanged())
    {
        dtostrf((int)current.iat.value, 0, 0, buf);
        lv_label_set_text_fmt(_matValueLabel, "%s", buf);

        switch (current.iat.thresholdState())
        {
        case CANDataFieldThreshold::NOMINAL:
            lv_obj_set_style_bg_opa(_matCell, LV_OPA_TRANSP, 0);
            break;
        case CANDataFieldThreshold::BELOW_THRESHOLD:
            lv_obj_set_style_bg_opa(_matCell, LV_OPA_COVER, 0);
            lv_obj_set_style_bg_color(_matCell, lv_color_make(0, 0, 255), 0);
            break;
        case CANDataFieldThreshold::ABOVE_THRESHOLD:
            lv_obj_set_style_bg_opa(_matCell, LV_OPA_COVER, 0);
            lv_obj_set_style_bg_color(_matCell, lv_color_make(255, 0, 0), 0);
            break;
        }
    }

    if (current.afr1.hasChanged())
    {
        dtostrf(current.afr1.value, 0, 1, buf);
        lv_label_set_text_fmt(_afrValueLabel, "%s", buf);
    }

    if (current.battery.hasChanged())
    {
        dtostrf(current.battery.value, 0, 1, buf);
        lv_label_set_text_fmt(_batteryValueLabel, "%s", buf);

        switch (current.battery.thresholdState())
        {
        case CANDataFieldThreshold::NOMINAL:
            lv_obj_set_style_bg_opa(_batteryCell, LV_OPA_TRANSP, 0);
            break;
        case CANDataFieldThreshold::BELOW_THRESHOLD:
            lv_obj_set_style_bg_color(_batteryCell, lv_color_make(0, 0, 255), 0);
            break;
        case CANDataFieldThreshold::ABOVE_THRESHOLD:
            lv_obj_set_style_bg_color(_batteryCell, lv_color_make(255, 0, 0), 0);
            break;
        }
    }

    if (current.speed.hasChanged())
    {
        lv_label_set_text_fmt(_speedValueLabel, "%d", (int)current.speed.value);
    }

    if (current.leftTurn.hasChanged())
    {
        if (current.leftTurn.value == true)
        {
            lv_obj_clear_flag(turnSignalLeftImg, LV_OBJ_FLAG_HIDDEN);
        }
        else
        {
            lv_obj_add_flag(turnSignalLeftImg, LV_OBJ_FLAG_HIDDEN);
        }
    }

    if (current.rightTurn.hasChanged())
    {
        if (current.rightTurn.value == true)
        {
            lv_obj_clear_flag(turnSignalRightImg, LV_OBJ_FLAG_HIDDEN);
        }
        else
        {
            lv_obj_add_flag(turnSignalRightImg, LV_OBJ_FLAG_HIDDEN);
        }
    }

    // Process the high beam indicator
    if (current.highBeam.hasChanged())
    {
        if (current.highBeam.value == true)
        {
            lv_obj_clear_flag(highBeamImg, LV_OBJ_FLAG_HIDDEN);
        }
        else
        {
            lv_obj_add_flag(highBeamImg, LV_OBJ_FLAG_HIDDEN);
        }
    }

    lvgl_port_unlock();
}

void DashboardUI::scale_label_format_cb(lv_event_t *e)
{
    lv_obj_draw_part_dsc_t *dsc = lv_event_get_draw_part_dsc(e);

    if (dsc->part == LV_PART_TICKS && dsc->text != NULL)
    {
        int display_value = dsc->value / 10;
        sprintf(dsc->text, "%d", display_value);
    }
}

void DashboardUI::ui_timer_cb(lv_timer_t *timer)
{
    DashboardUI *self =
        static_cast<DashboardUI *>(timer->user_data);

    if (self)
        self->render();
}