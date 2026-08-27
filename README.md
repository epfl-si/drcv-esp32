# Summary

<!-- Put after theses lines, ctrl + shift + p and write "Markdown" and click to "Markdwon all ine one" extension -->
<!-- TOC -->
<!-- TOC END -->
- [Summary](#summary)
- [❓ Quel est ce projet](#-quel-est-ce-projet)
- [🤔 Comment utiliser ce projet](#-comment-utiliser-ce-projet)
  - [⚙️ Configuration](#️-configuration)
    - [🛠️ Prérequis](#️-prérequis)
    - [📀 Installation du projet](#-installation-du-projet)
    - [💿 Installation de l'IDE Arduino](#-installation-de-lide-arduino)
    - [🤖 Configuration du Board Manager (gestionnaire de cartes)](#-configuration-du-board-manager-gestionnaire-de-cartes)
    - [📚 Libraries](#-libraries)
    - [🔑 Gestion des secrets](#-gestion-des-secrets)
  - [📁 Projet sur différent écran](#-projet-sur-différent-écran)
- [📙 Annexes](#-annexes)
  - [🧩 Classes et fonctions créées pour ce projet](#-classes-et-fonctions-créées-pour-ce-projet)
  - [❓ FAQ](#-faq)
    - [Comment modifier la langue de texte ?](#comment-modifier-la-langue-de-texte-)
  - [⛓️ Liens utiles](#️-liens-utiles)


# ❓ Quel est ce projet

Le projet DRCV, autrement dit "Dsi Room Calendar View" est un mini projet ayant pour but d'afficher sur un écran E-ink les réservations actuelles et à venir de nos salles de conférence.

# 🤔 Comment utiliser ce projet

La partie configuration du projet a une partie commune avec des prérequis globaux. Cette section de la documentation concerne donc les points et choses à avoir absolument peu importe l'écran

## ⚙️ Configuration

### 🛠️ Prérequis

Pour utiliser ce projet, il vous faudra tout d'abord :
- Un des écrans cités à la section [📁 Projet sur différent écran](#-projet-sur-différent-écran)
- Arduino IDE dans la version la plus récente [💿 Installation de l'IDE Arduino](#-installation-de-lide-arduino)
- Un compte pour le programme, qui a accès aux réservations de la salle souhaitée.

### 📀 Installation du projet

Cloner le repo sur lequel vous êtes actuellement avec la commande suivate :
```bash
git clone https://github.com/epfl-si/drcv-esp32.git
```

ou

```bash
git clone git@github.com:epfl-si/drcv-esp32.git
```

> [!NOTE]
> Vous pouvez depuis le [repository Github](https://github.com/epfl-si/drcv-esp32), cliquer sur le bouton vert avec inscris **Code**. Une interface s'ouvrira vous proposant, dans l'onglet **local**, plusieurs paramètres (https, ssh et Github CLI) afin de télécharger le repository sur votre appareil.
>
> Si vous n'avez pas l'utilitaire de commande **git**, vous pouvez également, en suivant les étapes précédente, cliquer sur **Download ZIP** puis l'extraire.

### 💿 Installation de l'IDE Arduino

| Système d'exploitation (OS) | Téléchargement |
| :--: | :-- |
| Windows | Depuis le site web officiel sur [https://www.arduino.cc/en/software/](https://www.arduino.cc/en/software/). |
| MAC | Depuis le site web officiel sur [https://www.arduino.cc/en/software/](https://www.arduino.cc/en/software/). |
| Linux | Je recommande d'installer la version **Arduino IDE v2** avec `flatpak` ou d'installer l'AppImage ou le zip que l'on peut récupérer depuis le site officiel sur [https://www.arduino.cc/en/software/](https://www.arduino.cc/en/software/). Je déconseille la version `snap` n'était pas à jour et causait donc de nombreux problèmes. |

### 🤖 Configuration du Board Manager (gestionnaire de cartes)

1. Lancer Arduino
2. Dirigez-vous sur `Files > Preferencies` (en français => `Fichier > Préférences`)
3. Tout à droite du champ "URL de gestionnaire de cartes supplémentaires", cliquez sur le bouton bleu avec deux carrés
4. Ajoutez cette ligne : [https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json](https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json)
5. Ensuite, allez dans `Tools > Board > Boards Manager` (en français => `Outils > Carte > Gestionnaire de Cartes`)
6. Tapez "esp32"
7. Installer la version la plus récente du gestionnaire de cartes **esp32**, par **Espressif Systems**

### 📚 Libraries

Différentes librairies sont à installer afin que le projet puisse fonctionner. Certaines libraries sont communes et sont donc référé ici, d'autres sont exclusives à l'écran et seront donc expliqués dans la section **📚 Libraries** du README.md de leur dossier respectif.

> [!NOTE]
> S'ils n'ont pas besoin de librairies supplémentaires, la section **📚 Libraries** sera manquante au README.md.

Voici donc la liste des libraires à installer qui sont communes à tous les écrans :
- `ArduinoHttpClient` par **Arduino** (Utilisé afin d'effectuer les requêtes HTTP vers l'API SOAP EWS)
- `Base64` par **Xander Electronics** (Utilisé afin de convertir en base 64 le nom d'utilisateur et le mot de passe du compte de service, et le mettre dans les requête HTTP)

> [!NOTE]
> Certaines méthodes comme le `XMLParser` ou encore le `Split` sont handmade.
> Les différentes documentations de ceux-ci sont référé dans la section [🧩 Classes et fonctions créées pour ce projet](#-classes-et-fonctions-créées-pour-ce-projet)

### 🔑 Gestion des secrets

1. Aller dans le dossier `src`
2. Dupliquer le fichier `secrets.h.example` et renommez-le `secrets.h`
3. Ajoutez les valeurs nécessaires (nom d'utilisateurs, mot de passe, etc..)
   > [!WARNING]
   > Prennez garde à ne pas modifier les valeurs déjà existante.

## 📁 Projet sur différent écran

- [CrowPanel ESP32 4.2" E-paper](./CrowPanel/)

- [M5Paper](./M5Paper/)

- [Seeed Studio XIAO 7.5" Epaper](./SeeedStudio7.5Epaper/)

# TRMNL Terminus

## Étape 1 : Lancer l'environnement
```bash
git clone git@github.com:epfl-si/drcv-esp32.git
cd drcv-esp32
```
> N'oubliez pas de remplir les .env
```bash
docker compose up -d --build
```

## Étape 2 : Connecter l'appareil au wifi et au serveur
[How to set up a new device](https://help.trmnl.com/en/articles/9416306-how-to-set-up-a-new-device)

[Wifi troubleshooting](https://help.trmnl.com/en/articles/10193157-device-wifi-troubleshooting)

## Étape 3 : Initialiser le serveur Terminus
1. Ouvrez votre navigateur et rendez vous sur le http://localhost:2300 et créez votre compte administrateur local. 
2. Allez dans l'onglet `Devices` (Appareils) et cliquez sur le bouton "+" pour ajouter un appareil
3. Remplissez les informations pour créer un écran : 
    - **Mac Address** : Mettez celle de votre vrai ESP32 ([Trouver l'adresse MAC de son ESP32](https://help.trmnl.com/en/articles/10614205-finding-your-trmnl-mac-address))
    - **API Key** : [Créer et/ou trouver sa clé API](https://help.trmnl.com/en/articles/11195228-user-level-api-keys)
    - Pour le reste, remplissez selon vos préférences.
4. Validez pour enregistrer l'appareil.

## Étape 4 : Créer l'Extension (La source de données)
1. Allez dans l'onglet Extensions et clique sur le "+".
2. Choisi le kind sur `Poll` et le mode sur `Text`.
3. Remplis les champs suivants :
4. Dans le champs Template and Input, rentrez cela :
    ```html
    <style>
      body, html, div[class^="view"] { 
        margin: 0 !important; 
        padding: 0 !important; 
        width: 100vw !important; 
        height: 100vh !important; 
        overflow: hidden !important; 
      }
    </style>
    
    <div style="position: fixed; top: 0; left: 0; right: 0; bottom: 0; box-sizing: border-box; background-color: white; color: black; font-family: sans-serif; display: flex; overflow: hidden;">
      {% assign batt_level = source_1.batteryPercentage | default: 0 %}
    
      <div style="display: flex; align-items: center; gap: 5px; margin-top: 5px; position: absolute; top: 5px; right: 5px;">
        {% if batt_level > 70 %}
          <svg width="24" height="12" viewBox="0 0 24 12" fill="none" xmlns="http://www.w3.org/2000/svg">
            <rect x="1" y="1" width="20" height="10" rx="1" stroke="black" stroke-width="2"/>
            <path d="M23 4V8" stroke="black" stroke-width="2" stroke-linecap="round"/>
            <rect x="3" y="3" width="16" height="6" fill="black"/>
          </svg>
        {% elsif batt_level > 20 %}
          <svg width="24" height="12" viewBox="0 0 24 12" fill="none" xmlns="http://www.w3.org/2000/svg">
            <rect x="1" y="1" width="20" height="10" rx="1" stroke="black" stroke-width="2"/>
            <path d="M23 4V8" stroke="black" stroke-width="2" stroke-linecap="round"/>
            <rect x="3" y="3" width="9" height="6" fill="black"/>
          </svg>
        {% else %}
          <svg width="24" height="12" viewBox="0 0 24 12" fill="none" xmlns="http://www.w3.org/2000/svg">
            <rect x="1" y="1" width="20" height="10" rx="1" stroke="black" stroke-width="2"/>
            <path d="M23 4V8" stroke="black" stroke-width="2" stroke-linecap="round"/>
            <rect x="3" y="3" width="3" height="6" fill="black"/>
          </svg>
        {% endif %}
        <span style="font-size: 14px; font-weight: bold; font-family: sans-serif;">{{ batt_level }}%</span>
      </div>
    
      {% assign time_seconds = "now" | date: "%s" %}
      {% assign current_time = time_seconds | date: "%H:%M" %}
    
      <div style="width: 50vw; height: 100vh; border-right: 3px solid black; box-sizing: border-box; display: flex; flex-direction: column; position: relative;">
        
        <div style="padding: 15px; flex-grow: 1; box-sizing: border-box; overflow: hidden; margin-bottom: 110px;">
          {% assign main_room = source_1.rooms[0] %}
          
          <div style="width: 100%; height: 60px; background-color: black; color: white; display: flex; align-items: center; padding-left: 15px; margin-bottom: 20px; break-inside: avoid;">
            <h1 style="margin: 0; font-size: 28px; font-weight: bolder;">{{ main_room.room_name }}</h1>
          </div>
    
          {% assign is_free = true %}
          {% assign next_start = "la fin de journée" %}
          {% for cours in main_room.events %}
            {% if current_time >= cours.start and current_time < cours.end %}
              {% assign is_free = false %}
            {% endif %}
            {% if current_time < cours.start and next_start == "la fin de journée" %}
              {% assign next_start = cours.start %}
            {% endif %}
          {% endfor %}
    
          {% if is_free %}
            <div style="font-size: 22px; font-weight: bold; margin-bottom: 20px; border: 2px dashed black; padding: 10px; display: inline-block; break-inside: avoid;">
              Libre jusqu'à {{ next_start }}
            </div>
          {% endif %}
    
          {% assign has_upcoming_1 = false %}
          {% for cours in main_room.events %}
            {% if cours.end > current_time %}
              {% assign has_upcoming_1 = true %}
              <div style="margin-bottom: 12px; font-size: 22px; line-height: 1.2; padding-left: 25px; text-indent: -25px; break-inside: avoid;">
                <span style="font-weight: bold; margin-right: 5px;">&gt;</span>
                <span style="white-space: nowrap; margin-right: 8px;">{{ cours.start }}-{{ cours.end }}</span>
                <span style="word-wrap: break-word;">{{ cours.title }}</span>
              </div>
            {% endif %}
          {% endfor %}
        </div>
    
        <div style="position: absolute; bottom: 0; left: 0; border-top: solid 3px black; border-right: solid 3px black; display: flex; flex-direction: column; align-items: center; padding: 5px 10px; background-color: white; width: max-content; box-sizing: border-box; z-index: 10;">
          <div style="font-size: 20px; font-weight: bold; line-height: 1.1;">
            {{ time_seconds | date: "%d-%m-%Y" }}
          </div>
          <div style="font-size: 12px; color: #333;">
            last refresh: {{ time_seconds | date: "%H:%M:%S" }}
          </div>
        </div>
    
        <div style="position: absolute; bottom: 0; right: 0; border-top: solid 3px black; border-left: solid 3px black; display: flex; flex-direction: column; align-items: center; padding: 8px; background-color: white; width: max-content; box-sizing: border-box; z-index: 10;">
          <img src="https://api.qrserver.com/v1/create-qr-code/?size=85x85&data=https://terminus.fsd.epfl.ch/add-event?room={{ main_room.prefix_name | url_encode }}%26key={{ main_room.qr_token }}" 
               style="width: 85px; height: 85px; border: 2px solid black; display: block; margin-bottom: 4px;" />
          <div style="font-size: 10px; font-weight: bold; text-transform: uppercase; letter-spacing: 0.02em; background-color: #000; color: #fff; padding: 1px 4px; text-align: center; width: 100%; box-sizing: border-box;">
            Réserver
          </div>
        </div>
    
      </div>
    
      <div style="width: 50vw; height: 100vh; display: flex; flex-direction: column;">
    
        {% assign room2 = source_1.rooms[1] %}
        <div style="height: 50vh; border-bottom: 3px solid black; padding: 15px; box-sizing: border-box; overflow: hidden; display: block;">
          <div style="float: right; width: 0; height: calc(100% - 90px);"></div>
          <div style="float: right; clear: right; margin-right: -15px; margin-bottom: -15px; width: 90px; height: 105px; border-top: solid 3px black; border-left: solid 3px black; display: flex; flex-direction: column; align-items: center; justify-content: center; background-color: white; box-sizing: border-box; z-index: 10; position: relative;">
            <img src="https://api.qrserver.com/v1/create-qr-code/?size=85x85&data=https://terminus.fsd.epfl.ch/add-event?room={{ room2.prefix_name | url_encode }}%26key={{ room2.qr_token }}" 
                 style="width: 70px; height: 70px; border: 2px solid black; display: block; margin-bottom: 4px;" />
            <div style="font-size: 9px; font-weight: bold; text-transform: uppercase; letter-spacing: 0.02em; background-color: #000; color: #fff; padding: 1px 4px; text-align: center; width: calc(100% - 10px); box-sizing: border-box;">
            Réserver
            </div>
          </div>
    
          <div style="background-color: black; color: white; padding: 5px 15px; display: inline-block; margin-bottom: 10px; break-inside: avoid;">
            <h2 style="margin: 0; font-size: 22px; font-weight: bolder;">{{ room2.room_name }}</h2>
          </div>
    
          {% assign is_free2 = true %}
          {% assign next_start2 = "la fin de journée" %}
          {% for cours in room2.events %}
            {% if current_time >= cours.start and current_time < cours.end %}
              {% assign is_free2 = false %}
            {% endif %}
            {% if current_time < cours.start and next_start2 == "la fin de journée" %}
              {% assign next_start2 = cours.start %}
            {% endif %}
          {% endfor %}
    
          {% if is_free2 %}
            <div style="font-size: 18px; font-weight: bold; margin-bottom: 10px; border: 2px dashed black; padding: 5px; display: inline-block; break-inside: avoid;">
              Libre jusqu'à {{ next_start2 }}
            </div>
          {% endif %}
    
          {% assign has_upcoming_2 = false %}
          {% for cours in room2.events %}
            {% if cours.end > current_time %}
              {% assign has_upcoming_2 = true %}
              <div style="margin-bottom: 8px; font-size: 18px; line-height: 1.2; padding-left: 22px; text-indent: -22px; break-inside: avoid;">
                <span style="font-weight: bold; margin-right: 5px;">&gt;</span>
                <span style="white-space: nowrap; margin-right: 5px;">{{ cours.start }}-{{ cours.end }}</span>
                <span style="word-wrap: break-word;">{{ cours.title }}</span>
              </div>
            {% endif %}
          {% endfor %}
          
        </div>
    
        {% assign room3 = source_1.rooms[2] %}
        <div style="height: 50vh; padding: 15px; box-sizing: border-box; overflow: hidden; display: block;">
          <div style="float: right; width: 0; height: calc(100% - 90px);"></div>
          <div style="float: right; clear: right; margin-right: -15px; margin-bottom: -15px; width: 90px; height: 105px; border-top: solid 3px black; border-left: solid 3px black; display: flex; flex-direction: column; align-items: center; justify-content: center; background-color: white; box-sizing: border-box; z-index: 10; position: relative;">
            <img src="https://api.qrserver.com/v1/create-qr-code/?size=85x85&data=https://terminus.fsd.epfl.ch/add-event?room={{ room3.prefix_name | url_encode }}%26key={{ room3.qr_token }}" 
                 style="width: 70px; height: 70px; border: 2px solid black; display: block; margin-bottom: 4px;" />
            <div style="font-size: 9px; font-weight: bold; text-transform: uppercase; letter-spacing: 0.02em; background-color: #000; color: #fff; padding: 1px 4px; text-align: center; width: calc(100% - 10px); box-sizing: border-box;">
            Réserver
            </div>
          </div>
    
          <div style="background-color: black; color: white; padding: 5px 15px; display: inline-block; margin-bottom: 10px; break-inside: avoid;">
            <h2 style="margin: 0; font-size: 22px; font-weight: bolder;">{{ room3.room_name }}</h2>
          </div>
    
          {% assign is_free3 = true %}
          {% assign next_start3 = "la fin de journée" %}
          {% for cours in room3.events %}
            {% if current_time >= cours.start and current_time < cours.end %}
              {% assign is_free3 = false %}
            {% endif %}
            {% if current_time < cours.start and next_start3 == "la fin de journée" %}
              {% assign next_start3 = cours.start %}
            {% endif %}
          {% endfor %}
    
          {% if is_free3 %}
            <div style="font-size: 18px; font-weight: bold; margin-bottom: 10px; border: 2px dashed black; padding: 5px; display: inline-block; break-inside: avoid;">
              Libre jusqu'à {{ next_start3 }}
            </div>
          {% endif %}
    
          {% assign has_upcoming_3 = false %}
          {% for cours in room3.events %}
            {% if cours.end > current_time %}
              {% assign has_upcoming_3 = true %}
              <div style="margin-bottom: 8px; font-size: 18px; line-height: 1.2; padding-left: 22px; text-indent: -22px; break-inside: avoid;">
                <span style="font-weight: bold; margin-right: 5px;">&gt;</span>
                <span style="white-space: nowrap; margin-right: 5px;">{{ cours.start }}-{{ cours.end }}</span>
                <span style="word-wrap: break-word;">{{ cours.title }}</span>
              </div>
            {% endif %}
          {% endfor %}
          
        </div>
    
      </div>
    
    </div>
    ```
5. Cliquez sur `Save`
6. Revennez ensuite sur l'extension que vous venez de créer. Puis cliquer sur `Exchanges`.
7. Cliquer sur "+"
8. Dans le champ `Template`, entrez y cette URL (deviceName étant le Label du device créé précédemment): 
    ```
    http://ews-api:5000/api/screen?deviceName=TRMNL1&roomList=inn011,INN033,inn041 
    ``` 
9. Cliquez sur `Save`
10. Retournez sur votre Extension et cliquez sur `Build`
11. Allez sur l'onglet `Screens` et vérifiez qu'un écran, avec le nom de votre extension, se soit bien créé.

## Étape 5 : Lier l'extension à l'appareil
1. Allez dans l'onglet `Playlist` et cliquez sur le bouton "+"
2. Donnez-lui un nom (ex: Playlist Salles) et ajoutez votre extension personnalisée à l'intérieur.
3. Retournez dans l'onglet `Devices`, modifiez votre appareil précédemment créé et associez-le à votre nouvelle playlist.


# 📙 Annexes

## 🧩 Classes et fonctions créées pour ce projet

- Fonction [Split](./src/split/)

- Classe [DateTime](./src/datetime/)

- Classe [Event](./src/event/)

- Function [XML_Parser](./src/xml_parser/)

## ❓ FAQ

### Comment modifier la langue de texte ?

La prise en charge du multi langue n'est pas disponible, cependant, l'intégralité des textes affichés sur l'écran (hors valeurs décimales), sont editables dans le fichier `variables.h`

## ⛓️ Liens utiles

- [Convertisseur d'image en image arduino (Bitmap)](https://javl.github.io/image2cpp/)
