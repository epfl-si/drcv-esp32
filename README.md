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