#include <Wire.h>

int global_action = 0;                  // 1,4-decrease speed (0.1 km/hr) | 8,16-increase speed (0.1 km/hr) | 2-start/stop | 153 - pair | 0 - blank
int global_no_of_writes = 0;            // = 15 + (5 * global_no_of_clicks)

int workout_id = -1;
int action_i = 0;
long unsigned start_time_ms = 0;

int paused_workout_id = -1;
int paused_action_i = 0;
long unsigned time_passed_for_pause_ms = 0;
int resume_workout_after_play = 0;

void setup() {
  //keeping serial monitor open, will give discrepancy in timing. close serial monitor before trail.
  Serial.begin(9600);                   // start serial for output
  Serial.println("Serial Begin");
  Wire.begin(18);                       // join i2c bus with address #18 (Treadmill Remote slave address - 18)
  Wire.onRequest(requestEvent);         // register event
}

void loop() {
  //snippets:
  //action(2, 1);         // start/pause (1 click) (coundown during start takes 3 sec (.05 min). so, immediate action after start should wait atleast .06 min)
  //action(2, 30);        // stop (28 clicks or above) (should trigger stop 3 sec in advance (.05 min))
  //action(8, 20);        // increase speed by 2 km/hr (0.1 km/hr x 20 clicks)
  //action(8, 40);        // increase speed by 4 km/hr (0.1 km/hr x 40 clicks)
  //action(8, 60);        // increase speed by 6 km/hr (0.1 km/hr x 60 clicks)
  //action(1, 20);        // decrease speed by 2 km/hr (0.1 km/hr x 20 clicks)
  //action(1, 40);        // decrease speed by 4 km/hr (0.1 km/hr x 40 clicks)
  //action(1, 60);        // decrease speed by 6 km/hr (0.1 km/hr x 60 clicks)

  extern int workout_id;
  extern int action_i;
  extern long unsigned start_time_ms;

  extern int paused_workout_id;
  extern int paused_action_i;
  extern long unsigned time_passed_for_pause_ms;
  extern int resume_workout_after_play;
  
  if (Serial.available()) {
    String tm_cmd = Serial.readString();
    tm_cmd.toUpperCase();

    //select workout
    if ( tm_cmd.substring(0,10) == "TM+WORKOUT") {
      noInterrupts();
      workout_id = tm_cmd.substring(10).toInt();
      action_i = 0;
      start_time_ms = 0;
      interrupts();
    }
    //pause selected workout
    else if (tm_cmd == "TM+PAUSE" && workout_id != -1) {
      paused_workout_id = workout_id;
      paused_action_i = action_i;
      time_passed_for_pause_ms = millis() - start_time_ms;  //pause time = current time - start time

      noInterrupts();
      workout_id = 1;
      action_i = 0;
      start_time_ms = 0;
      interrupts();
    }
    //resume paused workout (part I - play)
    else if (tm_cmd == "TM+RESUME" && paused_workout_id != -1) {
      //play
      noInterrupts();
      workout_id = 1;
      action_i = 0;
      start_time_ms = 0;
      interrupts();

      resume_workout_after_play = 1;
    }
    //stop selected workout
    else if (tm_cmd == "TM+STOP") {
      noInterrupts();
      workout_id = 2;
      action_i = 0;
      start_time_ms = 0;
      interrupts();
    }

    Serial.println("------------");
    Serial.println(tm_cmd);
  }
  
  //resume paused workout (part II - resume workout)
  if (resume_workout_after_play == 1 && workout_id == -1) {   //make sure play action is complete before resume
    //resume
    noInterrupts();
    workout_id = paused_workout_id;
    action_i = paused_action_i;
    start_time_ms = millis() - time_passed_for_pause_ms;  //start time = current time - pause time
    interrupts();
    
    paused_workout_id = -1;
    paused_action_i = 0;
    time_passed_for_pause_ms = 0;

    resume_workout_after_play = 0;
  }

  //test workouts
  if (workout_id == 0) {
    float intervals[] = { 0,.06,.24,.25,.25,.20 };
    uint8_t actions[] = { 2,  8,  8,  1,  1,  2 };    //using uint8_t (0 to 255) instead of int to reduce Global variable's memory usage (SRAM - max 2048 bytes only ie 2 KB)
    uint8_t clicks[]  = { 1, 10, 10, 10, 10, 30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //pause/play
  else if (workout_id == 1) {
    float intervals[] = { 0 };
    uint8_t actions[] = { 2 };
    uint8_t clicks[]  = { 1 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //stop
  else if (workout_id == 2) {
    float intervals[] = {  0 };
    uint8_t actions[] = {  2 };
    uint8_t clicks[]  = { 30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //20 min workout - level 1
  else if (workout_id == 61) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //20 min workout - level 2
  else if (workout_id == 62) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //20 min workout - level 3
  else if (workout_id == 63) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //20 min workout - level 4
  else if (workout_id == 64) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //20 min workout - level 5
  else if (workout_id == 65) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    40, 40, 40, 40, 50, 50, 50, 50, 50, 50, 50, 50,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //20 min workout - level 6
  else if (workout_id == 66) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    40, 40, 40, 40, 60, 60, 60, 60, 60, 60, 60, 60,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //20 min workout - level 7
  else if (workout_id == 67) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    40, 40, 40, 40, 60, 60, 60, 60, 70, 70, 70, 70,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //20 min workout - level 8
  else if (workout_id == 68) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    40, 40, 40, 40, 60, 60, 60, 60, 80, 80, 80, 80,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //24 min workout - level 1
  else if (workout_id == 81) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //24 min workout - level 2
  else if (workout_id == 82) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //24 min workout - level 3
  else if (workout_id == 83) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //24 min workout - level 4
  else if (workout_id == 84) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //24 min workout - level 5
  else if (workout_id == 85) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    40, 40, 40, 40, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //24 min workout - level 6
  else if (workout_id == 86) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    40, 40, 40, 40, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //24 min workout - level 7
  else if (workout_id == 87) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    40, 40, 40, 40, 60, 60, 60, 60, 70, 70, 70, 70, 70, 70, 70, 70,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //24 min workout - level 8
  else if (workout_id == 88) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    40, 40, 40, 40, 60, 60, 60, 60, 80, 80, 80, 80, 80, 80, 80, 80,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //28 min workout - level 1
  else if (workout_id == 101) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //28 min workout - level 2
  else if (workout_id == 102) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //28 min workout - level 3
  else if (workout_id == 103) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //28 min workout - level 4
  else if (workout_id == 104) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //28 min workout - level 5
  else if (workout_id == 105) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    40, 40, 40, 40, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //28 min workout - level 6
  else if (workout_id == 106) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    40, 40, 40, 40, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //28 min workout - level 7
  else if (workout_id == 107) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    40, 40, 40, 40, 60, 60, 60, 60, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //28 min workout - level 8
  else if (workout_id == 108) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    40, 40, 40, 40, 60, 60, 60, 60, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //32 min workout - level 1
  else if (workout_id == 121) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //32 min workout - level 2
  else if (workout_id == 122) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //32 min workout - level 3
  else if (workout_id == 123) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //32 min workout - level 4
  else if (workout_id == 124) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //32 min workout - level 5
  else if (workout_id == 125) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    40, 40, 40, 40, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //32 min workout - level 6
  else if (workout_id == 126) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    40, 40, 40, 40, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //32 min workout - level 7
  else if (workout_id == 127) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    40, 40, 40, 40, 60, 60, 60, 60, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
  //32 min workout - level 8
  else if (workout_id == 128) {
    float intervals[] = { 0, .06, 2.99,  2,     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2.95 };
    uint8_t actions[] = { 2,   8,    8,  1,     8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,  8,  1,    2 };
    uint8_t clicks[]  = { 1,  30,   20, 20,    40, 40, 40, 40, 60, 60, 60, 60, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80,   30 };
    workout(sizeof(intervals)/sizeof(intervals[0]), intervals, actions, clicks);
  }
}

void workout(int n, float intervals[], uint8_t actions[], uint8_t clicks[]) {
  extern long unsigned start_time_ms;
  extern int action_i;
  
  long unsigned current_time_ms = millis();
  if (action_i < n && current_time_ms - start_time_ms >= 61085 * intervals[action_i]) {      //Treadmill Display Timer has 1.085 sec (1085 ms) delay for every minute
    action(actions[action_i], clicks[action_i]);
    start_time_ms = millis();
    Serial.print("Action ");
    Serial.println(actions[action_i]);
    action_i++;
  }
  else if (action_i >= n) {
    workout_id = -1;
    action_i = 0;
    start_time_ms = 0;
  }
}

void action(int action, int no_of_clicks) {
  extern int global_action;
  extern int global_no_of_writes;

  global_action = action;
  global_no_of_writes = 15 + (5 * no_of_clicks);
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  extern int global_action;
  extern int global_no_of_writes;
  
  if (global_no_of_writes > 0) {
    Wire.write(global_action);          // respond with message of 1 byte
    global_no_of_writes--;
  }
  else {
    Wire.write(0);                      // respond with message of 1 byte
  }
}
