# -*- coding: utf-8 -*-
# SYMBIAN_UID = 0xE66B8ECD

import os
import sys
sys.setdefaultencoding("utf-8")
sys.path.append('C:\\python\lib')
sys.path.append('E:\\python\lib')

import string
import appuifw
import e32
try:
	import pyS60uiutil
except:
	appuifw.note(unicode("No pyS60uiutil.py",), "error")
	sys.exit(0)

vcodec_list = [u'disable', u'copy', u'asv1', u'asv2', u'bmp', u'dnxhd', u'dvvideo', u'ffv1', u'ffvhuff', u'flashsv', u'flv', u'gif', u'h261', u'h263', u'h263p', u'huffyuv', u'jpegls', u'ljpeg', u'mjpeg', u'mpeg1video', u'mpeg2video', u'mpeg4', u'msmpeg4', u'msmpeg4v1', u'msmpeg4v2', u'pam', u'pbm', u'pgm', u'pgmyuv', u'png', u'ppm', u'qtrle', u'rawvideo', u'roqvideo', u'rv10', u'rv20', u'sgi', u'snow', u'svq1', u'targa', u'tiff', u'wmv1', u'wmv2', u'zlib', u'zmbv']
acodec_list = [u'disable', u'copy', u'ac3', u'adpcm_adx', u'adpcm_ima_qt', u'adpcm_ima_wav', u'adpcm_ms', u'adpcm_swf', u'adpcm_yamaha', u'flac', u'g726', u'libfaac', u'libmp3lame', u'libvorbis', u'mp2', u'pcm_alaw', u'pcm_mulaw', u'pcm_s16be', u'pcm_s16le', u'pcm_s24be', u'pcm_s24daud', u'pcm_s24le', u'pcm_s32be', u'pcm_s32le', u'pcm_s8', u'pcm_u16be', u'pcm_u16le', u'pcm_u24be', u'pcm_u24le', u'pcm_u32be', u'pcm_u32le', u'pcm_u8', u'pcm_zork', u'roq_dpcm', u'sonic', u'sonicls', u'vorbis', u'wmav1', u'wmav2']
format_list = [u'', u'3g2', u'3gp', u'RoQ', u'ac3', u'adts', u'aiff', u'alaw', u'amr', u'asf', u'asf_stream', u'au', u'avi', u'avm2', u'crc', u'dirac', u'dts', u'dv', u'dvd', u'ffm', u'flac', u'flv', u'framecrc', u'gif', u'gxf', u'h261', u'h263', u'h264', u'image2', u'image2pipe', u'ipod', u'm4v', u'matroska', u'mjpeg', u'mmf', u'mov', u'mp2', u'mp3', u'mp4', u'mpeg', u'mpeg1video', u'mpeg2video', u'mpegts', u'mpjpeg', u'mulaw', u'null', u'nut', u'ogg', u'psp', u'rawvideo', u'rm', u'rtp', u's16be', u's16le', u's8', u'svcd', u'swf', u'u16be', u'u16le', u'u8', u'vcd', u'vob', u'voc', u'wav', u'yuv4mpegpipe']

input_file = u''
output_file = u''
vcodec_idx = 0
acodec_idx = 0
format_idx = 0

view_update = False
old_app_info = None

script_lock = e32.Ao_lock()

##########################################################
def view_main_callback():
	functions = [
		view_main_edit_input_file,
		view_main_edit_output_file,
		view_vcodec_select,
		view_acodec_select,
		view_format_select
	]
	global view_main
	functions[view_main.current()]()

def view_main_edit_input_file():
	global input_file
	r = appuifw.query(u'Input File', 'text', input_file)
	if r:
		input_file = r
		view_main_update()

def view_main_edit_output_file():
	global output_file
	r = appuifw.query(u'Output File', 'text', output_file)
	if r:
		output_file = r
		view_main_update()

def view_main_callback_menu():
	sel = pyS60uiutil.dirBrowser().select()
	if sel:
		global input_file, output_file
		input_file = unicode(os.path.join(sel[0], sel[1]), 'utf-8')
		output_file = input_file + u'.avi'
		view_main_update()

def view_main_update():
	global view_main
	fields = [
		(u'Input File', input_file),
		(u'Output File', output_file),
		(u'Video Codec', vcodec_list[vcodec_idx]),
		(u'Audio Codec', acodec_list[acodec_idx]),
		(u'Format', format_list[format_idx]),
	]
	view_main = appuifw.Listbox(fields, view_main_callback)
	appuifw.app.body = view_main
	view_update = False

def view_main_exit():
	appuifw.app.exit_key_handler = None
	script_lock.signal()
	sys.exit()

def view_sub_exit():
	pyS60uiutil.restore_app_info(old_app_info)
	if view_update:
		view_main_update()

##########################################################
def view_vcodec_select():
	global old_app_info
	old_app_info = pyS60uiutil.save_current_app_info()
	view_update = False
	appuifw.app.exit_key_handler = view_sub_exit;
	appuifw.app.body = view_vcodec
	appuifw.app.menu = []

def view_vcodec_callback():
	global vcodec_idx, view_update
	vcodec_idx = view_vcodec.current()
	view_update = True
	view_sub_exit()

##########################################################
def view_acodec_select():
	global old_app_info
	old_app_info = pyS60uiutil.save_current_app_info()
	view_update = False
	appuifw.app.exit_key_handler = view_sub_exit;
	appuifw.app.body = view_acodec
	appuifw.app.menu = []

def view_acodec_callback():
	global acodec_idx, view_update
	acodec_idx = view_acodec.current()
	view_update = True
	view_sub_exit()

##########################################################
def view_format_select():
	global old_app_info
	old_app_info = pyS60uiutil.save_current_app_info()
	view_update = False
	appuifw.app.exit_key_handler = view_sub_exit;
	appuifw.app.body = view_format
	appuifw.app.menu = []

def view_format_callback():
	global format_idx, view_update
	format_idx = view_format.current()
	view_update = True
	view_sub_exit()

##########################################################
def ffmpeg_start():
	ffmpeg = u'ffmpeg.exe'
	opt = [u'-y']

	opt_input = [u'-i', input_file]
	opt_output = [output_file]

	opt_fmt = []
	if format_idx > 0:
		opt_fmt = [u'-fmt', format_list[format_idx]]

	opt_vcodec = []
	if vcodec_idx > 0:
		opt_vcodec = [u'-vcodec', vcodec_list[vcodec_idx]]
	else:
		opt_vcodec = [u'-vn']

	opt_acodec = []
	if acodec_idx > 0:
		opt_acodec = [u'-acodec', acodec_list[acodec_idx]]
	else:
		opt_acodec = [u'-an']

	opt.extend(opt_input)
	opt.extend(opt_vcodec)
	opt.extend(opt_acodec)
	opt.extend(opt_fmt)
	opt.extend(opt_output)

	options = string.join(opt, ' ')

	if appuifw.query(ffmpeg + ' ' + options, 'query'):
		e32.start_exe(ffmpeg, options);

##########################################################
view_vcodec = appuifw.Listbox(vcodec_list, view_vcodec_callback)
view_acodec = appuifw.Listbox(acodec_list, view_acodec_callback)
view_format = appuifw.Listbox(format_list, view_format_callback)

if __name__ == '__main__':
	view_main = None
	view_main_menu = [
		(u'Select Input File', view_main_callback_menu),
		(u'Start', ffmpeg_start),
		(u'Exit', view_main_exit)
	]

	appuifw.app.exit_key_handler = view_main_exit;
	appuifw.app.body = view_main
	appuifw.app.menu = view_main_menu

	view_main_update()

	script_lock.wait()
