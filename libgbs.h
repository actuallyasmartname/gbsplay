/*
 * gbsplay is a Gameboy sound player
 *
 * 2003-2020 (C) by Tobias Diedrich <ranma+gbsplay@tdiedrich.de>
 *                  Christian Garbs <mitch@cgarbs.de>
 *
 * Licensed under GNU GPL v1 or, at your option, any later version.
 */

#ifndef _LIBGBS_H_
#define _LIBGBS_H_

/***
 ***  THIS IS THE OFFICIAL EXTERNAL API, DON'T MAKE ANY INCOMPATIBLE CHANGES!
 ***/

/**
 * @file libgbs.h
 * API of libgbs.
 * This file contains the API definition of libgbs, the Gameboy sound player library.
 */

#include <inttypes.h>
#include "common.h"

// FIXME: call everything (structs, enums, types, functions) "libgbs*" instead "gbs*"
//        to prevent internal/external namespace clashes?

//
//////  defines
//

#define GBS_LEN_SHIFT 10
#define GBS_LEN_DIV   (1 << GBS_LEN_SHIFT)

//
//////  globals
//

/**
 * @value gbs_errno
 * Error indicator.  Contains additional error information if the
 * previously called function can't directly return an error.
 * Possible error codes are listed with the function.
 */
extern __thread int gbs_errno;

//
//////  structs
//

/**
 * @struct gbs
 * gbs instance.  Opaque handle to a gbs player instance corresponding
 * to one gbs file.  Completely encapsulates the current player status
 * and allows multiple gbs files to be handled simultaneously.
 */
struct gbs;

/**
 * Sound output buffer.  Contains the next bit of calculated sound
 * output that can be passed to an audio driver or be processed
 * otherwise.
 */
struct gbs_output_buffer {
	int16_t *data;
	long bytes;
	long pos;
};

/**
 * Channel status.  Contains information about the current state of
 * one of the four emulated sound channels.
 */
struct gbs_channel_status {
	long mute;
	long vol;
	long div_tc;
	long playing;
};

/**
 * Player status.  Contains information about the current state of the
 * player routine.
 */
struct gbs_status {
	char *songtitle;
	int subsong;
	uint32_t subsong_len;  /* GBS_LEN_DIV (1024) == 1 second */
	uint8_t songs;
	uint8_t defaultsong;
	long lvol;
	long rvol;
	long long ticks;
	struct gbs_channel_status ch[4];
};

//
//////  enums
//

/**
 * Filter type.  Enumerates the available audio filters emulating
 * different hardware variants.
 */
enum gbs_filter_type {
	FILTER_OFF, /**< no filter */
	FILTER_DMG, /**< Gameboy classic high-pass filter */
	FILTER_CGB, /**< Gameboy Color high-pass filter */
};

//
//////  typedefs
//

/**
 * IO callback.  This callback gets executed when a write to an IO
 * address has happened.
 *
 * @param gbs     reference to the gbs instance that executed the IO
 * @param cycles  hardware cycles since start of current subsong
 * @param addr    the IO address that was written to
 * @param value   the value that was writton
 * @param priv    opaque private context pointer for the callback handler
 */
typedef void (*gbs_io_cb)(struct gbs *gbs, long cycles, uint32_t addr, uint8_t value, void *priv);

/**
 * Step callback.  This callback gets executed after each machine instruction
 *
 * This is "step"ing through machine instructions in the CPU emulation
 *
 * @param gbs       reference to the gbs instance that executed the IO
 * @param cycles    hardware cycles since start of current subsong
 * @param channels  current status of all 4 channels
 * @param priv      opaque private context pointer for the callback handler
 */
typedef void (*gbs_step_cb)(struct gbs *gbs, const long cycles, const struct gbs_channel_status channels[], void *priv);

/**
 * Sound callback.  This callback gets executed when the next part of
 * sound has been rendered into the sound output buffer.  The caller
 * could for example pass the buffer to a sound driver or write it to
 * a file.
 *
 * @param gbs   reference to the gbs instance that executed the IO
 * @param buf   the output buffer containing the rendered sound
 * @param priv  opaque private context pointer for the callback handler
 */
typedef void (*gbs_sound_cb)(struct gbs *gbs, struct gbs_output_buffer *buf, void *priv);

/**
 * Next subsong callback.  This callback gets executed when the
 * current subsong has finished playing.  The caller can for example
 * decide to start the next subsong via gbs_init() or end the playback
 * altogether.  The just finished subsong as well as the total number
 * of subsongs are available via gbs_get_status().
 *
 * FIXME: add subsong + songs directly to the callback?
 *
 * @param gbs     reference to the gbs instance that executed the IO
 * @param priv    opaque private context pointer for the callback handler
 */
