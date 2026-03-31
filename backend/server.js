const express = require('express');
const cors = require('cors');
const fs = require('fs');
const { exec } = require('child_process');
const path = require('path');

const app = express();
app.use(cors());

const PORT = 3000;
const SKETCH_PATH = '/app/CrowPanel';
const BUILD_DIR = path.join(SKETCH_PATH, 'build', 'esp32.esp32.esp32s3');

app.get('/bootloader', (req, res) => {
  const file = path.join(BUILD_DIR, 'CrowPanel.ino.bootloader.bin');
  if (fs.existsSync(file)) res.sendFile(file);
  else res.status(404).send("Bootloader introuvable dans " + BUILD_DIR);
});

app.get('/partitions', (req, res) => {
  const file = path.join(BUILD_DIR, 'CrowPanel.ino.partitions.bin');
  if (fs.existsSync(file)) res.sendFile(file);
  else res.status(404).send("Partitions introuvables");
});

app.get('/boot_app', (req, res) => {
  const file = path.join(BUILD_DIR, 'boot_app0.bin');
  if (fs.existsSync(file)) res.sendFile(file);
  else res.status(404).send("boot_app0 introuvable");
});

app.get('/build', (req, res) => {
  const { room } = req.query;

  const configContent = `#pragma once
  #define ROOM_NAME "${room}"
  `;

  fs.writeFileSync(path.join(SKETCH_PATH, '../src/config.h'), configContent);

  const fqbn = 'esp32:esp32:esp32s3';
  const cmd = `arduino-cli compile --fqbn ${fqbn} ${SKETCH_PATH} --output-dir ${path.join(SKETCH_PATH, 'build')}`;

  console.log("Compilation en cours pour : " + room);

  exec(cmd, { maxBuffer: 1024 * 1024 * 50 }, (error, stdout, stderr) => {
    if (error) {
      console.error("Erreur compilation :", stderr);
      return res.status(500).send(stderr);
    }

    const binPath = path.join(BUILD_DIR, 'CrowPanel.ino.bin');
    if (fs.existsSync(binPath)) {
      console.log("Compilation réussie, envoi du binaire.");
      res.setHeader('Content-Type', 'application/octet-stream');
      res.sendFile(binPath);
    } else {
      res.status(500).send("Fichier binaire non trouvé après compilation.");
    }
  });
});

app.listen(PORT, '0.0.0.0', () => {
  console.log(`Backend prêt sur le port ${PORT}`);
  console.log(`Dossier de build surveillé : ${BUILD_DIR}`);
});
