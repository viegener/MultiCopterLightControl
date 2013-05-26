MultiCopterLightControl v0.2
============================

MCLC ist ein OpenSource Projekt um ein WS2811 RGB LED Strip mit einem Arduino zu steuern. 
Das Hauptziel ist die Darstellung und Steuerung von unterschiedlichen Lichteffekten für MultiCopter. 
Allerdings kann es auch mit wenigen Code-Anpassungen für andere Projekte genutzt weden, wie z.B. MoodLamps. 
Das Projekt befindet sich noch in der Entwicklungsphase, in welcher der Fokus auf der Entwicklung von neuen Features liegt und nicht auf der Benutzerfreundlichkeit.

Ich habe mich entschieden, die Bibliothek FastSPI_LED2 zu benutzen, obwohl sie noch in der Beta-Phase ist. 
Allerdings ist die Bibliothek im direkten Vergleich zum Vorgänger wesentlich schlanker und die Timings sind näher an den Spezifikationen der LED-Strips. 
Mein Resepkt und Dank gilt dem Entwickler dieser Bibliothek, da sie sehr einfach zu benutzen ist und ohne die Bibliothek hätte ich das Projekt nicht gestartet.
https://code.google.com/p/fastspi/

Nun werde ich in Kürze die Anforderungen, die Code-Struktur, die Effekte und die Einstellungen erklären.

Anforderungen
=============
Um den Sketch zu kompilieren, sind neben der aktuellen Arduino IDE, zwei Bibliotheken erforderlich:<br/>
<b>SerialCommand</b>: <a>https://github.com/kroimon/Arduino-SerialCommand</a><br/>
<b>FastSPI_LED2</b>: <a>http://code.google.com/p/fastspi/</a><br/>

An Hardware wird neben einem Arduino natürlich auch ein WS2811 RGB_LED-Strip benötigt. 
Optional werden momentan ein RC-Empfänger und eine MultiWii-FC als Steuerung unterstützt.

Ich persönlich benutzte einen Arduino Nano v3, einen kleinen 6-Kanal DSM2 Empfänger und eine MultiWii-FC mit Gyro, Accelerometer, Magnetometer und Barometer. 
Als Test-Konfiguration benutzte ich einen Quadrocopter mit jeweils 6 LEDs pro Arm.

Am MultiCopter sollte der LED-Strip mit einem BEC mit 5V versorgt werden.
Je nach Anzahl der LEDs ist die maximale Stromstärke des BEC zu wählen.
Maximal braucht jede LED bei voller Helligkeit und weisser Lichtfarbe 60mA.
D.h. in meiner Test-Konfiguration mit 4 Armen mit je 6 LEDs (=24 LEDs) werden 24*60mA = 1440 mAh, also etwa 1,5 A benötigt.
Zum Testen und Entwickeln am PC benutzte ich ein 5V Steckernetzteil mit 2,5 A, da die Leistung eines USB-Ports mit maximal 500mA (USB2) bzw. 900mA (USB3) nicht ausreicht.

Wie die RGB-Strips genau verkabelt werden, dazu gibt es im Internet genügend Informationen.
Hier kann ich z.B. auf die gute Kurzanleitung von MWC-Light RGB verweisen.
Dort wird sehr schön und bebildert erklärt, wie die RGB-LED-Strips zu verkabeln sind. 

Falls ich einmal Zeit und Lust dazu finde, werde ich auch eine ausfürhliche Erklärung zur Verkabelung hier posten.

Code-Struktur
=============
Der Quellcode ist in mehere Dateien unterteilt.

<b>MultiCopterLightControl.ino</b><br/>
Hauptdatei mit setup() und loop() Methoden. Hier werden auch die Lichteffekte aufgerufen.
In dieser Datei sind auch die grundlegenden Methoden zur Ansteuerung der LEDs implementiert.

<b>LedEffects.ino</b><br/>
Diese Datei beinhaltet die Methoden, welche spezielle Lichteffekte realisieren.

<b>Colors.ino</b><br/>
Hier wird die Farbverwaltung realisiert.
Das Projekt verwendet eine Farbpalette von maximal 16 Farben.
Diese 16 Farben können vollkommen frei definiert werden.
Zusätzlich können 8 verschiedene Led-Konfigurationen gespeichert werden.
Jeder LED wird hier jeweils ein Farbe aus der Farbpalette zugeordnet.
Diese Art der Farbverwaltung benötigt sehr wenig Speicherplatz und ermöglicht in Zukunft einige interessante Effekte.

