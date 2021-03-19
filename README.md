# <ins> Esp32MidiPlayer </ins>
Benötigte Libs:<br>
fortyseveneffects/MIDI Library<br>
knolleary/PubSubClient<br>

---

## <ins> Playmidi Syntax </ins>

\[Erweitertermodus\] \<Instrument\> \<Noten\>

<br>

---

### <ins> Instrument </ins>
Setzt das genutzte Instrument.<br>
Instrument Liste:<br>
- piano
- vibes
- organ
- guitar
- brass
- oder Midi Instrumenten Nummer

---

### <ins> Erweitertermodus </ins>

Der Erweitertermodus wird aktivirt indem man ein „-“ am anfang der Nachricht Steht.<br>
**Dieser verändert die Syntax der Noten!**<br>
z.B.: -brass ...

---

### <ins> Noten </ins>

\<Note\> \<Note\> ...
<br>

#### <ins> Notenbestnteile </ins>
##### <ins> Länge </ins>
Die Länge ist eine Ganzzahl und gibt an eine wieviltel Note das ist.<br>
z.B.: 1 - ganze Note<br>
    2 - halbe Note<br>
    4 - virtel <br>
    8 - achtel Note<br>
    16 - sechszehntel Note
##### <ins> Ton </ins>
Bei den Tönen sind Halbtöne nur mit „#“ möglich und man kann bis zu drei Oktavenzeichen setzen „'“.<br>
Tonleiter: C C# D D# E F F# G G# A A# H c c# d d# e f f# g g# a a# h
#### <ins> Note im „Standartmodus“ </ins>
Hir werden die Töne für die angegebene Länge **gemeinsam** gespielt. **Es ist wichtig wenn sie zu sammen gespielt werden sollen darf kein „ “ (Lehrzeichen) vorhanden seien.**<br>
z.B.: G'H'D

\<Ton\>\[Ton\]\[Ton\]...\[Länge standart=4\]

#### <ins> Note im „Erweitertermodus“ </ins>

\[Ton\]...\[s\]\[Ton\]...\[Länge / k / i\]

##### <ins> Besondere Funktionen </ins>
###### <ins> i/I </ins>
Wechselt das Aktuelle Instrument im Midi-Kanal. Nach dem „i/I“ wird ein Instrument wie am Anfag genommen.<br>
z.B.: -organ ... ipiano ...<brS>

i\<instrument\>

###### <ins> s/S </ins>
Stopt alle aktuellspilenden Noten.<br>
z.B.: -piano A1 s1<br>

s

###### <ins> k/K </ins>
Wechselt den Akktuellen Midi-kanal um z.B.: mehre instrumente gleichzeitig ertönen zu Lassen.<br>
z.B.: -piano A k2 iorgan H 4 s

k\<Zahl - Neuer Kanal 1-16\>

###### <ins> Ton </ins>
**Im „Erweitertenmodus“ wird ein Ton bis zur nächsten Erwähnung im gleichen Kanal oder bis zum nächsten s gespielt.**<br>
Es folgt: a \<a wird gespielt\> 4 \<a wird gespielt\> a \<a wird nicht gespielt\><br>
und a \<a wird gespielt\> 4 \<a wird gespielt\> s \<a wird nicht gespielt\>
