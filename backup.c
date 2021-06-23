#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

struct __attribute__((__packed__)) wav_header_t {
    char chunk_id[4];
    uint32_t chunk_size;
    char format[4];
    char subchunk1_id[4];
    uint32_t subchunk1_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    char subchunk2_id[4];
    uint32_t subchunk2_size;
};

int playWAVE(char *path){


    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
    	return 1;
    }

    fseek(fp, 0L, SEEK_END);
    size_t sz = ftell(fp);
    rewind(fp);

    struct wav_header_t *hdr = malloc(sz);
    fread(hdr, sz, 1, fp);
    fclose(fp);

    printf("chunk_id = \'%.*s\'\n", 4, hdr->chunk_id);
    printf("chunk_size = %u\n", hdr->chunk_size);
    printf("format = \'%.*s\'\n", 4, hdr->format);
    printf("subchunk1_id = \'%.*s\'\n", 4, hdr->subchunk1_id);
    printf("subchunk1_size = %u\n", hdr->subchunk1_size);
    printf("audio_format = %u\n", hdr->audio_format);
    printf("num_channels = %u\n", hdr->num_channels);
    printf("sample_rate = %u\n", hdr->sample_rate);
    printf("byte_rate = %u\n", hdr->byte_rate);
    printf("block_align = %u\n", hdr->block_align);
    printf("bits_per_sample = %u\n", hdr->bits_per_sample);
    printf("subchunk2_id = \'%.*s\'\n", 4, hdr->subchunk2_id);
    printf("subchunk2_size = %u\n", hdr->subchunk2_size);

    ALCdevice *dev = alcOpenDevice(NULL);
    ALCcontext *ctx = alcCreateContext(dev, NULL);
    alcMakeContextCurrent(ctx);

    ALenum fmt;
    fmt = hdr->num_channels == 1
              ? (hdr->bits_per_sample == 8 ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16)
              : (hdr->bits_per_sample == 8 ? AL_FORMAT_STEREO8
                                          : AL_FORMAT_STEREO16);

    unsigned buf, source;
    alGenBuffers(1, &buf);
    alBufferData(buf, fmt, ((uint8_t *)hdr) + sizeof(struct wav_header_t),
                hdr->subchunk2_size, hdr->sample_rate);

    ALfloat ori[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};
    alListener3f(AL_POSITION, 0.0f, 0.0f, 1.0f);
    alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alListenerfv(AL_ORIENTATION, ori);

    alGenSources(1, &source);
    alSourcef(source, AL_PITCH, 1);
    alSourcef(source, AL_GAIN, 1);
    alSource3f(source, AL_POSITION, 0, 0, 0);
    alSource3f(source, AL_VELOCITY, 0, 0, 0);
    alSourcei(source, AL_LOOPING, AL_FALSE);
    alSourcei(source, AL_BUFFER, buf);

    alSourcePlay(source);

    int state;
    int i = 0;
    do {
      	alGetSourcei(source, AL_SOURCE_STATE, &state);
    } while (state == AL_PLAYING);

    free(hdr);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
    	return 1;
    }

    const char *check = argv[1];
    uint8_t len = strlen(check);
    const char *lastFour = &check[len-4];


    if(!strncmp(lastFour, ".wav", 4)){
        playWAVE(argv[1]);
        return 0;
    } else {
        return 1;
    }
}