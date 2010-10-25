#include <stdio.h>
//#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include "avcodec.h"

class VideoEncoder {
public:
	VideoEncoder(CodecID codecId, int width, int height, int bitrate) {
		codec = avcodec_find_encoder(codecId);
		if (!codec) {
			fprintf(stderr, "Codec not found\n");
			return;
		}
		c = avcodec_alloc_context();
		picture = avcodec_alloc_frame();
		c->bit_rate = bitrate;
		c->width = width;
		c->height = height;
		c->time_base = (AVRational) {1, 25};
		c->gop_size = 10;
//		c->max_b_frames = 1;
		c->pix_fmt = PIX_FMT_YUV420P;

		picture->linesize[0] = c->width;
		picture->linesize[1] = c->width/2;
		picture->linesize[2] = c->width/2;

		if (avcodec_open(c, codec) < 0) {
			fprintf(stderr, "Could not open codec\n");
			return;
		}

		frames = 0;
	}

	void finish() {
		while (true) {
			int written = avcodec_encode_video(c, outbuf, sizeof(outbuf), NULL);
			if (written <= 0)
				break;
			storeData(written);
			frames++;
		}
	}

	virtual ~VideoEncoder() {
		avcodec_close(c);
		av_free(c);
		av_free(picture);
	}

	void encodeFrame(uint8_t *frame) {
		picture->data[0] = frame;
		picture->data[1] = frame + c->width*c->height;
		picture->data[2] = frame + c->width*c->height + (c->width/2) * (c->height/2);

		int written = avcodec_encode_video(c, outbuf, sizeof(outbuf), picture);
		storeData(written);
		frames++;
	}

	virtual void storeData(int size) = 0;

private:
	AVCodec* codec;
	AVCodecContext* c;
	AVFrame* picture;
protected:
	uint8_t outbuf[100000];
	int frames;
};

class MpegFileEncoder : public VideoEncoder {
public:
	MpegFileEncoder(int width, int height, int bitrate) : VideoEncoder(CODEC_ID_MPEG1VIDEO, width, height, bitrate) {
		out = fopen("e:\\video.mpg", "wb");
	}
	~MpegFileEncoder() {
		outbuf[0] = 0x00;
		outbuf[1] = 0x00;
		outbuf[2] = 0x01;
		outbuf[3] = 0xb7;
		fwrite(outbuf, 4, 1, out);
		fclose(out);
	}
	void storeData(int size) {
		fwrite(outbuf, size, 1, out);
		printf("writing frame %d, %d bytes\n", frames, size);
	}

private:
	FILE *out;
};

void doTest() {
	avcodec_init();
	avcodec_register_all();
	int width = 176;
	int height = 144;
	MpegFileEncoder enc(width, height, 100000);
	int size = width*height + 2*(width/2)*(height/2);
	uint8_t* buffer = new uint8_t[size];
	for (int i = 0; i < 15; i++) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				int Y = (x + y + 20*i) & 0xff;
				int U = x << 1;
				int V = y << 1;
				buffer[y*width + x] = Y;
				buffer[width*height + (y/2)*(width/2) + (x/2)] = U;
				buffer[width*height + (width/2)*(height/2) + (y/2)*(width/2) + (x/2)] = V;
			}
		}
		enc.encodeFrame(buffer);
	}
	delete [] buffer;
	enc.finish();
}


extern "C" int entrypoint() {
	doTest();
	return 0;
}


