#ifndef ESRGAN_H
#define ESRGAN_H

#include "vendor/onnxruntime/include/onnxruntime_cxx_api.h"
#include <string>

class esrgan
{
private:
    Ort::Env m_env;
    Ort::RunOptions m_opts;
    Ort::Session m_session;

public:
    esrgan();

    void upscale(uint8_t *out, const uint8_t *data, int width, int height, int stride, bool x16);
private:
    void run(float *out, float *in, int width, int height);
    void hwc2chw(float *out, const uint8_t *in, int width, int height, int stride);
    void chw2hwc(uint8_t *out, const float *in, int width, int height);
};

#endif // ESRGAN_H
