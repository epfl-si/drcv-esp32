# Quel est ce projet ?

Le projet DRCV, autrement dit "Dsi Room Calendar View" est un mini projet ayant pour but d'afficher sur un écran Eink (ref de l'écran - [Elecrow CrowPanel E-Paper 4.2"](https://www.berrybase.ch/fr/elecrow-crowpanel-e-paper-hmi-display-4.2-zoll-400x300-aufloesung-esp32-s3-spi-schwarz-weiss)) les réservations actuelles et à venir de nos salles de conférence.

# Comment utiliser ce projet ?

## Prérequis

Afin d'effectuer ce projet, plusieurs choses sont indispensables :

- Écran e-ink ou autre type d'écran (ici e-ink)
- ESP32 ou autre microcontrôleur pouvant être connecté à un réseau et donceffectuer des requêtes HTTP (ici, utilisation d'un ESP32 intégré à l'écran e-ink [Elecrow CrowPanel E-Paper 4.2"](https://www.berrybase.ch/fr/elecrow-crowpanel-e-paper-hmi-display-4.2-zoll-400x300-aufloesung-esp32-s3-spi-schwarz-weiss))
- API pour récupérer les données à afficher (ici, utilisation de l'API SOAP d'EWS)

## Configuration

> [!NOTE] Fichiers du projet
> Les fichiers du projets sont `drcv-esp32.ino` et `secrets.h.example`. Les autres fichiers sont les fichiers de librairies.

> [!WARNING] Noms
> Pensez impérativement avant d'utiliser ce code, à modifier le nom du fichier `secrets.h.example` en `secrets.h`, ainsi que de remplir les champs dans le fichier.

### Board and Port :

| Type |  Name |
| :--: | :--: |
| Board | ESP32S3 Dev Module |
| Port | /dev/ttyUSB0 |

### Libraries :

> [!NOTE] Librairies du projet
> Certaines méthodes comme le `XMLParser` ou encore le `Split` sont handmade.

| Name |  Creator | Version |
| :--: | :--: | :--: |
| ArduinoHttpClient | Arduino | 0.6.1 |
| Wifi101 | Arduino | 0.16.1 |
| Base64 | Arturo Guadalupi | 1.0.2 |

# Liens utiles

- [Convertisseur d'image en image arduino](https://javl.github.io/image2cpp/)