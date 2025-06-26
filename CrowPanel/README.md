# Summary

<!-- Put after theses lines, ctrl + shift + p and write "Markdown" and click to "Markdwon all ine one" extension -->
<!-- TOC -->
<!-- TOC END -->
- [Summary](#summary)
- [🤔 Comment utiliser ce projet](#-comment-utiliser-ce-projet)
  - [🪪 Référence](#-référence)
  - [🛠️ Prérequis](#️-prérequis)
  - [⚙️ Configuration](#️-configuration)
    - [🤖 Board Manager](#-board-manager)
    - [🔌 Board and Port](#-board-and-port)
    - [📚 Libraries](#-libraries)

# 🤔 Comment utiliser ce projet

## 🪪 Référence

[Elecrow CrowPanel E-Paper 4.2"](https://www.berrybase.ch/fr/elecrow-crowpanel-e-paper-hmi-display-4.2-zoll-400x300-aufloesung-esp32-s3-spi-schwarz-weiss)

## 🛠️ Prérequis

Afin d'effectuer ce projet, plusieurs choses sont indispensables :

- Écran e-ink ou autre type d'écran (ici e-ink)
- ESP32 ou autre microcontrôleur pouvant être connecté à un réseau et donc effectuer des requêtes HTTP (ici, utilisation d'un ESP32 intégré à l'écran e-ink [Elecrow CrowPanel E-Paper 4.2"](https://www.berrybase.ch/fr/elecrow-crowpanel-e-paper-hmi-display-4.2-zoll-400x300-aufloesung-esp32-s3-spi-schwarz-weiss))
- API pour récupérer les données à afficher (ici, utilisation de l'API SOAP d'EWS)

## ⚙️ Configuration

> [!NOTE]
> Les fichiers du projets sont `drcv-esp32.ino` et `secrets.h.example`. Les autres fichiers sont les fichiers de librairies.

> [!WARNING]
> Pensez impérativement avant d'utiliser ce code, à modifier le nom du fichier `secrets.h.example` en `secrets.h`, ainsi que de remplir les champs dans le fichier.

### 🤖 Board Manager

Files > Preferences

Additional Boards Manager URLs: https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

Tools > Board > Boards Manager...

Recher `esp32` créé par **Espressif Systems** et installe-le

>[!NOTE]
>Pour ma part, seulement la version 3.0.7 ou les antérieures fonctionnent correctement, et non les plus récente.

### 🔌 Board and Port

| Type |  Name | Manipulation de changement |
| :--: | :--: | :--: |
| Board | ESP32S3 Dev Module | Tools > Board > ESP32 Arduino > ESP32S3 Dev Module |
| Port | /dev/ttyUSB0 | Tools > Port > /dev/ttyUSB0 |

### 📚 Libraries

> [!NOTE]
> Certaines méthodes comme le `XMLParser` ou encore le `Split` sont handmade.
> Les différentes documentations de ceux-ci sont référé [ici](../README.md#🧩-functions-créées-pour-ce-projet)

| Name |  Creator | Version |
| :--: | :--: | :--: |
| ArduinoHttpClient | Arduino | 0.6.1 |
| Wifi101 | Arduino | 0.16.1 |
| Base64 | Arturo Guadalupi | 1.0.2 |