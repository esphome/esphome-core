FROM python:2.7
MAINTAINER Otto Winter <contact@otto-winter.com>

RUN pip install --no-cache-dir platformio && \
    platformio settings set enable_telemetry No

COPY docker/platformio.ini /
RUN platformio run -e espressif32 -e espressif8266; exit 0
