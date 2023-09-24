#include "pulsecontrol.h"
#include "mainwindow.h"

#include <QMessageBox>

#define PACKAGE_VERSION "0.1"

static pa_context* g_context = nullptr;

PulseControl::PulseControl()
{
    m_glib_loop =
        pa_glib_mainloop_new(g_main_context_default());
    g_assert(m_glib_loop);
    m_api = pa_glib_mainloop_get_api(m_glib_loop);
    g_assert(m_api);
    m_mainWindow = new MainWindow();
}

PulseControl::~PulseControl()
{
    if(m_context)
        pa_context_unref(m_context);
    pa_glib_mainloop_free(m_glib_loop);
}

void PulseControl::connectToPulse()
{
    if (m_context)
        return;

    pa_proplist *proplist = pa_proplist_new();
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_NAME, QObject::tr("Volume Control").toUtf8().constData());
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_ID, "org.PulseAudio.soundsettings");
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_ICON_NAME, "audio-card");
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_VERSION, PACKAGE_VERSION);

    m_context = pa_context_new_with_proplist(m_api, nullptr, proplist);
    g_assert(m_context);

    g_context = m_context;

    pa_proplist_free(proplist);

    pa_context_set_state_callback(m_context,
           &PulseControl::context_state_callback, this);

    m_mainWindow->setConnectingMessage();
    if (pa_context_connect(m_context, nullptr, PA_CONTEXT_NOFAIL, nullptr) < 0) {
        if (pa_context_errno(m_context) == PA_ERR_INVALID) {
            m_mainWindow->setConnectingMessage(QObject::tr("Connection to PulseAudio failed. Automatic retry in 5s.<br><br>"
                                                "In this case this is likely because PULSE_SERVER in the Environment/X11 Root Window Properties"
                                                "or default-server in client.conf is misconfigured.<br>"
                                                "This situation can also arrise when PulseAudio crashed and left stale details in the X11 Root Window.<br>"
                                                "If this is the case, then PulseAudio should autospawn again, or if this is not configured you should"
                                                "run start-pulseaudio-x11 manually.").toUtf8().constData());
            m_reconnect_timeout = 5;
        }
        else {
            if(!m_retry) {
                m_reconnect_timeout = -1;
                qApp->quit();
            } else {
                g_debug("%s", QObject::tr("Connection failed, attempting reconnect").toUtf8().constData());
                m_reconnect_timeout = 5;
                // TODO: fix
                //g_timeout_add_seconds(m_reconnect_timeout, connect_to_pulse, nullptr);
            }
        }
    }

    return;
}

void PulseControl::showMainWindow()
{
    if(m_mainWindow)
    {
        connectToPulse();
        m_mainWindow->show();
    }
}

