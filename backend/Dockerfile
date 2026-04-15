FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    curl \
    unzip \
    openjdk-11-jre \
    build-essential \
    ca-certificates \
    gnupg \
    lsb-release

RUN curl -fsSL https://deb.nodesource.com/setup_18.x | bash - \
 && apt-get install -y nodejs

RUN curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh

ENV PATH="/root/.arduino15/bin:${PATH}"

RUN arduino-cli core update-index \
 && arduino-cli core install esp32:esp32

RUN arduino-cli lib install "Adafruit GFX Library"
RUN arduino-cli lib install "GxEPD2"
RUN arduino-cli lib install "Base64"
RUN arduino-cli lib install "ArduinoHttpClient"

RUN rm -rf /root/.cache/arduino

WORKDIR /app

COPY backend/ /app
COPY CrowPanel/ /app/CrowPanel/
COPY src/ /app/src/

RUN npm install express cors

EXPOSE 3000

CMD ["node", "server.js"]
