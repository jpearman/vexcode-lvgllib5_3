/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Copyright (c) James Pearman, All rights reserved.                       */
/*                                                                            */
/*    Module:     v5lvgl.c                                                    */
/*    Author:     James Pearman                                               */
/*    Created:    23 March 2019                                               */
/*                                                                            */
/*    Revisions:  V0.1                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include "v5.h"
#include "v5lvgl.h"

// necessary V5 API functions
void  vexTaskAdd( int (* callback)(void), int interval, char const *label );
void  vexTaskSleep( uint32_t time );

/*----------------------------------------------------------------------------*/
/* Flush the content of the internal buffer the specific area on the display  */
/* You can use DMA or any hardware acceleration to do this operation in the   */
/*  background but 'lv_flush_ready()' has to be called when finished          */
/* This function is required only when LV_VDB_SIZE != 0 in lv_conf.h          */
/*----------------------------------------------------------------------------*/
//
static void
disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p) {
  vexDisplayCopyRect(x1, y1, x2, y2, (uint32_t*)color_p, x2 - x1 + 1);
  lv_flush_ready();
}

/*----------------------------------------------------------------------------*/
/* Write a pixel array (called 'map') to the a specific area on the display   */
/* This function is required only when LV_VDB_SIZE == 0 in lv_conf.h          */
/*----------------------------------------------------------------------------*/
//
static void 
disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p) {
  vexDisplayCopyRect(x1, y1, x2, y2, (uint32_t*)color_p, x2 - x1 + 1);
}

/*----------------------------------------------------------------------------*/
/* Write a pixel array (called 'map') to the a specific area on the display   */
/* This function is required only when LV_VDB_SIZE == 0 in lv_conf.h          */
/*----------------------------------------------------------------------------*/
//
static void 
disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2,  lv_color_t color) {
  uint32_t c = vexDisplayForegroundColorGet();
  vexDisplayForegroundColor( color.full & 0xFFFFFF );
  vexDisplayRectFill( x1, y1, x2, y2 );
  vexDisplayForegroundColor( c );
}

/*----------------------------------------------------------------------------*/
/* Read the touchpad and store it in 'data'                                   */
/* Return false if no more data read; true for ready again                    */
/*----------------------------------------------------------------------------*/
//
static bool ex_tp_read(lv_indev_data_t * data)
{
  V5_TouchStatus status;
  vexTouchDataGet( &status );

  if( status.lastEvent == kTouchEventPress )
    data->state = LV_INDEV_STATE_PR;
  else
  if( status.lastEvent == kTouchEventRelease )
    data->state = LV_INDEV_STATE_REL;

  data->point.x = status.lastXpos;
  data->point.y = status.lastYpos;

  // false: no more data to read because we are not buffering
  return false;
}

/*----------------------------------------------------------------------------*/
/* Task to refresh internal lvgl counters and allow display updates           */
/*----------------------------------------------------------------------------*/
#define V5_LVGL_RATE    4

int
lvgltask() {
  while(1) {
      // this just increments internal counter, may as well put here to simplify
      lv_tick_inc(V5_LVGL_RATE);
      lv_task_handler();

      // Allow other tasks to run
      vexTaskSleep(V5_LVGL_RATE);
  }
  return(0);
}

/*----------------------------------------------------------------------------*/
/* Initialize lvgl for V5 running VEXcode projects                            */
/*----------------------------------------------------------------------------*/
void
v5_lv_init() {
    // init lvgl
    lv_init();

    // create display driver
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);

    // callback to copy to screen
    disp_drv.disp_flush = disp_flush;
    // callback for writing map
    disp_drv.disp_map   = disp_map;
    // callback to fill area
    disp_drv.disp_fill  = disp_fill;
    // register the driver
    lv_disp_drv_register(&disp_drv);

    // touch screen input
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read = ex_tp_read;
    lv_indev_drv_register(&indev_drv);

    // set some theme
    lv_theme_set_current(lv_theme_alien_init(40, NULL));
    lv_obj_t* page = lv_obj_create(NULL, NULL);
    lv_obj_set_size(page, 480, 240);
    lv_scr_load(page);

    // add the update task
    vexTaskAdd( lvgltask, 2, "LVGL" );
}
