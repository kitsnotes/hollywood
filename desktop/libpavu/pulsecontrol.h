#ifndef PULSECONNTROL_H
#define PULSECONNTROL_H

#include "libpavu_global.h"
#include <pulse/pulseaudio.h>
#include <glib.h>
#include <pulse/glib-mainloop.h>
#include <pulse/ext-stream-restore.h>
#include <pulse/ext-device-manager.h>
#include <pulse/context.h>
#include <pulse/proplist.h>
#include <pulse/ext-stream-restore.h>

#define HAVE_SOURCE_OUTPUT_VOLUMES PA_CHECK_VERSION(0,99,0)
#define HAVE_EXT_DEVICE_RESTORE_API PA_CHECK_VERSION(0,99,0)

#ifndef PA_VOLUME_UI_MAX
#define PA_VOLUME_UI_MAX (pa_sw_volume_from_dB(+11.0))
#endif

#if HAVE_EXT_DEVICE_RESTORE_API
#include <pulse/ext-device-restore.h>
#endif

enum SinkInputType {
    SINK_INPUT_ALL,
    SINK_INPUT_CLIENT,
    SINK_INPUT_VIRTUAL
};

enum SinkType {
    SINK_ALL,
    SINK_HARDWARE,
    SINK_VIRTUAL,
};

enum SourceOutputType {
    SOURCE_OUTPUT_ALL,
    SOURCE_OUTPUT_CLIENT,
    SOURCE_OUTPUT_VIRTUAL
};

enum SourceType {
    SOURCE_ALL,
    SOURCE_NO_MONITOR,
    SOURCE_HARDWARE,
    SOURCE_VIRTUAL,
    SOURCE_MONITOR,
};

pa_context* get_context(void);
void show_error(const char *txt);

class MainWindow;
class LIBPAVU_EXPORT PulseControl
{
public:
    PulseControl();
    ~PulseControl();
    void connectToPulse();
    void showMainWindow();
    void contextStateCallback();
    void subscribeCallback(pa_subscription_event_type_t t, uint32_t index);
    void cardCallback(const pa_card_info *i, int eol);
    void sinkCallback(const pa_sink_info *i, int eol);
    void sourceCallback(const pa_source_info *i, int eol);
    void sinkInputCallback(const pa_sink_input_info *i, int eol);
    void sourceOutputCallback(const pa_source_output_info *i, int eol);
    void clientCallback(const pa_client_info *i, int eol);
    void serverInfoCallback(const pa_server_info *i);
    void extStreamRestoreReadCallback(const pa_ext_stream_restore_info *i, int eol);
    void extStreamRestoreSubscribeCallback();
    void extDeviceManagerReadCallback(const pa_ext_device_manager_info *info, int eol);
    void extDeviceManagerSubscribeCallback();
#if HAVE_EXT_DEVICE_RESTORE_API
    void extDeviceRestoreReadCallback(const pa_ext_device_restore_info *i, int eol);
    void extDeviceRestoreSubscribeCallback(pa_device_type_t type, uint32_t idx);
#endif
public: // callbacks
    static void context_state_callback(pa_context *c, void *userdata);
    static void subscribe_cb(pa_context *c, pa_subscription_event_type_t t, uint32_t index, void *userdata);
    static void card_cb(pa_context *, const pa_card_info *i, int eol, void *userdata);
    static void sink_cb(pa_context *c, const pa_sink_info *i, int eol, void *userdata);
    static void source_cb(pa_context *, const pa_source_info *i, int eol, void *userdata);
    static void sink_input_cb(pa_context *, const pa_sink_input_info *i, int eol, void *userdata);
    static void source_output_cb(pa_context *, const pa_source_output_info *i, int eol, void *userdata);
    static void client_cb(pa_context *, const pa_client_info *i, int eol, void *userdata);
    static void server_info_cb(pa_context *, const pa_server_info *i, void *userdata);
    static void ext_stream_restore_read_cb(pa_context *, const pa_ext_stream_restore_info *i, int eol, void *userdata);
    static void ext_stream_restore_subscribe_cb(pa_context *c, void *userdata);
    static void ext_device_manager_read_cb(pa_context *, const pa_ext_device_manager_info *, int eol, void *userdata);
    static void ext_device_manager_subscribe_cb(pa_context *c, void *userdata);

//#if HAVE_EXT_DEVICE_RESTORE_API
    static void ext_device_restore_read_cb(pa_context *, const pa_ext_device_restore_info *i, int eol, void *userdata);
    static void ext_device_restore_subscribe_cb(pa_context *c, pa_device_type_t type, uint32_t idx, void *userdata);
//#endif
private:
    void dec_outstanding();
private:
    MainWindow *m_mainWindow = nullptr;
    pa_glib_mainloop *m_glib_loop = nullptr;
    pa_context *m_context = nullptr;
    pa_mainloop_api* m_api = nullptr;

    int m_n_outstanding = 0;
    bool m_retry = false;
    int m_reconnect_timeout = 1;
};



#endif // PULSECONNTROL_H