void PulseControl::contextStateCallback()
{
    switch (pa_context_get_state(m_context))
    {
    case PA_CONTEXT_UNCONNECTED:
    case PA_CONTEXT_CONNECTING:
    case PA_CONTEXT_AUTHORIZING:
    case PA_CONTEXT_SETTING_NAME:
        break;

    case PA_CONTEXT_READY: {
        pa_operation *o;

        m_reconnect_timeout = 1;

        /* Create event widget immediately so it's first in the list */
        m_mainWindow->createEventRoleWidget();

        pa_context_set_subscribe_callback(m_context, &PulseControl::subscribe_cb, this);

        if (!(o = pa_context_subscribe(m_context, (pa_subscription_mask_t)
                                       (PA_SUBSCRIPTION_MASK_SINK|
                                        PA_SUBSCRIPTION_MASK_SOURCE|
                                        PA_SUBSCRIPTION_MASK_SINK_INPUT|
                                        PA_SUBSCRIPTION_MASK_SOURCE_OUTPUT|
                                        PA_SUBSCRIPTION_MASK_CLIENT|
                                        PA_SUBSCRIPTION_MASK_SERVER|
                                        PA_SUBSCRIPTION_MASK_CARD), nullptr, nullptr))) {
            show_error(QObject::tr("pa_context_subscribe() failed").toUtf8().constData());
            return;
        }
        pa_operation_unref(o);

        /* Keep track of the outstanding callbacks for UI tweaks */
        m_n_outstanding = 0;

        if (!(o = pa_context_get_server_info(m_context, &PulseControl::server_info_cb, this))) {
            show_error(QObject::tr("pa_context_get_server_info() failed").toUtf8().constData());
            return;
        }
        pa_operation_unref(o);
        m_n_outstanding++;

        if (!(o = pa_context_get_client_info_list(m_context, &PulseControl::client_cb, this))) {
            show_error(QObject::tr("pa_context_client_info_list() failed").toUtf8().constData());
            return;
        }
        pa_operation_unref(o);
        m_n_outstanding++;

        if (!(o = pa_context_get_card_info_list(m_context, &PulseControl::card_cb, this))) {
            show_error(QObject::tr("pa_context_get_card_info_list() failed").toUtf8().constData());
            return;
        }
        pa_operation_unref(o);
        m_n_outstanding++;

        if (!(o = pa_context_get_sink_info_list(m_context, &PulseControl::sink_cb, this))) {
            show_error(QObject::tr("pa_context_get_sink_info_list() failed").toUtf8().constData());
            return;
        }
        pa_operation_unref(o);
        m_n_outstanding++;

        if (!(o = pa_context_get_source_info_list(m_context, &PulseControl::source_cb, this))) {
            show_error(QObject::tr("pa_context_get_source_info_list() failed").toUtf8().constData());
            return;
        }
        pa_operation_unref(o);
        m_n_outstanding++;

        if (!(o = pa_context_get_sink_input_info_list(m_context, &PulseControl::sink_input_cb, this))) {
            show_error(QObject::tr("pa_context_get_sink_input_info_list() failed").toUtf8().constData());
            return;
        }
        pa_operation_unref(o);
        m_n_outstanding++;

        if (!(o = pa_context_get_source_output_info_list(m_context, &PulseControl::source_output_cb, this))) {
            show_error(QObject::tr("pa_context_get_source_output_info_list() failed").toUtf8().constData());
            return;
        }
        pa_operation_unref(o);
        m_n_outstanding++;

        /* These calls are not always supported */
        if ((o = pa_ext_stream_restore_read(m_context, ext_stream_restore_read_cb, this))) {
            pa_operation_unref(o);
            m_n_outstanding++;

            pa_ext_stream_restore_set_subscribe_cb(m_context, ext_stream_restore_subscribe_cb, this);

            if ((o = pa_ext_stream_restore_subscribe(m_context, 1, nullptr, nullptr)))
                pa_operation_unref(o);

        } else
            g_debug(QObject::tr("Failed to initialize stream_restore extension: %s").toUtf8().constData(), pa_strerror(pa_context_errno(m_context)));

#if HAVE_EXT_DEVICE_RESTORE_API
        /* TODO Change this to just the test function */
        if ((o = pa_ext_device_restore_read_formats_all(m_context, ext_device_restore_read_cb, this))) {
            pa_operation_unref(o);
            m_n_outstanding++;

            pa_ext_device_restore_set_subscribe_cb(m_context, ext_device_restore_subscribe_cb, this);

            if ((o = pa_ext_device_restore_subscribe(m_context, 1, nullptr, nullptr)))
                pa_operation_unref(o);

        } else
            g_debug(QObject::tr("Failed to initialize device restore extension: %s").toUtf8().constData(), pa_strerror(pa_context_errno(m_context)));

#endif

        if ((o = pa_ext_device_manager_read(m_context, ext_device_manager_read_cb, this))) {
            pa_operation_unref(o);
            m_n_outstanding++;

            pa_ext_device_manager_set_subscribe_cb(m_context, ext_device_manager_subscribe_cb, this);

            if ((o = pa_ext_device_manager_subscribe(m_context, 1, nullptr, nullptr)))
                pa_operation_unref(o);

        }
        else
            g_debug(QObject::tr("Failed to initialize device manager extension: %s").toUtf8().constData(), pa_strerror(pa_context_errno(m_context)));

        break;
    }

    case PA_CONTEXT_FAILED:
        m_mainWindow->setConnectionState(false);
        m_mainWindow->removeAllWidgets();
        m_mainWindow->updateDeviceVisibility();
        pa_context_unref(m_context);
        m_context = nullptr;

        if (m_reconnect_timeout > 0) {
            g_debug("%s", QObject::tr("Connection failed, attempting reconnect").toUtf8().constData());
  //          g_timeout_add_seconds(m_reconnect_timeout, connect_to_pulse, w);
        }
        return;

    case PA_CONTEXT_TERMINATED:
    default:
        qApp->quit();
        return;
    }
}

void PulseControl::context_state_callback(pa_context *c, void *userdata)
{
    PulseControl *pc = static_cast<PulseControl*>(userdata);
    g_assert(c);
    if(pc)
        pc->contextStateCallback();
}

