/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//#define LOG_NDEBUG 0

#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/time.h>

#include <cutils/log.h>

#include <hardware/hardware.h>
#include <system/audio.h>
#include <hardware/audio.h>

#include <tinyalsa/asoundlib.h>

/* Mixer control names */
#define MIXER_MASTER_PLAYBACK_VOLUME    "Master Playback Volume"
#define MIXER_MASTER_PLAYBACK_SWITCH    "Master Playback Switch"
#define MIXER_PCM_PLAYBACK_VOLUME       "PCM Playback Volume"
#define MIXER_PCM_PLAYBACK_SWITCH       "PCM Playback Switch"
#define MIXER_CAPTURE_SOURCE		"Capture Source"
#define MIXER_CAPTURE_SWITCH		"Capture Switch"
#define MIXER_CAPTURE_VOLUME		"Capture Volume"

/* CARD/PORT */
#define DEFAULT_CARD 0
#define DEFAULT_PORT 0

struct vbox86_audio_device {
    struct audio_hw_device device;

    struct pcm_config m_pcm_config_out;
    struct pcm *m_pcm_out;
    struct pcm_config m_pcm_config_in;
    struct pcm *m_pcm_in;
    struct mixer *m_mixer;
};

struct vbox86_stream_out {
    struct audio_stream_out stream;

    struct vbox86_audio_device *dev;
};

struct vbox86_stream_in {
    struct audio_stream_in stream;

    struct vbox86_audio_device *dev;
};

static int set_mixer_value_int(struct mixer *mixer, char *value_name, int value)
{
    struct mixer_ctl *ctl;
    int j;

    ctl = mixer_get_ctl_by_name(mixer, value_name);
    if (!ctl)
	return -EINVAL;

    for (j = 0;j < mixer_ctl_get_num_values(ctl); j++) 
	mixer_ctl_set_value(ctl, j, value);

    return 0;
}

static uint32_t out_get_sample_rate(const struct audio_stream *stream)
{
    return 44100;
}

static int out_set_sample_rate(struct audio_stream *stream, uint32_t rate)
{
    return 0;
}

static size_t out_get_buffer_size(const struct audio_stream *stream)
{
    return 4096;
}

static uint32_t out_get_channels(const struct audio_stream *stream)
{
    return AUDIO_CHANNEL_OUT_STEREO;
}

static int out_get_format(const struct audio_stream *stream)
{
    return AUDIO_FORMAT_PCM_16_BIT;
}

static int out_set_format(struct audio_stream *stream, int format)
{
    return 0;
}

static int out_standby(struct audio_stream *stream)
{
    return 0;
}

static int out_dump(const struct audio_stream *stream, int fd)
{
    return 0;
}

static int out_set_parameters(struct audio_stream *stream, const char *kvpairs)
{
    return 0;
}

static char * out_get_parameters(const struct audio_stream *stream, const char *keys)
{
    return strdup("");
}

static uint32_t out_get_latency(const struct audio_stream_out *stream)
{
    return 0;
}

static int out_set_volume(struct audio_stream_out *stream, float left,
                          float right)
{
    return 0;
}

static ssize_t out_write(struct audio_stream_out *stream, const void* buffer,
                         size_t bytes)
{
    struct vbox86_audio_device *adev = ((struct vbox86_stream_out *)stream)->dev;
    int num_wrote;
    int num_to_fake=0;

    num_wrote = pcm_write(adev->m_pcm_out, buffer, bytes);
    if (num_wrote < 0)
        num_to_fake = bytes;

    if (num_to_fake>0) {
    	/* XXX: fake timing for audio output */
    	usleep(num_to_fake * 1000000 / audio_stream_frame_size(&stream->common) / out_get_sample_rate(&stream->common));
        return bytes;
    }
    return num_wrote;
}

static int out_get_render_position(const struct audio_stream_out *stream,
                                   uint32_t *dsp_frames)
{
    return -EINVAL;
}

static int out_add_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    return 0;
}

static int out_remove_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    return 0;
}

/** audio_stream_in implementation **/
static uint32_t in_get_sample_rate(const struct audio_stream *stream)
{
    return 44100;
}

static int in_set_sample_rate(struct audio_stream *stream, uint32_t rate)
{
    return 0;
}