<b>structs.h</b><br/>
Beinhaltet alle Definitionen von Strukturen, welche in dem Projekt benötigt werden.
Insbesondere wird hier die Struktur CRGB definiert.
Falls bei euch die Farben der LEDs nicht mit den eingestellten Farben in der Software übereinstimmen, dann muss diese Struktur geändert werden.
D.h. falls ihr Rot eingestellt habt (also z.B. [R=200, G=0, B=0]) und die LEDs z.B. grün leuchten, müsst ihr die Reihenfolge der entsprechenden Variablen in dem STRUCT ändern.

<b>config.h</b><br/>
Diese Datei ist angelehnt an die MultiWii Software.
Diese Datei muss entsprechend euren Anforderungn angepasst werden.

Die folgenden Dateien implementieren jeweils eine mögliche Variante, die LEDs und die Lichteffekte zu steuern.

<b>InputTerminal.ino</b><br/>
Hier wird die Konfiguration über ein Serial Terminal implementiert.
D.h. ihr ladet den Sketch per USB auf den Arduino und könnt dann mit dem Serial Monitor der Arduino IDE alles konfigurieren.
Die Baud Rate beträgt 115200.
Nur mit dieser Steuerungsvariante können die Farben eingestellt und gespeichert werden.
Mit den restlichen Steuerungs-Varianten können nur bestimmte Effekt-Modi gewählt werden.

<b>InputRC.ino</b><br/>
Hiermit kann ein Kanal eines RC-Empfänger als Steuerung benutzt werden.
Dazu wird der Empfänger an den entsprechen Pin angeschlossen, welcher in der Datei config.h eingestellt werden kann.
Es gibt dann einen Modus für LOW (650 >= rc > 1350), MID (1350 >= rc < 1700), HIGH (rc >= 1700) und NO_CONNECTION (rc < 650).

<b>InputMSP.ino</b><br/>
Hier wird die Kommunikation mit dem MultiWii Serial Protocol implementiert.
Die MultiWii FC wird an die RX und TX Pins des Arduino angeschlossen.
Dazu entsprechend RX vom Arduino an TX der FC anschliessen und TX vom Arduino an RX der FC.
Falls eurer Arduino oder euere FC mehere RX/TX Pins hat, müsst ihr euch entsprechend erkundigen, welche ihr benutzten müsst.

Einstellungen
=============

Generelle Einstellungen

<b>NUM_ARMS</b>        = Anzahl der Arme eures Multikopters<br/>
<b>LEDS_PER_ARM</b>    = Anzahl der LEDs pro Arm<br/>
<b>LED_PIN</b>         = Digitaler Arduino PIN, an welcher der erste LED-Strip angeschlossen werden muss<br/>

Steuerungs-Einstellugnen (nur jeweils ein Modus gleichzeit wählbar)

<b>INPUT_TERMINAL</b>  = Konfiguration / Steuerung der LEDs über den Serial Monitor<br/>
<b>INPUT_RC</b>        = Steuerung der LEDs über einen Kanal eines RC-Empgängers<br/>
<b>INPUT_MSP</b>       = Steuerung der LEDs über das MultiWii Serial Protocol<br/>

<b>RC_PIN</b>          = Digitaler Arduino Pin, an welcher ein Kanal des Empfängers angeschlossen werden muss<br/>
<b>REQUEST_DELAY</b>   = Intervall in Millisekunden, in welcher die MultiWii-FC nach neuen Werten gefragt wird<br/>
<b>REQUEST_MOTORS</b>  = Falls aktiviert, werden zusätzlich zu dem Flugmodus, auch die Motoroutputs abgefragt<br/>

Zusätzlich können die unterschiedlichen Modi der unterschiedlichen Steuerungs-Varianten auf einheitliche Modi MODE_0 bis MODE_3 gemappt werden.
In der loop()-Methode in der Datei "MultiCopterLightControl.ino" können dann grundsätzlich die Modi MODE_0 bis MODE_3 benutzt werden.

Konfiguration
=============

<b>setR N</b>           : setzt den Rot-Wert für die aktuelle Farbe, 0 <= N < 256<br/>
<b>setG N</b>           : setzt den Grün-Wert für die aktuelle Farbe, 0 <= N < 256<br/>
<b>setB N</b>           : setzt den Blau-Wert für die aktuelle Farbe, 0 <= N < 256<br/>
<b>setColor N</b>       : weist die aktuelle Farbe dem N.ten Platz in der Farbpalette zu, 0 <= N < 16<br/>
<b>getColor N</b>       : setzt die aktuelle Farbe auf die N.te Farbe der Farbpalette, 0 <= N < 16<br/>
<b>printColor</b>       : gibt die aktuelle Farbe in der Konsole aus<br/>
<b>setConfig N</b>      : wählt die aktuelle LED-Konfiguration, 0 <= N < 8<br/>

