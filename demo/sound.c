/*sound.c*/

#include <dsound.h>

/*array of notes that make up the refrain*/
/*of Devil with a Blue Dress*/

static const note_t devil[] = { 
  { PITCH_G4, QUARTER },
  { PITCH_G4, QUARTER },
  { PITCH_G4, QUARTER },
  { PITCH_G4, QUARTER },
  { PITCH_G4, HALF },
  { PITCH_G4, HALF },

  { PITCH_G4, HALF },
  { PITCH_G4, HALF },
  { PITCH_G4, HALF },
  { PITCH_G4, HALF },

  { PITCH_F4, QUARTER },
  { PITCH_F4, QUARTER },
  { PITCH_F4, QUARTER },
  { PITCH_F4, QUARTER },
  { PITCH_F4, HALF },
  { PITCH_F4, HALF },

  { PITCH_F4, HALF },
  { PITCH_PAUSE, HALF },
  { PITCH_PAUSE, HALF },
  { PITCH_PAUSE, HALF },

  { PITCH_E4, QUARTER },
  { PITCH_E4, QUARTER },
  { PITCH_E4, QUARTER },
  { PITCH_E4, QUARTER },
  { PITCH_F4, HALF },
  { PITCH_F4, HALF },

  { PITCH_E4, HALF },
  { PITCH_E4, HALF },
  { PITCH_F4, HALF },
  { PITCH_F4, HALF },

  { PITCH_E4, QUARTER },
  { PITCH_E4, QUARTER },
  { PITCH_E4, QUARTER },
  { PITCH_E4, QUARTER },
  { PITCH_F4, HALF },
  { PITCH_F4, HALF },

  { PITCH_E4, HALF },
  { PITCH_PAUSE, HALF }, 
  { PITCH_PAUSE, HALF }, 
  { PITCH_PAUSE, HALF }, 
  { PITCH_END, 0 }
};

    
int main(int argc,char *argv[]) {
  
  /*The default makes this a really, really slow song*/
  /*So, we speed it up a little bit.*/
  dsound_set_duration(40);

  /*now, we play it*/
  while(1) {
    dsound_play(devil);
    wait_event(dsound_finished,0);
    sleep(1);
  }

  return 0;
}