static size_t in_get_buffer_size(const struct audio_stream *stream)
{
    struct vbox86_audio_device *adev = ((struct vbox86_stream_in *)stream)->dev;
    int size;
 
    size = (adev->m_pcm_config_in.period_size * 44100) / adev->m_pcm_config_in.rate;
    size = ((size + 15) / 16) * 16;

    return size * 2 * sizeof(short);
}

static uint32_t in_get_channels(const struct audio_stream *stream)
{
    return AUDIO_CHANNEL_IN_STEREO;
}

static int in_get_format(const struct audio_stream *stream)
{
    return AUDIO_FORMAT_PCM_16_BIT;
}

static int in_set_format(struct audio_stream *stream, int format)
{
    return 0;
}

static int in_standby(struct audio_stream *stream)
{
    return 0;
}

static int in_dump(const struct audio_stream *stream, int fd)
{
    return 0;
}

static int in_set_parameters(struct audio_stream *stream, const char *kvpairs)
{
    return 0;
}

static char * in_get_parameters(const struct audio_stream *stream,
                                const char *keys)
{
    return strdup("");
}

static int in_set_gain(struct audio_stream_in *stream, float gain)
{
    return 0;
}

static ssize_t in_read(struct audio_stream_in *stream, void* buffer,
                       size_t bytes)
{
    struct vbox86_audio_device *adev = ((struct vbox86_stream_in *)stream)->dev;
    int num_read;

    if (!adev->m_pcm_in) {
	ALOGE("in_read: No IN device\n");
	return -1;
    }

    num_read = pcm_read(adev->m_pcm_in, buffer, bytes);
    if (num_read < 0) {
	ALOGE("in_read: pcm_read returned %d with errno=%d\n", num_read, errno);
        usleep(bytes * 1000000 / audio_stream_frame_size(&stream->common) /
               in_get_sample_rate(&stream->common));
    }
    return num_read;
}

static uint32_t in_get_input_frames_lost(struct audio_stream_in *stream)
{
    return 0;
}

static int in_add_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    return 0;
}

static int in_remove_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    return 0;
}

//static int adev_open_output_stream(struct audio_hw_device *dev,
//                                   uint32_t devices, int *format,
//                                   uint32_t *channels, uint32_t *sample_rate,
//                                   struct audio_stream_out **stream_out)
static int adev_open_output_stream(struct audio_hw_device *dev,
                                   audio_io_handle_t handle,
                                   audio_devices_t devices,
                                   audio_output_flags_t flags,
                                   struct audio_config *config,
                                   struct audio_stream_out **stream_out)
{
    struct vbox86_audio_device *ladev = (struct stub_audio_device *)dev;
    struct vbox86_stream_out *out;
    int ret;

    out = (struct vbox86_stream_out *)calloc(1, sizeof(struct vbox86_stream_out));
    if (!out)
        return -ENOMEM;

    out->stream.common.get_sample_rate = out_get_sample_rate;
    out->stream.common.set_sample_rate = out_set_sample_rate;
    out->stream.common.get_buffer_size = out_get_buffer_size;
    out->stream.common.get_channels = out_get_channels;
    out->stream.common.get_format = out_get_format;
    out->stream.common.set_format = out_set_format;
    out->stream.common.standby = out_standby;
    out->stream.common.dump = out_dump;
    out->stream.common.set_parameters = out_set_parameters;
    out->stream.common.get_parameters = out_get_parameters;
    out->stream.common.add_audio_effect = out_add_audio_effect;
    out->stream.common.remove_audio_effect = out_remove_audio_effect;
    out->stream.get_latency = out_get_latency;
    out->stream.set_volume = out_set_volume;
    out->stream.write = out_write;
    out->stream.get_render_position = out_get_render_position;

    out->dev = dev;

    config->format = out_get_format(&out->stream.common);
    config->channel_mask = out_get_channels(&out->stream.common);
    config->sample_rate = out_get_sample_rate(&out->stream.common);

    *stream_out = &out->stream;
    return 0;

err_open:
    free(out);
    *stream_out = NULL;
    return ret;
}

static void adev_close_output_stream(struct audio_hw_device *dev,
                                     struct audio_stream_out *stream)
{
    free(stream);
}

static int adev_set_parameters(struct audio_hw_device *dev, const char *kvpairs)
{
    return -ENOSYS;
}

static char * adev_get_parameters(const struct audio_hw_device *dev,
                                  const char *keys)
{
    return NULL;
}

static int adev_init_check(const struct audio_hw_device *dev)
{
    return 0;
}

