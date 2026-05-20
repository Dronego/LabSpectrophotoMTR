# LabSpectrophotoMTR

Code de depart pour piloter un micro-spectrometre Hamamatsu C12880MA avec une carte Adafruit ESP32-S3 TFT Feather ou ESP32-S3 Reverse TFT Feather.

L'objectif du projet est de construire progressivement un outil de paillasse compact : acquisition du spectre, affichage TFT, calibration en longueur d'onde, gestion des sources lumineuses et export des donnees.

## Etat actuel

- Acquisition des 288 canaux du C12880MA.
- Sortie des donnees au format CSV sur le port serie.
- Pilotage de deux sorties lumineuses : LED blanche et laser 404 nm.
- Affectation des broches adaptee a l'ESP32-S3 TFT Feather.

## Materiel vise

- Hamamatsu C12880MA
- Adafruit ESP32-S3 TFT Feather ou ESP32-S3 Reverse TFT Feather
- Source lumineuse blanche commandee par transistor/MOSFET
- Laser 404 nm commande par transistor/MOSFET
- Adaptation analogique de la sortie video du C12880MA vers l'ADC ESP32-S3

## Fichier Arduino

Le sketch principal est ici :

`firmware/C12880MA_ESP32S3_TFT_Feather/C12880MA_ESP32S3_TFT_Feather.ino`

Dans l'IDE Arduino, ouvrir le fichier `.ino` depuis ce dossier. Le nom du dossier correspond au nom du sketch, ce qui evite les avertissements de l'IDE.

## Reglages Arduino IDE

- Carte : `Adafruit Feather ESP32-S3 TFT` ou `Adafruit Feather ESP32-S3 Reverse TFT`
- Port serie : celui expose par la carte en USB-C
- Moniteur serie : `921600 baud`

## Brochage utilise

| Fonction | Broche ESP32-S3 Feather | Remarque |
| --- | --- | --- |
| `SPEC_VIDEO` | `A5` / `GPIO8` | Entree analogique ADC1 |
| `SPEC_CLK` | `GPIO5` | Horloge vers C12880MA |
| `SPEC_ST` | `GPIO6` | Start pulse vers C12880MA |
| `SPEC_TRG` | `GPIO9` | Reserve, maintenu bas dans cette version |
| `WHITE_LED` | `GPIO11` | Commande via transistor/MOSFET |
| `LASER_404` | `GPIO12` | Commande via transistor/MOSFET |
| `GND` | `GND` | Masse commune obligatoire |

Voir aussi [le schema de cablage](docs/wiring.md) et le rendu SVG [docs/wiring.svg](docs/wiring.svg).

## Attention alimentation et niveaux

Le C12880MA est typiquement alimente en 5 V. L'ESP32-S3 fonctionne en logique 3,3 V et ses entrees ADC ne doivent jamais recevoir plus de 3,3 V.

La sortie `Video` du C12880MA peut depasser 3,3 V selon l'eclairement et le montage. Il faut donc ajouter une adaptation analogique avant `A5`, par exemple :

- un buffer/ampli-op alimente en 3,3 V avec sortie bornee,
- ou un diviseur de tension suivi d'un buffer,
- ou une carte d'interface dediee.

Les lignes `CLK` et `ST` emises par l'ESP32-S3 sont en 3,3 V. Cela doit etre valide par le montage final et la version exacte de la fiche technique utilisee.

## Format de sortie serie

Chaque ligne contient 288 valeurs ADC separees par des virgules :

```text
123,125,126,...,118
```

La resolution ADC est reglee sur 12 bits, donc les valeurs vont de `0` a `4095`.

## Prochaines etapes

- Ajouter un temps d'integration parametrable.
- Ajouter une acquisition dark/reference.
- Afficher le spectre sur TFT.
- Ajouter calibration pixel -> longueur d'onde.
- Ajouter export CSV vers USB, carte SD ou interface serie.