void PulseControl::subscribeCallback(pa_subscription_event_type_t t, uint32_t index)
{
    switch (t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) {
        case PA_SUBSCRIPTION_EVENT_SINK:
            if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE)
                m_mainWindow->removeSink(index);
            else {
                pa_operation *o;
                if (!(o = pa_context_get_sink_info_by_index(m_context, index, &PulseControl::sink_cb, this))) {
                    show_error(QObject::tr("pa_context_get_sink_info_by_index() failed").toUtf8().constData());
                    return;
                }
                pa_operation_unref(o);
            }
            break;

        case PA_SUBSCRIPTION_EVENT_SOURCE:
            if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE)
                m_mainWindow->removeSource(index);
            else {
                pa_operation *o;
                if (!(o = pa_context_get_source_info_by_index(m_context, index, &PulseControl::source_cb, this))) {
                    show_error(QObject::tr("pa_context_get_source_info_by_index() failed").toUtf8().constData());
                    return;
                }
                pa_operation_unref(o);
            }
            break;

        case PA_SUBSCRIPTION_EVENT_SINK_INPUT:
            if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE)
                m_mainWindow->removeSinkInput(index);
            else {
                pa_operation *o;
                if (!(o = pa_context_get_sink_input_info(m_context, index, &PulseControl::sink_input_cb, this))) {
                    show_error(QObject::tr("pa_context_get_sink_input_info() failed").toUtf8().constData());
                    return;
                }
                pa_operation_unref(o);
            }
            break;

        case PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT:
            if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE)
                m_mainWindow->removeSourceOutput(index);
            else {
                pa_operation *o;
                if (!(o = pa_context_get_source_output_info(m_context, index, &PulseControl::source_output_cb, this))) {
                    show_error(QObject::tr("pa_context_get_sink_input_info() failed").toUtf8().constData());
                    return;
                }
                pa_operation_unref(o);
            }
            break;

        case PA_SUBSCRIPTION_EVENT_CLIENT:
            if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE)
                m_mainWindow->removeClient(index);
            else {
                pa_operation *o;
                if (!(o = pa_context_get_client_info(m_context, index, &PulseControl::client_cb, this))) {
                    show_error(QObject::tr("pa_context_get_client_info() failed").toUtf8().constData());
                    return;
                }
                pa_operation_unref(o);
            }
            break;

        case PA_SUBSCRIPTION_EVENT_SERVER: {
                pa_operation *o;
                if (!(o = pa_context_get_server_info(m_context, &PulseControl::server_info_cb, this))) {
                    show_error(QObject::tr("pa_context_get_server_info() failed").toUtf8().constData());
                    return;
                }
                pa_operation_unref(o);
            }
            break;

        case PA_SUBSCRIPTION_EVENT_CARD:
            if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE)
                m_mainWindow->removeCard(index);
            else {
                pa_operation *o;
                if (!(o = pa_context_get_card_info_by_index(m_context, index, &PulseControl::card_cb, this))) {
                    show_error(QObject::tr("pa_context_get_card_info_by_index() failed").toUtf8().constData());
                    return;
                }
                pa_operation_unref(o);
            }
            break;

    }
}

void PulseControl::subscribe_cb(pa_context *c, pa_subscription_event_type_t t, uint32_t index, void *userdata)
{
    PulseControl *pc = static_cast<PulseControl*>(userdata);
    g_assert(c);
    if(pc)
        pc->subscribeCallback(t, index);
}

void PulseControl::cardCallback(const pa_card_info *i, int eol)
{
    if (eol < 0) {
        if (pa_context_errno(m_context) == PA_ERR_NOENTITY)
            return;

        show_error(QObject::tr("Card callback failure").toUtf8().constData());
        return;
    }

    if (eol > 0) {
        dec_outstanding();
        return;
    }

    m_mainWindow->updateCard(*i);
}

void PulseControl::card_cb(pa_context *c, const pa_card_info *i, int eol, void *userdata)
{
    PulseControl *pc = static_cast<PulseControl*>(userdata);
    g_assert(c);
    if(pc)
        pc->cardCallback(i, eol);
}

void PulseControl::sinkCallback(const pa_sink_info *i, int eol)
{
    if (eol < 0) {
        if (pa_context_errno(m_context) == PA_ERR_NOENTITY)
            return;

        show_error(QObject::tr("Sink callback failure").toUtf8().constData());
        return;
    }

    if (eol > 0) {
        dec_outstanding();
        return;
    }
#if HAVE_EXT_DEVICE_RESTORE_API
    if (m_mainWindow->updateSink(*i))
        PulseControl::ext_device_restore_subscribe_cb(m_context, PA_DEVICE_TYPE_SINK, i->index, this);
#else
    m_mainWindow->updateSink(*i);
#endif
}

