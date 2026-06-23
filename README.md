# ESP32 PWM Thermal Governor for P40

This project implements a thermal governor for the NVIDIA Tesla P40 GPU using an ESP32 to control PWM fans based on temperature readings.

## Descrizione
Il progetto realizza un sistema di controllo termico chiuso per la GPU Tesla P40. Un ESP32 legge la temperatura tramite un sensore DS18B20 posto in contatto termico con il dissipatore della GPU e regola la velocità della ventola tramite segnale PWM pilotato da un MOSFET logico. L'obiettivo è mantenere la GPU entro un range di temperatura ottimale riducendo rumore e consumo quando non necessario.

## Architettura
- **ESP32** (es. ESP32-WROOM-32) come unità di controllo principale
- **Sensore di temperatura** DS18B20 (versione waterproof consigliata)
- **Ventola** stock della Tesla P40 (12V, ingresso PWM)
- **MOSFET** N-channel logico (es. IRLZ44N) con diodo flyback e resistenza di gate
- **Alimentazione** 12V DC per la ventola, 3.3V per ESP32 e sensore (ottenibili da un unico alimentatore con buck converter o separati con massa comune)
- **Firmware** Arduino/ESPHome che implementa lettura temperatura, controllo PID o soglie, e output PWM

## Installazione
1. Clonare questo repository
2. Collegare l'hardware secondo lo schema in `docs/hardware_schematic.md`
3. Caricare il firmware sull'ESP32 usando l'Arduino IDE o PlatformIO (file in `src/`)
4. (Opzionale) Configurare ESPHome per integrazione con Home Assistant (vedi `src/esp32_pwm_governor.yaml`)
5. Assicurarsi che le masse siano comuni tra alimentatore, ESP32, MOSFET e ventola
6. Montare il sensore DS18B20 in contatto termico con il dissipatore della P40 usando pasta termica o adesivo

## Uso
- Alimentare il circuito: la ventola partirà secondo il profilo definito nel firmware
- Il firmware legge la temperatura ogni secondo e aggiorna il duty cycle PWM per mantenere la temperatura entro i limiti configurabili
- I parametri di soglia (temperatura minima, massima, velocità minima/massima) possono essere modificati nel file sorgente e ricaricati sull'ESP32
- Se abilitato, l'ESPHome espone i sensori e lo switch su MQTT/Home Assistant per monitoraggio e controllo remoto

## Esempi
- Firmware base: `src/esp32_pwm_governor.ino`
- Configurazione ESPHome: `src/esp32_pwm_governor.yaml`
- Schema elettrico: `docs/hardware_schematic.md`

## Stato
✅ COMPLETATO — 2026-06-14
- Struttura progetto inizializzata
- Schema hardware definito
- Firmware Arduino base sviluppato
- Integrazione ESPHome configurata
- Documentazione completa

## Licenza
Questo progetto è open source e disponibile per uso pubblico.