# Summary

<!-- Put after theses lines, ctrl + shift + p and write "Markdown" and click to "Markdwon all ine one" extension -->
<!-- TOC -->
<!-- TOC END -->
- [Summary](#summary)
- [🤔 Comment utiliser ce projet](#-comment-utiliser-ce-projet)
  - [🪪 Référence Produit](#-référence-produit)
  - [🛠️ Prérequis](#️-prérequis)
  - [⚙️ Configuration](#️-configuration)
    - [🔌 Board and Port](#-board-and-port)
  - [🎮 Utilisation](#-utilisation)
- [📙 Annexes](#-annexes)
  - [❓ FAQ](#-faq)
  - [⛓️ Liens utiles](#️-liens-utiles)

# 🤔 Comment utiliser ce projet

## 🪪 Référence Produit

[Elecrow CrowPanel E-Paper 4.2"](https://www.berrybase.ch/fr/elecrow-crowpanel-e-paper-hmi-display-4.2-zoll-400x300-aufloesung-esp32-s3-spi-schwarz-weiss)

## 🛠️ Prérequis

Afin d'effectuer ce projet, plusieurs choses sont indispensables :

- [Elecrow CrowPanel E-Paper 4.2"](https://www.berrybase.ch/fr/elecrow-crowpanel-e-paper-hmi-display-4.2-zoll-400x300-aufloesung-esp32-s3-spi-schwarz-weiss)
- [La configuration principale](../README.md#️-configuration)

## ⚙️ Configuration

Les fichiers du projet sont `CrowPanel.ino` et `pic_scenario.h`. Les autres fichiers sont les fichiers de librairies ou de documentations.

### 🔌 Board and Port

| Type |  Name | Manipulation de changement |
| :-- | :-- | :-- |
| Board | ESP32S3 Dev Module | Tools > Board > ESP32 > ESP32S3 Dev Module |
| Port | /dev/ttyUSB0 | Tools > Port > /dev/ttyUSB0 |

## 🎮 Utilisation

Afin d'utiliser le programme, voici les différentes étapes à suivre :

1. Veillez à avoir réalisé les étapes de configuration que l'on retrouve dans [cette documentation > ⚙️ Configuration](#️-configuration) ainsi que dans la [documentation principale > ⚙️ Configuration](../README.md#-Configuration)
2. Brancher votre écran avec un cable de transfert (ici c'est un type c)
3. Lancer le fichier `CrowPanel.ino` depuis Arduino
<!-- 4. Si vous avez bien séléctionné le **Board** ainsi que le **Port** comme l'étape [🔌 Board and Port](#-board-and-port) le suggère, alors cliquez sur la flèche en haut à gauche avec le label **upload** -->
4. Cliquez sur la flèche en haut à gauche avec le label **upload**
   > [!WARNING]
   > Vous devez avoir séléctionné le **Board** ainsi que le **Port** comme l'étape [🔌 Board and Port](#-board-and-port) l'indique, avant de procéder à cette étape
5. Quand l'upload est terminé, vous pouvez débrancher votre appareil de votre ordniateur, et voilà!


# 📙 Annexes

## ❓ FAQ

## ⛓️ Liens utiles