# Double Mansi – Vocal Doubler (VST3)

Macht aus einer Gesangsspur zwei zusätzliche „Takes“ (links/rechts) für einen vollen Vocal-Stack.

## Regler

| Regler | Was er macht | Startwert |
|---|---|---|
| TIMING | Wie weit die Doubles hinter dem Original liegen (8–45 ms). Wenig = tight, viel = lockerer | 22 ms |
| DETUNE | Feste Verstimmung der Doubles (eins höher, eins tiefer) | 9 ct |
| VARIATION | Wie „menschlich“ Timing, Tonhöhe und Lautstärke schwanken | 50 % |
| BREITE | Wie weit die Doubles nach links/rechts gehen | 80 % |
| TON | Höhen der Doubles. Weniger = die Doubles sitzen hinter dem Lead | 9000 Hz |
| DOUBLES | Lautstärke der Doubles | 60 % |
| NUR DOUBLES | Original stummschalten, z. B. für eine eigene Double-Spur | aus |

Doppelklick auf einen Regler setzt ihn zurück.

---

## Weg A: Ohne Programmierumgebung (GitHub baut es für dich)

1. Kostenloses Konto auf github.com anlegen.
2. Neues Repository erstellen (z. B. `double-mansi`), dann „uploading an existing file“ wählen.
3. Den **Inhalt** des Ordners `DoubleMansi` hochladen – inklusive des versteckten Ordners `.github`
   (am einfachsten den ganzen Ordner per Drag & Drop ins Browserfenster ziehen).
4. Oben auf **Actions** klicken. Der Build startet automatisch und dauert ca. 10 Minuten.
5. Wenn er grün ist: auf den Lauf klicken, unten bei *Artifacts* **Double-Mansi-VST3** herunterladen und entpacken.
6. Weiter mit „Installieren“.

## Weg B: Selbst auf dem PC bauen

1. Installieren:
   - **Visual Studio 2022 Community** mit der Komponente „Desktopentwicklung mit C++“
   - **CMake** (cmake.org, beim Installieren „Add to PATH“ anhaken)
   - **Git** (git-scm.com)
2. Im Ordner `DoubleMansi` eine Eingabeaufforderung öffnen (Adressleiste im Explorer: `cmd` eintippen, Enter).
3. Ausführen:
   ```
   cmake -B build -G "Visual Studio 17 2022" -A x64
   cmake --build build --config Release --target DoubleMansi_VST3
   ```
   Beim ersten Mal wird JUCE automatisch heruntergeladen, das dauert etwas.
4. Fertig liegt das Plugin unter `build\DoubleMansi_artefacts\Release\VST3\Double Mansi.vst3`.

## Installieren in Studio One 7

1. Den Ordner **`Double Mansi.vst3`** nach `C:\Program Files\Common Files\VST3\` kopieren.
2. Studio One starten → *Studio One > Optionen > Ort > VST-Plug-Ins* → **Neu scannen** (falls es nicht von selbst erscheint).
3. Im Browser unter *Effekte* nach „Double Mansi“ suchen und auf die Vocal-Spur ziehen.

**Tipp:** Funktioniert am besten auf einer Mono-Gesangsspur. Studio One macht daraus automatisch einen Stereo-Ausgang, damit die Doubles links und rechts liegen.

## Lizenzen
- Code: deins.
- JUCE: kostenlos für Personal-Nutzung (JUCE-Lizenz/AGPL).
- Schrift Bebas Neue: SIL Open Font License (siehe `Resources/OFL.txt`).
