#include "esrgan.h"

esrgan::esrgan()
    : m_session(m_env, L"RealESRGAN_x4plus_anime_6B.onnx", Ort::SessionOptions())
{
}

void esrgan::upscale(uint8_t *out, const uint8_t *data, int width, int height, int stride, bool x16)
{
    // Maximum buffer sizes for x16
    const int buf1_size = width * 4 * height * 4 * 3;
    const int buf2_size = width * 16 * height * 16 * 3;

    float *buf[2] = {
        new float[buf1_size],
        new float[buf2_size]
    };

    hwc2chw(buf[1], data, width, height, stride);
    run(buf[0], buf[1], width, height);

    if (x16) {
        width *= 4;
        height *= 4;

        run(buf[1], buf[0], width, height);
        chw2hwc(out, buf[1], width*4, height*4);
    } else {
        chw2hwc(out, buf[0], width*4, height*4);
    }

    delete[] buf[0];
    delete[] buf[1];
}


void esrgan::run(float *out, float *in, int width, int height)
{
    const int input_size = width * height * 3;
    const int output_width = width * 4;
    const int output_height = height * 4;
    const int output_size = output_width * output_height * 3;

    const int64_t input_shape[4] = { 1, 3, width, height };
    const int64_t output_shape[4] = { 1, 3, output_width, output_height };

    auto memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
    auto input_tensor = Ort::Value::CreateTensor<float>(memory_info, in, input_size, input_shape, 4);
    auto output_tensor = Ort::Value::CreateTensor<float>(memory_info, out, output_size, output_shape, 4);

    const char *input_name = "input";
    const char *output_name = "output";
    m_session.Run(m_opts, &input_name, &input_tensor, 1, &output_name, &output_tensor, 1);

    input_tensor.release();
    output_tensor.release();
    memory_info.release();
}

void esrgan::hwc2chw(float *out, const uint8_t *in, int width, int height, int stride)
{
    const int plane_size = width*height;
    for (int y = 0; y < height; y++) {
        const uint8_t* p = in + y*stride;
        for (int x = 0; x < width; x++) {
            out[plane_size*0 + y*width + x] = (float)(p[x*3 + 0] / 255.0f);
            out[plane_size*1 + y*width + x] = (float)(p[x*3 + 1] / 255.0f);
            out[plane_size*2 + y*width + x] = (float)(p[x*3 + 2] / 255.0f);
        }
    }
}

void esrgan::chw2hwc(uint8_t *out, const float *in, int width, int height)
{
    const int plane_size = width*height;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint8_t r = (uint8_t)(in[plane_size*0 + y*width + x] * 255.0f);
            uint8_t g = (uint8_t)(in[plane_size*1 + y*width + x] * 255.0f);
            uint8_t b = (uint8_t)(in[plane_size*2 + y*width + x] * 255.0f);

            *out++ = r;
            *out++ = g;
            *out++ = b;
        }
    }
}
