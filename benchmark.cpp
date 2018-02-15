#include <benchmark/benchmark.h>
#include <proxygen/lib/utils/Base64.h>

#include "fb64d.h"

static const char input[] = "SfmEYZdlmeBgvgrQBPKfhoEP5Kl7LJGeJBD6a0v5ZGQYyfdfLqVj2nIydWO7o8Rk185NkpeaLWouQrEQPgrttrin8/aiiKAd3XUZDIUeewoWvLULm5kw707dfUdGOvpb2gAvIFD+9LLUYyNptZyKD/2UdgV+nqsZYahUscqQc7lsnDES+2xT042rPpt4h9c2nHeGleU6oivg0D9y2vWAxz7VEHwchesj8ddqAuKCVNamw5hY2qCJMks5nQKYG1iBtg/dgOO6D3tIhT9ctRctWWimGFczh5rDPXS0k4PWKtFeciwdesyxqLYOAFNkCc6P9ebtI0fvfvSzoUEZ7VioTI4PKtk2mtEmgZY09HXW7KbmOIyDEsLFgb1GxLmCToqpKfuxIE8Rtnw0zjevpzAc3uTAJowuQbzVC0+9w6QDxbN7eurXRGDpFZX5izt9v653lVSUXRELU615TtOc56USqiEmW3Euir/vvyhJm6V2WbTKCEOUslIW5A7u44CV+4Vtb1I7kzOoSQEX0F1E7NWONDkWleV8bSg9sUWfEK7dcSu8GuQBxPkfev+d60AfZRXNqIqFOUkxSCaQxn3zoH0qIyoGcoOmzPu/621zPZ60bb/sHu0gIRACfVQmOnLlvYXvekl++NYm+16/IXmLfvYjhCpR+BPrIRzuNtswAVotwWjDsmmgmaLnPwUV6djFORD0W30j5KV/fR8RPnun+Se3+TvJcrzms5ZWYjxHq/ccNJ+WAeP6Yhn0C6aIDmEriptnZ6f2KETdtHuU5f5LadnZGw0KR9QdxkHnBbf5bKjrKtts/p0N6E4/BSd0u/xPz8E16JMbX+bkuAkU6puf0y/5e1F8wK+NM7VZr5ShIOpsHcvSXlXuPVm3uEjx263zZvq3BV90JApjSvxtJRKFw7fMCoVVLxgc/P6KWlxC1xlwL5dZbkeAU/mWcPlpFwpraXp1Lt/XoxAHPoplI5VG/SwDUbR57M9Nh26qsSM5CWiPCQTfy1879Rv2POhQ9vVmarsmFW+BItHZT3J8kTH06q/UrOTstGpTc1NehyrS0OHMJxhplMGSUnX7pe+BnsPvZv889MBLKISGphRu6G6vK0FHGAR3talr+yvJWYDuLuKQTDug5pkGMf4zi0MQM1+XUOSpPrF5FGAG3950SgTHNlOlGyRVF0ykpugsxtKwnouAnkwDORSjUryLuP+0H2kNEnffbuZlo6uC8Y0lbFepeZ6PHUST5gmlthP/LBVaHni7y1WWG8cFw+w8ld2DEtQw/UNWF/JCDP1/8D4vO7iqs8+DBeMxkyqu7IhPa22WhbEo8cior9fcV6yhBzHWh2qLrblhBrvo1T5rEvab0HswC/RKqQ==";
static const size_t input_len = sizeof(input) - 1;

static void BM_Decode(benchmark::State& state) {
    fb64d_init();

    uint8_t decoded[sizeof(input)-1]; // bigger than necessary but OK.
    for (auto _: state) {
        fb64d_decode(input, input_len, decoded);
    }
}

BENCHMARK(BM_Decode);

static void BM_ProxygenOpenSSLDecode(benchmark::State& state) {
    std::string in(input, input_len), out;
    out.reserve(in.size());

    for (auto _: state) {
        out = proxygen::Base64::urlDecode(in);
    }
}
BENCHMARK(BM_ProxygenOpenSSLDecode);

BENCHMARK_MAIN()
