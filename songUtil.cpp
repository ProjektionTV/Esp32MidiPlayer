#include "songUtil.h"

void parserT(String buffer) {
  if (buffer.charAt(0) == 'm' || buffer.charAt(0) == 'M') {
    buffer.remove(0,1);
#if ALLOW_PARSER_2
    parserV2 = !parserV2;
#endif
  }
  if (parserV2){
    parser2(buffer);
  } else {
#if ENABLE_PARSER_1_1
    parser1_1(buffer);
    parser2allOFF();
#else
    parser(buffer);
#endif
  }
}

void readInstrument(String &s) {
    if (isNumber(s.charAt(0))) {
      uint32_t midiInstrument = readNumber(s);
      if(midiInstrument < 128)
        MIDI.sendProgramChange(midiInstrument, currentChanal);
    }

    for (uint8_t i = 0; i < MENGE_PRESET_INSTRUMENTE; i++) {
      if(s.startsWith(instrumente[i].name)){
        s.remove(0, instrumente[i].name.length());
        MIDI.sendProgramChange(instrumente[i].instrument,currentChanal);
        MIDI.sendControlChange(0, instrumente[i].bank_MSB, currentChanal);  //MSB
        MIDI.sendControlChange(32, instrumente[i].bank_LSB, currentChanal); //LSB
      }
    }
}

String expandLoops(String input) {
    bool inLoop = false;
    String out = "";
    String buff;
    bool rec = false;
    while (input.length() > 0) {
        char loc = input.charAt(0);
        input.remove(0, 1);
        if (inLoop){
            switch (loc){
            case 'w':
            case 'W':
                inLoop = false;
                out += buff;
                buff = "";
                break;
            case 'u':
            case 'U':
                rec = false;
                out += buff;
                break;
            case 'n':
            case 'N':
                rec = false;
                break;
            
            default:
                out += loc;
                if (rec)
                    buff += loc;
                break;
            }
        } else {
            switch (loc){
            case 'w':
            case 'W':
                inLoop = true;
                buff = "";
                rec = true;
                break;
            
            default:
                out += loc;
                break;
            }
        }
    }
    return out;
}
