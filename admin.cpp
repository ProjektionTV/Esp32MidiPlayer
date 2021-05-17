#include "admin.h"

void parseAdminCommand(String command, String user){
  deleteSpace(command);
  uint8_t state = 0;
  while(command.length() != 0){
    char c = command.charAt(0);
    command.remove(0,1);
    deleteSpace(command);
    //interpret
    switch(state){
    //base
    case 0:
      switch (c){
      case 'P':
      case 'p':{
        uint32_t time = 16;
        if(isNumber(command.charAt(0))){
          time = readNumber(command);
          deleteSpace(command);
        }
        playSong(command, time);
        return;
        break;
      }
      case '~':
        if(isNumber(command.charAt(0))){
          uint32_t id = readNumber(command);
          deleteSpace(command);
          uint32_t time = 16;
          if(isNumber(command.charAt(0))){
            time = readNumber(command);
            deleteSpace(command);
          }
          playSong(notenBuffer[id].daten, time);
          notenBuffer[id].besitzer = "";
          notenBuffer[id].daten = "";
          notenBuffer[id].maximaleLaenge = 0;
          notenBuffer[id].priority = 0;
        }
        break;
      case 'q':
        if(isNumber(command.charAt(0))){
          uint32_t id = readNumber(command);
          deleteSpace(command);
          uint32_t time = 16;
          if(isNumber(command.charAt(0))){
            time = readNumber(command);
            deleteSpace(command);
          }
          playSong(notenBuffer[id].daten, time);
        }
        break;
      case 'B':
      case 'b':
        state = 1;
        break;
      case 'I':
      case 'i':
        state = 2;
        break;
      case 'L':
      case 'l':
        state = 3;
        break;
      }
      break;
    //buffer
    case 1:
      switch (c){
      case ';':
        state = 0;
        break;
      case 'L':
      case 'l':
        schreibeChatNachricht("(MIDI) @" + user + " Liste von Puffern:");
        for(uint16_t i = 0; i < NOTEN_BUFFER_LAENGE; i++){
          schreibeChatNachricht("(MIDI) @" + user + " " + i + ": " + notenBuffer[i].besitzer + " - " + notenBuffer[i].daten.length() + "/" + notenBuffer[i].maximaleLaenge);
        }
        break;
      case 'O':
      case 'o':
        //set owner
        if(isNumber(command.charAt(0))){
          uint32_t id = readNumber(command);
          deleteSpace(command);
          String e = "";
          while((command.length() != 0) && (command.charAt(0) != ' ') && (command.charAt(0) != ';')){
            e += "" + command.substring(0, 1);
            command.remove(0,1);
          }
          notenBuffer[id].besitzer = e;
        }
        break;
      case 'D':
      case 'd':
        //set data
        if(isNumber(command.charAt(0))){
          uint32_t id = readNumber(command);
          deleteSpace(command);
          notenBuffer[id].daten = command;
          return;
        }
        break;
      case 'A':
      case 'a':
        //add data
        if(isNumber(command.charAt(0))){
          uint32_t id = readNumber(command);
          deleteSpace(command);
          notenBuffer[id].daten += command;
          return;
        }
        break;
      case 'C':
      case 'c':
        //clear
        if(isNumber(command.charAt(0))){
          uint32_t id = readNumber(command);
          deleteSpace(command);
          notenBuffer[id].besitzer = "";
          notenBuffer[id].daten = "";
          notenBuffer[id].maximaleLaenge = 0;
          notenBuffer[id].priority = 0;
        }
        break;
      case 'P':
      case 'p':
        //print
        if(isNumber(command.charAt(0))){
          uint32_t id = readNumber(command);
          deleteSpace(command);
          schreibeChatNachricht("(MIDI) @" + user + " Daten vom Puffer " + id + ": " + notenBuffer[id].daten);
        }
        break;
      }
      break;
    //instrument
    case 2:
      switch (c){
      case ';':
        state = 0;
        break;
      case 'L':
      case 'l':
        // list
        schreibeChatNachricht("(MIDI) @" + user + " Liste von Instrumenten:");
        for(uint16_t i = 0; i < MENGE_PRESET_INSTRUMENTE; i++){
          schreibeChatNachricht("(MIDI) @" + user + " " + i + ": " + instrumente[i].name + " - instr=" + instrumente[i].instrument + ", msb=" + instrumente[i].bank_MSB + ", lsb=" + instrumente[i].bank_LSB);
        }
        break;
      case 'N':
      case 'n':
        //set name
        if(isNumber(command.charAt(0))){
          uint32_t id = readNumber(command);
          deleteSpace(command);
          String e = "";
          while((command.length() != 0) && (command.charAt(0) != ' ') && (command.charAt(0) != ';')){
            e += "" + command.substring(0, 1);
            command.remove(0,1);
          }
          instrumente[id].name = e;
        }
        break;
      case 'M':
      case 'm':
        //set msb
        if(isNumber(command.charAt(0))){
          uint32_t id = readNumber(command);
          deleteSpace(command);
          if(isNumber(command.charAt(0))){
            uint32_t val = readNumber(command);
            deleteSpace(command);
            instrumente[id].bank_MSB = val;
          }
        }
        break;
      case 'I':
      case 'i':
        //set instrument
        if(isNumber(command.charAt(0))){
          uint32_t id = readNumber(command);
          deleteSpace(command);
          if(isNumber(command.charAt(0))){
            uint32_t val = readNumber(command);
            deleteSpace(command);
            instrumente[id].instrument = val;
          }
        }
        break;
      case 'D':
      case 'd':
        //set lsb
        if(isNumber(command.charAt(0))){
          uint32_t id = readNumber(command);
          deleteSpace(command);
          if(isNumber(command.charAt(0))){
            uint32_t val = readNumber(command);
            deleteSpace(command);
            instrumente[id].bank_LSB = val;
          }
        }
        break;
      }
      break;
    //lied
    case 3:
      switch (c){
      case ';':
        state = 0;
        break;
      case 'l':
      case 'L':
        schreibeChatNachricht("(MIDI) @" + user + " Liste von Preset Liedern:");
        for(uint16_t i = 0; i < MENGE_PRESET_LIEDER; i++){
          schreibeChatNachricht("(MIDI) @" + user + " " + i + ": " + presetLieder[i]);
        }
        break;
      case 'D':
      case 'd':
        //set data
        if(isNumber(command.charAt(0))){
          uint32_t id = readNumber(command);
          deleteSpace(command);
          presetLieder[id] = command;
          return;
        }
        break;
      case 'A':
      case 'a':
        //add data
        if(isNumber(command.charAt(0))){
          uint32_t id = readNumber(command);
          deleteSpace(command);
          presetLieder[id] += command;
          return;
        }
        break;
      case 'C':
      case 'c':
        //clear
        if(isNumber(command.charAt(0))){
          uint32_t id = readNumber(command);
          deleteSpace(command);
          presetLieder[id] = "";
        }
        break;
      }
      break;
    //end
    }
    deleteSpace(command);
  }
}
