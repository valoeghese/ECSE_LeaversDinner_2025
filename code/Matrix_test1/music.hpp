/* ========================================
 *
 * Copyright Norman the Nomad, 2024
 * All Rights Reserved
 *
 * ========================================
*/

#include <stdint.h>

extern uint8_t numSongs;
extern int8_t currentSong;

/**
 * Launch music on the second core.
 */
void core1MusicMain(void);

uint8_t isPlaying(void);
void nextNote();
void toggleMusic();
void stopMusic();
int8_t playSong(int8_t songID);
void initMusic();

/* [] END OF FILE */