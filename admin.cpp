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
          playSong(notesBuffer[id].data, time);
          notesBuffer[id].owner = "";
          notesBuffer[id].data = "";
          notesBuffer[id].maxLength = 0;
          notesBuffer[id].priority = 0;
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
          playSong(notesBuffer[id].data, time);
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
        sendIrcMessage("(MIDI) @" + user + " Liste von Puffern:");
        for(uint16_t i = 0; i < NOTES_BUFFER_LENGTH; i++){
          sendIrcMessage("(MIDI) @" + user + " " + i + ": " + notesBuffer[i].owner + " - " + notesBuffer[i].data.length() + "/" + notesBuffer[i].maxLength);
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
          notesBuffer[id].owner = e;
        }
        break;
      case 'D':
      case 'd':
        //set data
        if(isNumber(command.charAt(0))){
          uint32_t id = readNumber(command);
          deleteSpace(command);
          notesBuffer[id].data = command;
          return;
        }
        break;
      case 'A':
      case 'a':
        //add data
        if(isNumber(command.charAt(0))){
          uint32_t id = readNumber(command);
          deleteSpace(command);
          notesBuffer[id].data += command;
          return;
        }
        break;
      case 'C':
      case 'c':
        //clear
        if(isNumber(command.charAt(0))){
          uint32_t id = readNumber(command);
          deleteSpace(command);
          notesBuffer[id].owner = "";
          notesBuffer[id].data = "";
          notesBuffer[id].maxLength = 0;
          notesBuffer[id].priority = 0;
        }
        break;
      case 'P':
      case 'p':
        //print
        if(isNumber(command.charAt(0))){
          uint32_t id = readNumber(command);
          deleteSpace(command);
          sendIrcMessage("(MIDI) @" + user + " Daten vom Puffer " + id + ": " + notesBuffer[id].data);
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
        sendIrcMessage("(MIDI) @" + user + " Liste von Instrumenten:");
        for(uint16_t i = 0; i < AMOUNT_PRESET_INSTRUMENTS; i++){
          sendIrcMessage("(MIDI) @" + user + " " + i + ": " + instruments[i].name + " - instr=" + instruments[i].instrument + ", msb=" + instruments[i].bank_MSB + ", lsb=" + instruments[i].bank_LSB);
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
          instruments[id].name = e;
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
            instruments[id].bank_MSB = val;
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
            instruments[id].instrument = val;
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
            instruments[id].bank_LSB = val;
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
        sendIrcMessage("(MIDI) @" + user + " Liste von Preset Liedern:");
        for(uint16_t i = 0; i < AMOUNT_PRESET_SONGS; i++){
          sendIrcMessage("(MIDI) @" + user + " " + i + ": " + presetSongs[i].name);
        }
        break;
      case 'D':
      case 'd':
        //set data
        if(isNumber(command.charAt(0))){
          uint32_t id = readNumber(command);
          deleteSpace(command);
          presetSongs[id].data = command;
          return;
        }
        break;
      case 'A':
      case 'a':
        //add data
        if(isNumber(command.charAt(0))){
          uint32_t id = readNumber(command);
          deleteSpace(command);
          presetSongs[id].data += command;
          return;
        }
        break;
      case 'C':
      case 'c':
        //clear
        if(isNumber(command.charAt(0))){
          uint32_t id = readNumber(command);
          deleteSpace(command);
          presetSongs[id].data = "";
          presetSongs[id].name = "";
        }
        break;
      case 'P':
      case 'p':
        //print
        if(isNumber(command.charAt(0))){
          uint32_t id = readNumber(command);
          deleteSpace(command);
          sendIrcMessage("(MIDI) @" + user + " Daten vom Lied " + id + ": " + presetSongs[id].data);
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
          presetSongs[id].name = e;
        }
        break;
      }
      break;
    //end
    }
    deleteSpace(command);
  }
}