void PulseControl::sink_cb(pa_context *c, const pa_sink_info *i, int eol, void *userdata)
{
    PulseControl *pc = static_cast<PulseControl*>(userdata);
    g_assert(c);
    if(pc)
        pc->sinkCallback(i, eol);
}

void PulseControl::sourceCallback(const pa_source_info *i, int eol)
{
    if (eol < 0) {
        if (pa_context_errno(m_context) == PA_ERR_NOENTITY)
            return;

        show_error(QObject::tr("Source callback failure").toUtf8().constData());
        return;
    }

    if (eol > 0) {
        dec_outstanding();
        return;
    }

    m_mainWindow->updateSource(*i);
}

void PulseControl::source_cb(pa_context *c, const pa_source_info *i, int eol, void *userdata)
{
    PulseControl *pc = static_cast<PulseControl*>(userdata);
    g_assert(c);
    if(pc)
        pc->sourceCallback(i, eol);
}

void PulseControl::sinkInputCallback(const pa_sink_input_info *i, int eol)
{
    if (eol < 0) {
        if (pa_context_errno(m_context) == PA_ERR_NOENTITY)
            return;

        show_error(QObject::tr("Sink input callback failure").toUtf8().constData());
        return;
    }

    if (eol > 0) {
        dec_outstanding();
        return;
    }

    m_mainWindow->updateSinkInput(*i);
}

void PulseControl::sink_input_cb(pa_context *c, const pa_sink_input_info *i, int eol, void *userdata)
{
    PulseControl *pc = static_cast<PulseControl*>(userdata);
    g_assert(c);
    if(pc)
        pc->sinkInputCallback(i, eol);
}

void PulseControl::sourceOutputCallback(const pa_source_output_info *i, int eol)
{
    if (eol < 0) {
        if (pa_context_errno(m_context) == PA_ERR_NOENTITY)
            return;

        show_error(QObject::tr("Source output callback failure").toUtf8().constData());
        return;
    }

    if (eol > 0)  {
        dec_outstanding();
        return;
    }

    m_mainWindow->updateSourceOutput(*i);
}

void PulseControl::source_output_cb(pa_context *c, const pa_source_output_info *i, int eol, void *userdata)
{
    PulseControl *pc = static_cast<PulseControl*>(userdata);
    g_assert(c);
    if(pc)
        pc->sourceOutputCallback(i, eol);
}

void PulseControl::clientCallback(const pa_client_info *i, int eol)
{
    if (eol < 0) {
        if (pa_context_errno(m_context) == PA_ERR_NOENTITY)
            return;

        show_error(QObject::tr("Client callback failure").toUtf8().constData());
        return;
    }

    if (eol > 0) {
        dec_outstanding();
        return;
    }

    m_mainWindow->updateClient(*i);
}

void PulseControl::client_cb(pa_context *c, const pa_client_info *i, int eol, void *userdata)
{
    PulseControl *pc = static_cast<PulseControl*>(userdata);
    g_assert(c);
    if(pc)
        pc->clientCallback(i, eol);
}

void PulseControl::serverInfoCallback(const pa_server_info *i)
{
    if (!i) {
        show_error(QObject::tr("Server info callback failure").toUtf8().constData());
        return;
    }

    m_mainWindow->updateServer(*i);
    dec_outstanding();
}

void PulseControl::server_info_cb(pa_context *c, const pa_server_info *i, void *userdata)
{
    PulseControl *pc = static_cast<PulseControl*>(userdata);
    g_assert(c);
    if(pc)
        pc->serverInfoCallback(i);
}


void PulseControl::extStreamRestoreReadCallback(const pa_ext_stream_restore_info *i, int eol)
{
    if (eol < 0) {
        dec_outstanding();
        g_debug(QObject::tr("Failed to initialize stream_restore extension: %s").toUtf8().constData(), pa_strerror(pa_context_errno(m_context)));
        m_mainWindow->deleteEventRoleWidget();
        return;
    }

    if (eol > 0) {
        dec_outstanding();
        return;
    }

    m_mainWindow->updateRole(*i);
}

void PulseControl::ext_stream_restore_read_cb(pa_context *c, const pa_ext_stream_restore_info *i, int eol, void *userdata)
{
    PulseControl *pc = static_cast<PulseControl*>(userdata);
    g_assert(c);
    if(pc)
        pc->extStreamRestoreReadCallback(i, eol);
}