static int adev_set_voice_volume(struct audio_hw_device *dev, float volume)
{
    return -ENOSYS;
}

static int adev_set_master_volume(struct audio_hw_device *dev, float volume)
{
    return -ENOSYS;
}

static int adev_set_mode(struct audio_hw_device *dev, int mode)
{
    return 0;
}

static int adev_set_mic_mute(struct audio_hw_device *dev, bool state)
{
    return -ENOSYS;
}

static int adev_get_mic_mute(const struct audio_hw_device *dev, bool *state)
{
    return -ENOSYS;
}

static size_t adev_get_input_buffer_size(const struct audio_hw_device *dev,
                                         uint32_t sample_rate, int format,
                                         int channel_count)
{
    return 320;
}

//static int adev_open_input_stream(struct audio_hw_device *dev, uint32_t devices,
//                                  int *format, uint32_t *channels,
//                                  uint32_t *sample_rate,
//                                  audio_in_acoustics_t acoustics,
//                                  struct audio_stream_in **stream_in)
static int adev_open_input_stream(struct audio_hw_device *dev,
                                  audio_io_handle_t handle,
                                  audio_devices_t devices,
                                  struct audio_config *config,
                                  struct audio_stream_in **stream_in)
{
    struct vbox86_audio_device *ladev = (struct stub_audio_device *)dev;
    struct vbox86_stream_in *in;
    int ret;

    config->format = in_get_format(&in->stream.common);
    config->channel_mask = in_get_channels(&in->stream.common);
    config->sample_rate = in_get_sample_rate(&in->stream.common);

    in = (struct vbox86_stream_in *)calloc(1, sizeof(struct vbox86_stream_in));
    if (!in)
        return -ENOMEM;

    in->stream.common.get_sample_rate = in_get_sample_rate;
    in->stream.common.set_sample_rate = in_set_sample_rate;
    in->stream.common.get_buffer_size = in_get_buffer_size;
    in->stream.common.get_channels = in_get_channels;
    in->stream.common.get_format = in_get_format;
    in->stream.common.set_format = in_set_format;
    in->stream.common.standby = in_standby;
    in->stream.common.dump = in_dump;
    in->stream.common.set_parameters = in_set_parameters;
    in->stream.common.get_parameters = in_get_parameters;
    in->stream.common.add_audio_effect = in_add_audio_effect;
    in->stream.common.remove_audio_effect = in_remove_audio_effect;
    in->stream.set_gain = in_set_gain;
    in->stream.read = in_read;
    in->stream.get_input_frames_lost = in_get_input_frames_lost;

    in->dev = dev;

    *stream_in = &in->stream;
    return 0;

err_open:
    free(in);
    *stream_in = NULL;
    return ret;
}

static void adev_close_input_stream(struct audio_hw_device *dev,
                                   struct audio_stream_in *in)
{
    return;
}

static int adev_dump(const audio_hw_device_t *device, int fd)
{
    return 0;
}

static int adev_close(hw_device_t *device)
{
    free(device);
    return 0;
}

static uint32_t adev_get_supported_devices(const struct audio_hw_device *dev)
{
    return (/* OUT */
            AUDIO_DEVICE_OUT_EARPIECE |
            AUDIO_DEVICE_OUT_SPEAKER |
            AUDIO_DEVICE_OUT_WIRED_HEADSET |
            AUDIO_DEVICE_OUT_WIRED_HEADPHONE |
            AUDIO_DEVICE_OUT_AUX_DIGITAL |
            AUDIO_DEVICE_OUT_ANLG_DOCK_HEADSET |
            AUDIO_DEVICE_OUT_DGTL_DOCK_HEADSET |
            AUDIO_DEVICE_OUT_ALL_SCO |
            AUDIO_DEVICE_OUT_DEFAULT |
            /* IN */
            AUDIO_DEVICE_IN_COMMUNICATION |
            AUDIO_DEVICE_IN_AMBIENT |
            AUDIO_DEVICE_IN_BUILTIN_MIC |
            AUDIO_DEVICE_IN_WIRED_HEADSET |
            AUDIO_DEVICE_IN_AUX_DIGITAL |
            AUDIO_DEVICE_IN_BACK_MIC |
            AUDIO_DEVICE_IN_ALL_SCO |
            AUDIO_DEVICE_IN_DEFAULT);
}

