# Esp32MidiPlayer
Benötigte Bibliotheken:
* fortyseveneffects/MIDI Library
* knolleary/PubSubClient
* bblanchon/ArduinoJson

## Playmidi-Syntax

`[;[l][n]][-][bpm<BPM>] [<INSTRUMENT>] <NOTEN...>`/`~<lied Preset>`


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

Eine Note setzt sich zusammen aus der **Tonhöhe**, einem **Oktavzeichen** (optional), einem **Vorzeichen** (optional) und dem **Notenwert** (optional).

Beispiele für Noten sehen folgendermaßen aus: `C` oder `d''#4`
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

###### bpm änderung
`q<neue BPM>`

###### controllchange 0 MSB-Bank standart: 0
`x<neuer Wert>`

###### controllchange 32 LSB-Bank standart: 0
`y<neuer Wert>`

###### controllchange 72 relese standart: 63
`j<neuer Wert>`

###### controllchange 73 attack standart: 63
`o<neuer Wert>`

###### Ton
Wenn der erweiterte Modus aktiv ist, wird ein Ton bis zur nächsten Erwähnung im gleichen Kanal oder bis zum nächsten `s` gespielt.  
`a (a wird gespielt) 4 (a wird gespielt) a (a wird nicht mehr gespielt)
und a (a wird wieder gespielt) 4 (a wird gespielt) s (a wird nicht mehr gespielt)`

### Wiederholung
Achtung: Der aktuelle Modus wird nicht beachtet, woduch eine Wiederholung im selben Modus aufhören solte, wie sie angefangen hat.

#### w
Aktiviert/Deaktiviert die wiederholung
`...w<0>w...`→`...<0><0>...`

#### n
noten die nach dem ersten spielen gespielt werden
`...w<0>n<1>w...`→`...<0><1><0>...`

#### u
`u` wird nach `n` genutzt um weitere wieder holungen anzuhengen mit optional noten
* `...w<0>n<1>u[2]w...`→`...<0><1><0>[2]<0>...`
* `...w<0>n<1>u[2]u[3]w...`→`...<0><1><0>[2]<0>[3]<0>...`
* ...

#### Beispiele
* `cw d ew g` → `c d e d e g`
* `cw d en fw g` → `c d e f d e g`
* `cw d en fu dw g` → `c d e f d e d d e g`

### Puffer (optional)
`;...` fals keine unten genanten Pufferaktionen folen werden folgende Noten in den Puffer geschrieben.

#### Puffer löschen
`;l...` löscht den Puffer.

#### neuen Puffer erstellen
`;[l]n...` erschaft einen neuen Puffer folgende Noten werden in diesen eingefügt.

### lied Preset
* `0`- `118 c4 C'4 mi119 c'2`
* `1`- `brass F2 G#2 F4 F8 A#4 F4 D# F2 c2 F4 F8 c# c4 G# F4 c4 f4 F4 D# D#8 C G# F1` - `Klavier Spielen` beispiel melodie
* `2`- `-126 c1 1 1 1 1 1` - Aplaus
* `3`- ... - Gravity Falls
* `4`- ... - Vivaldi
* `5`- ... - cowntdown\(/Vorhersage/Umfrage Melodie\)
* `6`- ... - irgendwas was `d7gr` gemacht hat
* `7`- ... - trinken
* `8`- ... - Ducktales Intro theme
* `9`- ... - Wenn ich reich wär - Abomelodie
* `10`- ... - Für Elise
* `11`- ... - Sandmann
* `12`- ... - Airwolf

## Mqtt

Der Midi-Player reagiert auf den MQTT-topic `playmidi`. Dort wird ein einfacher String mit JSON-Daten oder rohen Lieddaten empfangen.
Wenn eine Nachricht vom MQTT-topic `killmidi` emfangen wird, wird das aktuelle lied unterbrochen und die play queue wird geleert.

## JSON

Es handelt sich um ein JSON-Objekt mit folgenden tags:
* `laenge` - maximale Lied länge
* `midi` - daten des Lides

optional:
* `aktiviereBuffer` - falls vorhanden und der Wert `true` ist, wird die buffer funktion aktiviert.
* `adminModus` - falls vorhanden und der Wert `true` ist, wird die admin funktion aktiviert.

Bei aktiviertem buffer werden folgende tags benötigt:
* `nutzer` - name des users
* `prioritaet` - priorität endscheidet welche buffer zu erst gelöscht werden, kleinere zahlen werden eher gelöscht. z.B.: der buffer ist mit priorität 1 gefüllt dan würde wenn einer mit priorität 1 einen Puffer erstellen möchte einen fehler bekommen, einer mit priorität 2 nicht da dies einen buffer mit priorität 1 überschreibt.
* `maximaleBufferGroesse` - die maximale größe des Puffers


Bei aktiviertem admin modus werden folgende tags benötigt:
* `nutzer` - name des users

## Adminmodus MIDI syntax
`command;[command;...]`

### p
Spielt die folgenden daten. Es können keine weiteren befehle angehängt werden.
`p <noten>`

### ~
Spielt den Puffer und löscht diesen
`~ <Puffer id>`

### q
Spielt den Puffer
`q <Puffer id>`

### i

instrument optionen

#### l
listet die instrumente.
`il`

#### i
setzt die instrument nummer des instrumentes
`ii<interne instrument id> <instrument id>`

#### m
setzt die msb des instrumentes
`im<interne instrument id> <msb>`

#### d
setzt die lsb des instrumentes
`id<interne instrument id> <lsb>`

#### n
setzt den namen des instrumentes, mit welchen man dies ereicht
`in<interne instrument id> <name ohne lerzeichen>`

### l

#### l
listet all lieder und deren daten
`ll`

#### d
setzt die daten des liedes. Es können keine weiteren befehle angehengt werden.
`ld<lied id> <noten>`

#### a
fügt die folgenden daten zum lied hinzu. Es können keine weiteren befehle angehengt werden.
`la<lied id> <noten>`

#### c
löscht die daten des liedes
`lc<lied id>`

### b

#### l
listet alle puffer und deren besitzer
`bl`

#### d
setzt die daten des puffer. Es können keine weiteren befehle angehängt werden.
`bd<puffer id> <noten>`

#### a
fügt die folgenden daten zum lied hinzu. Es können keine weiteren befehle angehengt werden.
`ba<puffer id> <noten>`

#### c
löscht die daten des puffers
`bc<puffer id>`

#### o
setzt den besitzer des puffers
`bo<puffer id> <neuer besitzer ohne lerzeichen>`

#### p
listet die daten des puffers
`bp<puffer id>`