void PulseControl::extStreamRestoreSubscribeCallback()
{
    pa_operation *o;

    if (!(o = pa_ext_stream_restore_read(m_context, &PulseControl::ext_stream_restore_read_cb, this))) {
        show_error(QObject::tr("pa_ext_stream_restore_read() failed").toUtf8().constData());
        return;
    }

    pa_operation_unref(o);
}

void PulseControl::ext_stream_restore_subscribe_cb(pa_context *c, void *userdata)
{
    PulseControl *pc = static_cast<PulseControl*>(userdata);
    g_assert(c);
    if(pc)
        pc->extStreamRestoreSubscribeCallback();
}

void PulseControl::extDeviceManagerReadCallback(const pa_ext_device_manager_info *info, int eol)
{
    Q_UNUSED(info);
    if (eol < 0) {
        dec_outstanding();
        g_debug(QObject::tr("Failed to initialize device manager extension: %s").toUtf8().constData(), pa_strerror(pa_context_errno(m_context)));
        return;
    }

    m_mainWindow->setCanRenameDevices(true);

    if (eol > 0) {
        dec_outstanding();
        return;
    }
}

void PulseControl::ext_device_manager_read_cb(pa_context *c, const pa_ext_device_manager_info *info, int eol, void *userdata)
{
    PulseControl *pc = static_cast<PulseControl*>(userdata);
    g_assert(c);
    if(pc)
        pc->extDeviceManagerReadCallback(info, eol);
}


void PulseControl::extDeviceManagerSubscribeCallback()
{
    pa_operation *o;

    if (!(o = pa_ext_device_manager_read(m_context, &PulseControl::ext_device_manager_read_cb, this))) {
        show_error(QObject::tr("pa_ext_device_manager_read() failed").toUtf8().constData());
        return;
    }

    pa_operation_unref(o);
}



void PulseControl::ext_device_manager_subscribe_cb(pa_context *c, void *userdata)
{
    PulseControl *pc = static_cast<PulseControl*>(userdata);
    g_assert(c);
    if(pc)
        pc->extDeviceManagerSubscribeCallback();
}

//#if HAVE_EXT_DEVICE_RESTORE_API
void PulseControl::extDeviceRestoreReadCallback(const pa_ext_device_restore_info *i, int eol)
{
    if (eol < 0) {
        dec_outstanding();
        g_debug(QObject::tr("Failed to initialize device restore extension: %s").toUtf8().constData(), pa_strerror(pa_context_errno(m_context)));
        return;
    }

    if (eol > 0) {
        dec_outstanding();
        return;
    }

    /* Do something with a widget when this part is written */
    m_mainWindow->updateDeviceInfo(*i);
}

void PulseControl::ext_device_restore_read_cb(pa_context *c, const pa_ext_device_restore_info *i, int eol, void *userdata)
{
    PulseControl *pc = static_cast<PulseControl*>(userdata);
    g_assert(c);
    if(pc)
        pc->extDeviceRestoreReadCallback(i, eol);
}

void PulseControl::extDeviceRestoreSubscribeCallback(pa_device_type_t type, uint32_t idx)
{
    pa_operation *o;

    if (type != PA_DEVICE_TYPE_SINK)
        return;

    if (!(o = pa_ext_device_restore_read_formats(m_context, type, idx, &PulseControl::ext_device_restore_read_cb, this))) {
        show_error(QObject::tr("pa_ext_device_restore_read_sink_formats() failed").toUtf8().constData());
        return;
    }

    pa_operation_unref(o);
}

void PulseControl::ext_device_restore_subscribe_cb(pa_context *c, pa_device_type_t type, uint32_t idx, void *userdata)
{
    PulseControl *pc = static_cast<PulseControl*>(userdata);
    g_assert(c);
    if(pc)
        pc->extDeviceRestoreSubscribeCallback(type, idx);
}
//#endif

void PulseControl::dec_outstanding()
{
    if(m_n_outstanding <= 0)
        return;

    if(--m_n_outstanding <= 0)
        m_mainWindow->setConnectionState(true);
}

pa_context* get_context() {
    return g_context;
}


void show_error(const char *txt) {
    char buf[256];

    snprintf(buf, sizeof(buf), "%s: %s", txt, pa_strerror(pa_context_errno(g_context)));

    QMessageBox::critical(nullptr, QObject::tr("Error"), QString::fromUtf8(buf));
    qApp->quit();
}