static int adev_open(const hw_module_t* module, const char* name,
                     hw_device_t** device)
{
    struct vbox86_audio_device *adev;
    int ret;

    if (strcmp(name, AUDIO_HARDWARE_INTERFACE) != 0)
        return -EINVAL;

    adev = calloc(1, sizeof(struct vbox86_audio_device));
    if (!adev)
        return -ENOMEM;

    adev->device.common.tag = HARDWARE_DEVICE_TAG;
    adev->device.common.version = AUDIO_DEVICE_API_VERSION_1_0;
    adev->device.common.module = (struct hw_module_t *) module;
    adev->device.common.close = adev_close;

    adev->device.get_supported_devices = adev_get_supported_devices;
    adev->device.init_check = adev_init_check;
    adev->device.set_voice_volume = adev_set_voice_volume;
    adev->device.set_master_volume = adev_set_master_volume;
    adev->device.set_mode = adev_set_mode;
    adev->device.set_mic_mute = adev_set_mic_mute;
    adev->device.get_mic_mute = adev_get_mic_mute;
    adev->device.set_parameters = adev_set_parameters;
    adev->device.get_parameters = adev_get_parameters;
    adev->device.get_input_buffer_size = adev_get_input_buffer_size;
    adev->device.open_output_stream = adev_open_output_stream;
    adev->device.close_output_stream = adev_close_output_stream;
    adev->device.open_input_stream = adev_open_input_stream;
    adev->device.close_input_stream = adev_close_input_stream;
    adev->device.dump = adev_dump;

    //Init ALSA
    memset(&adev->m_pcm_config_out, sizeof(struct pcm_config), 0);
    adev->m_pcm_config_out.channels = 2;
    adev->m_pcm_config_out.rate = 44100;
    adev->m_pcm_config_out.period_size = 1024;
    adev->m_pcm_config_out.period_count = 4;
    adev->m_pcm_config_out.format = PCM_FORMAT_S16_LE;
    adev->m_pcm_config_out.start_threshold = 0;
    adev->m_pcm_config_out.stop_threshold = 0;
    adev->m_pcm_config_out.silence_threshold = 0;

    adev->m_pcm_out = pcm_open(0, 0, PCM_OUT, &adev->m_pcm_config_out);
    if (!adev->m_pcm_out) {
	ALOGE("Error in pcm_open OUT");
	return -1;
    }

    memset(&adev->m_pcm_config_in, sizeof(struct pcm_config), 0);
    adev->m_pcm_config_in.channels = 2;
    adev->m_pcm_config_in.rate = 44100;
    adev->m_pcm_config_in.period_size = 1024;
    adev->m_pcm_config_in.period_count = 4;
    adev->m_pcm_config_in.format = PCM_FORMAT_S16_LE;
    adev->m_pcm_config_in.start_threshold = 0;
    adev->m_pcm_config_in.stop_threshold = 0;
    adev->m_pcm_config_in.silence_threshold = 0;

    adev->m_pcm_in = pcm_open(0, 0, PCM_IN, &adev->m_pcm_config_in);
    if (!adev->m_pcm_in) {
	ALOGE("Error in pcm_open IN");
	return -1;
    }

    adev->m_mixer = mixer_open(0);
    if (!adev->m_mixer) {
	ALOGE("Error in mixer_open");
	return -1;
    }

    set_mixer_value_int(adev->m_mixer, MIXER_MASTER_PLAYBACK_SWITCH, 1);
    set_mixer_value_int(adev->m_mixer, MIXER_MASTER_PLAYBACK_VOLUME, 20);
    set_mixer_value_int(adev->m_mixer, MIXER_PCM_PLAYBACK_SWITCH, 1);
    set_mixer_value_int(adev->m_mixer, MIXER_PCM_PLAYBACK_VOLUME, 20);
    set_mixer_value_int(adev->m_mixer, MIXER_CAPTURE_SWITCH, 1);
    set_mixer_value_int(adev->m_mixer, MIXER_CAPTURE_VOLUME, 20);

    *device = &adev->device.common;

    return 0;
}

static struct hw_module_methods_t hal_module_methods = {
    .open = adev_open,
};

struct audio_module HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = AUDIO_MODULE_API_VERSION_0_1,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = AUDIO_HARDWARE_MODULE_ID,
        .name = "VBox86 audio HW HAL",
        .author = "The Android Open Source Project",
        .methods = &hal_module_methods,
    },
};
