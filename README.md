# Summary

- [Summary](#summary)
- [What is this project](#what-is-this-project)
- [How to use this project](#how-to-use-this-project)
  - [Configuration](#configuration)
    - [Prerequisites](#prerequisites)
    - [Project Installation](#project-installation)
    - [Arduino IDE Installation](#arduino-ide-installation)
    - [Board Manager Configuration](#board-manager-configuration)
    - [Libraries](#libraries)
    - [Secrets Management](#secrets-management)
  - [Project on different screens](#project-on-different-screens)
- [Appendices](#appendices)
  - [Classes and functions created for this project](#classes-and-functions-created-for-this-project)
  - [FAQ](#faq)
    - [How to change the text language?](#how-to-change-the-text-language)
  - [Useful links](#useful-links)


# What is this project

The DRCV project, also known as "Dsi Room Calendar View", is a mini-project aimed at displaying current and upcoming conference room reservations on an E-ink screen.

# How to use this project

The configuration part of the project has a common section with global prerequisites. This section of the documentation therefore concerns the points and things to have absolutely regardless of the screen used.

## Configuration

### Prerequisites

To use this project, you will first need:
- One of the screens mentioned in the section [Project on different screens](#project-on-different-screens)
- The most recent version of Arduino IDE [Arduino IDE Installation](#arduino-ide-installation)
- An account for the program that has access to the reservations of the desired room.

### Project Installation

Clone the repository you are currently on with the following command:
```bash
git clone https://github.com/epfl-si/drcv-esp32.git
```

or

```bash
git clone git@github.com:epfl-si/drcv-esp32.git
```

> [!NOTE]
> From the [Github repository](https://github.com/epfl-si/drcv-esp32), you can click on the green button labeled **Code**. An interface will open offering several parameters in the **local** tab (https, ssh, and Github CLI) to download the repository to your device.
>
> If you do not have the **git** command-line utility, you can also click on **Download ZIP** and then extract it.

### Environment Variable
You will find a `.env.example` file in the `./frontend` folder. Rename it to `.env` and enter the backend URL.

### Secrets
You will find a `.secrets.h.example` file in the `./src` folder. Rename it to `.secrets.h` and enter the informations.

> The informations are available in keybase at the path `epfl_sopec/drcv-esp32`.

### Docker Container
While positioned at the root of the `drcv-esp32` project, run the command `docker compose up -d`. This may take a few minutes.

Once the containers are created, go to the address `http://localhost:8080`.

> Note that you must open the page in a Chromium-based browser (Brave, Chrome, Edge, Opera, etc.) for the program to function correctly.

### Arduino IDE Installation

| Operating System (OS) | Download |
| :--: | :-- |
| Windows | From the official website at [https://www.arduino.cc/en/software/](https://www.arduino.cc/en/software/). |
| MAC | From the official website at [https://www.arduino.cc/en/software/](https://www.arduino.cc/en/software/). |
| Linux | I recommend installing the **Arduino IDE v2** version with `flatpak` or installing the AppImage or zip file available from the official website at [https://www.arduino.cc/en/software/](https://www.arduino.cc/en/software/). I advise against the `snap` version as it was not up to date and caused many problems. |

### Board Manager Configuration

1. Launch Arduino
2. Go to `Files > Preferences`
3. On the far right of the "Additional Boards Manager URLs" field, click the blue button with two squares
4. Add this line: [https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json](https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json)
5. Then, go to `Tools > Board > Boards Manager`
6. Type "esp32"
7. Install the most recent version of the **esp32** board manager by **Espressif Systems**

### Libraries

Various libraries need to be installed for the project to work. Some libraries are common and are referred to here; others are screen-specific and will be explained in the **Libraries** section of the README.md in their respective folders.

> [!NOTE]
> If they do not require additional libraries, the **Libraries** section will be missing from their README.md.

Here is the list of common libraries to install for all screens:
- `ArduinoHttpClient` by **Arduino** (Used to perform HTTP requests to the EWS SOAP API)
- `Base64` by **Xander Electronics** (Used to convert the service account username and password to base64 and include them in HTTP requests)

> [!NOTE]
> Some methods like the `XMLParser` or the `Split` are handmade.
> Documentation for these is referred to in the section [Classes and functions created for this project](#classes-and-functions-created-for-this-project)

### Secrets Management

1. Go to the `src` folder
2. Duplicate the `secrets.h.example` file and rename it `secrets.h`
3. Add the necessary values (usernames, password, etc.)
   > [!WARNING]
   > Be careful not to modify existing values.

## Project on different screens

- [CrowPanel ESP32 4.2" E-paper](./CrowPanel/)

- [M5Paper](./M5Paper/)

- [Seeed Studio XIAO 7.5" Epaper](./SeeedStudio7.5Epaper/)

# Appendices

## Classes and functions created for this project

- Function [Split](./src/split/)

- Class [DateTime](./src/datetime/)

- Class [Event](./src/event/)

- Function [XML_Parser](./src/xml_parser/)

## FAQ

### How to change the text language?

Multi-language support is not available; however, all text displayed on the screen (excluding decimal values) is editable in the `variables.h` file.

## Useful links

- [Arduino image converter (Bitmap)](https://javl.github.io/image2cpp/)
