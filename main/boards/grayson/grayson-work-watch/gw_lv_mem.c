// Grayson Work: LVGL custom allocator. Widgets, styles and strings go to PSRAM so internal
// SRAM stays free for Wi-Fi, TLS and audio. Falls back to internal RAM if PSRAM is exhausted.
#include <lvgl.h>
#include "sdkconfig.h"
#if CONFIG_LV_USE_CUSTOM_MALLOC
#include <esp_heap_caps.h>
#include <string.h>

void lv_mem_init(void) {}
void lv_mem_deinit(void) {}
lv_mem_pool_t lv_mem_add_pool(void *mem, size_t bytes) { (void)mem; (void)bytes; return NULL; }
void lv_mem_remove_pool(lv_mem_pool_t pool) { (void)pool; }

void *lv_malloc_core(size_t size) {
    void *p = heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (p == NULL) p = heap_caps_malloc(size, MALLOC_CAP_8BIT);
    return p;
}
void *lv_realloc_core(void *p, size_t new_size) {
    void *n = heap_caps_realloc(p, new_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (n == NULL) n = heap_caps_realloc(p, new_size, MALLOC_CAP_8BIT);
    return n;
}
void lv_free_core(void *p) { heap_caps_free(p); }
void lv_mem_monitor_core(lv_mem_monitor_t *mon_p) {
    memset(mon_p, 0, sizeof(*mon_p));
    mon_p->total_size = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
    mon_p->free_size = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    mon_p->free_biggest_size = heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM);
    mon_p->used_pct = mon_p->total_size ? (uint8_t)(100 - 100 * mon_p->free_size / mon_p->total_size) : 0;
}
lv_result_t lv_mem_test_core(void) { return LV_RESULT_OK; }
#endif
