# Esp32MidiPlayer
Benötigte Bibliotheken:
* fortyseveneffects/MIDI Library
* knolleary/PubSubClient
* bblanchon/ArduinoJson

## Playmidi-Syntax

`[;[l][n]][-][bpm<BPM>] [<INSTRUMENT>] <NOTEN...>`


### Flag Erweiterter Modus (optional)

`-...`

Ist das allererste Zeichen ein `-`, wir der erweiterte Modus aktiviert. Die Noten werden dann solange gehalten, bis sie zum nächsten Mal erwähnt werden.


### BPM (optional)

`... bpm<BPM> ...`

Setzt das Tempo des Liedes in *beats per minute* (BPM). Standardmäßig werden die Lieder mit Tempo 240 wiedergegeben.


### Instrument (optional)
Setzt das genutzte Instrument.
Liste der verfügbaren Instrumente:
* `piano` (Klavier)
* `vibes` (Vibraphon)
* `organ` (Orgel)
* `guitar` (Gitarre)
* `brass` (Bläser)
* oder Nummer des Midi-Instruments

### Noten

`<Note> [M][Note] [M][Note]...`

Es können beliebig viele Noten hintereinander auftauchen. Einzelne Noten werden mit Leerzeichen getrennt. Sollen mehrere Noten zugleich gespielt werden, so werden die Noten ohne Leerzeichen hintereinander geschrieben.

#### Notenbestandteile

Eine Note setzt sich zusammen aus der **Tonhöhe**, einem **Vorzeichen** (optional), einem **Oktavzeichen** (optional) und dem **Notenwert** (optional).

Beispiele für Noten sehen folgendermaßen aus: `C` oder `d#''4`
##### Tonhöhe
Die Tonhöhe ergibt sich aus den deutschen Notenbezeichnungen (`CDEFGAH`) und einem optionalen Vorzeichen, welches nach der Notenbezeichnung steht (`#`, `b`).
Die Oktave ergibt sich aus bis zu drei Oktavzeichen (`'`)
Die Reihenfolge der Oktaven von tief nach hoch sieht folgendermaßen aus:  
`C''' C'' C' C c c' c'' c'''`.
##### Notenwert (optional)
Die Länge ist eine Ganzzahl und gibt den Kehrwert der Notenlänge an.
* 1 entspricht einer ganzen Note
* 2 entspricht einer halben Note
* 4 entspricht einer Viertelnote
* 8 entspricht einer Achtelnote
* 16 entspricht einer Sechzehntelnote

Folgt der Länge ein Punkt (`.`), wird die Note um den anderthalbfachen Wert verlängert (Punktierung).

#### Besonderheit: Akkorde
Mehrere direkt aufeinanderfolgende Töne werden zusammen gespielt.
Beispie: `CEGC GHdg FAcf`

#### Modus wächsel
Mit `m` kann der Modus zwischen dem "standart" und dem "erweiterten" gewächselt werden.

#### Noten im *erweiterten Modus*

`[Ton]...[s][Ton]...[Länge / k / i]`

##### Besondere Funktionen
###### Instrument ändern
`i<instrument>`  
Wechselt das Aktuelle Instrument für die nachfolgenden Noten.  
`-organ ... ipiano ...`

###### Alle Noten stoppen
Mit `s` werden alle Noten gestoppt, die zur Zeit klingen.

###### Midi-Channel wechseln
`k<Midikanal>`
Mit `k` kann der aktuelle Midi-Kanal von 1-16 gewechselt werden, um z.B. mehrere Instrumente gleichzeitig spielen zu können.
`-piano A k2 iorgan H 4 s`

###### Lauststärke ändern
`v<Neue Lautstärke>`
Mit `v` wird die lautstärke in dem Aktuellen Midi-Kanal geändert \(0-127\) \(Keine garantie da nicht jeder Midi-Synthesizer diese Funktion unterstützt\);
`-piano A1 v64 A1`

###### zuletzt genannte Note Stoppen
`l`
Mit `l` wird die zuletzt genannte Note gestoppt oder gestartet.

###### Ton
Wenn der erweiterte Modus aktiv ist, wird ein Ton bis zur nächsten Erwähnung im gleichen Kanal oder bis zum nächsten `s` gespielt.  
`a (a wird gespielt) 4 (a wird gespielt) a (a wird nicht mehr gespielt)
und a (a wird wieder gespielt) 4 (a wird gespielt) s (a wird nicht mehr gespielt)`

### Puffer (optional)
`;...` fals keine unten genanten Pufferaktionen folen werden folgende Noten in den Puffer geschrieben.

#### Puffer löschen
`;l...` löscht den Puffer.

#### neuen Puffer erstellen
`;[l]n...` erschaft einen neuen Puffer folgende Noten werden in diesen eingefügt.

## Mqtt

Der Midi-Player reagiert auf den MQTT-topic `playmidi`. Dort wird ein einfacher String mit JSON-Daten oder rohen Lied Daten empfangen.

## JSON

Es handelt sich um ein JSON-Objekt mit folgenden tags:
* `laenge` - maximale Lied laenge
* `midi` - daten des Lides

optional:
* `aktiviereBuffer` - fals vorhanden und der Wert `true` ist, wird die buffer funktion aktiviert.

Bei aktivirtem buffer werden folgende tags benötigt:
* `nutzer` - name des users
* `prioritaet` - priorität endscheidet welce buffer zu erst gelöscht werden, kleinere zahlen werden eher gelöscht. z.B.: der buffer ist mit priorität 1 gefüllt dan würde wenn einer mit priorität 1 einen Puffer erstellen möchte einen feher bekommen, einer mit priorität 2 nicht da dies einen buffer mit priorität 1 überschreibt.
* `maximaleBufferGroesse` - die maximale größe des Puffers