<b>saveColors</b>       : Speichert die aktuelle Farbpalette im EEPROM<br/>
<b>saveLeds</b>         : Speichert die aktuellen Led-Konfigurationen<br/>
<b>clearEeprom</b>      : Löscht das EEPROM und löscht damit die Farbpalette und LED-Konfiguartionen im Speicher<br/>

<b>setArm N</b>         : weist die zuletzt gesetzte/geladene Palettenfarbe dem N.ten Arm zu, 0 <= N < NUM_ARMS<br/>
<b>setLed N</b>         : weist die zuletzt gesetzte/geladene Palettenfarbe der N.ten LED zu, 0 <= N < NUM_LEDS<br/>

<b>m+</b>               : Wählt den nächsten Modus<br/>
<b>m-</b>               : Wählt den vorherigen Modus<br/>
<b>d N</b>              : Setzt die Verzögerung eines Effekts auf N Millisekunden<br/>

Bei der Erst-Konfigurationen solltet ihr wie folgt vorgehen:
<ol>
  <li>Zuerst einmal den Speicher löschen mit "clearEeprom"</li>
  <li>Dann kann die Farbpalette erstellt werden
    <ul>
      <li>Die aktuelle Farbe mit "setR", "setG" und "setB" auf die gewünschte Farbe setzten</li>
      <li>Die aktuelle Farbe einem Paletten-Platz zuordnen mit "setColor"
      <li>Die beiden Schritte solange wiederholen, bis entweder alle 16 Palettenplätze belegt sind oder alle benötigten Farben gespeichert sind</li>
      <li>Die Farbpalette speichern mit "saveColors"</li>
    </ul>
  </li>
  <li>Nachdem die Farbpalette erstellt ist, können LED-Konfigurationen bzw. Muster erstellt werden
    <ul>
      <li>Mit "setConfig" die aktuelle LED-Konfiguration wählen</li>
      <li>Mit "getColor" eine Farbe der Palette wählen</li>
      <li>Die Farbe mit "setArm" oder "setLed" einem Arm oder einer LED zuweisen</li>
      <li>Die Schritte solange wiederholen, bis alle LEDs und alle gewünschten Konfigurationen eingerichtet sind</li>
      <li>Die LED-Konfigurationen speichern mit "saveLeds"
    </ul>
  </li>
</ol>

LED-Effekte
===========

<b>oneColor(CRGB color, int DELAY)</b><br/>
Auf allen LEDs wird eine Farbe dargestellt.

<b>showCurrentColors(int iConfig, int DELAY)</b><br/>
Die LEDs leuchten in den Farben entsprechend der gewählten Konfiguration.

<b>police(CRGB color1, CRGB color2, int DELAY)</b><br/>
Die LEDs blinken (pro Arm) abwechseln in den beiden angebenen Farben.

<b>runningLed(int iConfig, CRGB* blinkColor, int blinkDelay, boolean bounce, int length, int DELAY)</b><br/>
Ein LED-Streifen läuft von aussen nach innen (auf jedem Arm) bzw. wieder zurück.
Es muss eine LED-Konfiguration angegeben werden, sodass die LEDs in den entsprechenden Farben leuchten.
"blinkColor" kann NULL sein, oder man gibt explizit eine Farbe an.
Falls "blinkDelay" > 0 ist, wird am Ende einer Animation (wenn der LED-Streifen die Arm-Enden erreicht) ein Blitz ausgelöst.
Falls "blinkColor" NULL ist, wird für den Blitz die Farben der Konfiguration benutzt, ansonsten blitzen alle LEDs in der gewählten Farbe auf.
"bounce" gibt an, ob der Streifen hin und her laufen soll, oder nur von innen nach aussen.
"length" gibt die Länge des LED-Streifens an, welcher hin und her läuft.

<b>showBrightness(int iConfig, int brightness, int DELAY)</b><br/>
Stellt die Farben der gewählten Konfiguration mit einer gewählten Helligkeit dar.

<b>pulseBrightness(int iConfig, int startBrightness, int endBrightness, int steps, int DELAY)</b><br/>
Fadet die Helligkeit der LEDs zwischen den beiden gewählten Werte.
Auch hier muss wieder eine Konfiguration angegeben werden.
"steps" gibt an, wie fein/grob das Fading sein soll.
