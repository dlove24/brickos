// Plays "wir sind die roboter" theme by Kraftwerk.

#include <dsound.h>

//! "wir sind die roboter" theme by Kraftwerk
static const note_t robots[] = { 
  { PITCH_D4,  2 } , { PITCH_C4,  1 } , { PITCH_D4,  1 },
  { PITCH_F4,  1 } , { PITCH_D4,  1 } , { PITCH_D4,  2 },
  { PITCH_F4,  2 } , { PITCH_G4,  1 } , { PITCH_C5,  1 },
  { PITCH_A4,  2 } , { PITCH_D4,  2 } , { PITCH_END, 0 }
};

    
int main(int argc,char *argv[]) {
  while(1) {
    dsound_play(robots);
    wait_event(dsound_finished,0);
  }

  return 0;
}

