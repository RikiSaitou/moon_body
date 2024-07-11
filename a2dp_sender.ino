/*
  Streaming of sound data with Bluetooth to other Bluetooth device.
  We generate 2 tones which will be sent to the 2 channels.
  
  Copyright (C) 2020 Phil Schatzmann
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "BluetoothA2DPSource.h"
#include <math.h>
#include <M5Stack.h>
#include "driver/adc.h"
#include <driver/i2s.h>

#define MIC1 36
#define MIC2 35

#define NUM_READ_LEN 1


BluetoothA2DPSource a2dp_source;

void setupADC() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
    .sample_rate =  44100/2,              // The format of the signal using ADC_BUILT_IN
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // is fixed at 12bit, stereo, MSB
    .channel_format = I2S_CHANNEL_FMT_ALL_LEFT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB), 
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 16, 
    .dma_buf_len = 64, 
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };

  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_11db);
  adc1_config_width(ADC_WIDTH_12Bit);

  esp_err_t erReturns;
  erReturns = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  erReturns = i2s_set_adc_mode(ADC_UNIT_1, ADC1_CHANNEL_0);
  i2s_adc_enable(I2S_NUM_0);
}

// The supported audio codec in ESP32 A2DP is SBC. SBC audio stream is encoded
// from PCM data normally formatted as 44.1kHz sampling rate, two-channel 16-bit sample data

int sampling_rate = 44100;
int rate = 44100 / sampling_rate;
int32_t get_data_frames(Frame *frame, int32_t frame_count) {
    
    uint16_t u16Buf[frame_count];
    size_t uiGotLen=0;
    i2s_read(I2S_NUM_0, u16Buf, frame_count*sizeof(uint16_t), &uiGotLen, portMAX_DELAY);

    for (int i = 0; i < frame_count; i++)
    {

      float amp = ((float)u16Buf[i] - 4095.0 / 2.0) * 8.0; // 45μsくらい 4095 は 12bit
      // Serial.println(u16Buf[i]);

      frame[i].channel1 = amp;
      frame[i].channel2 = amp;

    }
    // delay(1);
    
    return frame_count;
}


void setup() {
  M5.begin();
  M5.Speaker.write(0); // スピーカーをオフする
  Serial.begin(9600);
  M5.lcd.setBrightness(0); // LCDバックライトの輝度を下げる

  setupADC();

  pinMode(MIC1, INPUT);
  pinMode(MIC2, INPUT);

  a2dp_source.set_auto_reconnect(false);
  a2dp_source.start("X6S", get_data_frames);  
  a2dp_source.set_volume(100);
}

void loop() {
  // to prevent watchdog in release > 1.0.6
  // delay(1000);
}