typedef long (*gbs_nextsubsong_cb)(struct gbs *gbs, void *priv);

//
//////  functions
//

/**
 * Open GBS file.  The given file is read and the contents are
 * returned as an initialized @link struct gbs @endlink.  gbsplay can
 * handle multiple files in parallel, so the handle is needed for
 * nearly all function calls.
 *
 * On error returns NULL and sets @link gbs_errno @endlink to one of:
 *
 * <dl>
 *  <dt>-1<dt> <dl>could not open file (see libc <code>errno</code> for details)</dl>
 *  <dt>-2<dt> <dl>could not stat file (see libc <code>errno</code> for details)</dl>
 *  <dt>-3<dt> <dl>file is too big</dl>
 *  <dt>-4<dt> <dl>other read error (see libc <code>errno</code> for details)</dl>
 *  <dt>-5<dt> <dl>file is no GBS file</dl>
 *  <dt>-6<dt> <dl>unsupported GBS version</dl>
 *  <dt>-7<dt> <dl>unreasonable number of subsongs</dl>
 *  <dt>-8<dt> <dl>default subsong out of range</dl>
 *  <dt>-9<dt> <dl>zlib support is missing</dl>
 * <dt>-10<dt> <dl>inflateInit2 failed on zlib uncompress</dl>
 * <dt>-11<dt> <dl>inflate failed on zlib uncompress</dl>
 * <dt>-12<dt> <dl>file is no GBR file</dl>
 * <dt>-13<dt> <dl>unsupported timerflag value (GBR)</dl>
 * <dt>-14<dt> <dl>file is no VGM file</dl>
 * <dt>-15<dt> <dl>unsupported VGM version</dl>
 * <dt>-16<dt> <dl>unsupported DMG clock (VGM)</dl>
 * <dt>-17<dt> <dl>bad file size in header (VGM)</dl>
 * <dt>-18<dt> <dl>bad GD3 offset (VGM)</dl>
 * <dt>-19<dt> <dl>bad data length (VGM)</dl>
 * <dt>-20<dt> <dl>unsupported VGM opcode</dl>
 * <dt>-21<dt> <dl>unsupported cartridge type (GB)</dl>
 * </dl>
 *
 * @param name  filename to open (optionally including a path)
 * @return an opaque @link struct gbs @endlink to be passed to other functions or NULL on error
 */
struct gbs *gbs_open(const char *name);

void gbs_configure(struct gbs *gbs, long subsong, long subsong_timeout, long silence_timeout, long subsong_gap, long fadeout);
void gbs_configure_channels(struct gbs *gbs, long mute_0, long mute_1, long mute_2, long mute_3);
void gbs_configure_output(struct gbs *gbs, struct gbs_output_buffer *buf, long rate);
const char *gbs_get_title(struct gbs *gbs);
const char *gbs_get_author(struct gbs *gbs);
const char *gbs_get_copyright(struct gbs *gbs);

/**
 * Play GBS subsong.  Start playing the given subsong.
 *
 * On error returns 0 and sets @link gbs_errno @endlink to one of:
 *
 * <dl>
 *  <dt>-1<dt> <dl>subsong number is out of range</dl>
 * </dl>
 *
 * @param gbs  opaque  @link struct gbs @endlink handle
 * @param subsong Number of the subsong to play (zero-based).  -1 will play the default subsong.
 * @return 1 on success, 0 on error
 */
long gbs_init(struct gbs *gbs, long subsong);
uint8_t gbs_io_peek(struct gbs *gbs, uint16_t addr);
const struct gbs_status* gbs_get_status(struct gbs *gbs);
long gbs_step(struct gbs *gbs, long time_to_work);
void gbs_set_nextsubsong_cb(struct gbs *gbs, gbs_nextsubsong_cb cb, void *priv);
void gbs_set_io_callback(struct gbs *gbs, gbs_io_cb fn, void *priv);
void gbs_set_step_callback(struct gbs *gbs, gbs_step_cb fn, void *priv);
void gbs_set_sound_callback(struct gbs *gbs, gbs_sound_cb fn, void *priv);
long gbs_set_filter(struct gbs *gbs, enum gbs_filter_type type);
void gbs_print_info(struct gbs *gbs, long verbose);
long gbs_toggle_mute(struct gbs *gbs, long channel);
void gbs_close(struct gbs *gbs);
long gbs_write(struct gbs *gbs, char *name);

#endif
