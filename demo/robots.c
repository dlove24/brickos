// Plays "wir sind die roboter" theme by Kraftwerk.

#include <config.h>
#if defined(CONF_DSOUND)

#include <dsound.h>
#include <tm.h>

//! "wir sind die roboter" theme by Kraftwerk
static const note_t robots[] = { 
  { PITCH_D4,  2 } , { PITCH_C4,  1 } , { PITCH_D4,  1 },
  { PITCH_F4,  1 } , { PITCH_D4,  1 } , { PITCH_D4,  2 },
  { PITCH_F4,  2 } , { PITCH_G4,  1 } , { PITCH_C5,  1 },
  { PITCH_A4,  2 } , { PITCH_D4,  2 } , { PITCH_END, 0 }
};
    
int main(int argc,char *argv[]) {
  while (!shutdown_requested()) {
    if (wait_event(dsound_finished,0) != 0)
	    dsound_play(robots);
  }
  return 0;
}
#else // CONF_DSOUND
#warning robots.c requires CONF_DSOUND
#warning robots demo will do nothing
int main(int argc, char *argv[]) {
  return 0;
}
#endif // CONF_DSOUND
