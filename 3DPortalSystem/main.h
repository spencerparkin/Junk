// main.h

// Programmed by Spencer T. Parkin

#ifndef __main_h__
#define __main_h__

extern float FPS;
extern bool level_complete;
extern bool cheat_mode;
extern bool draw_all;

// GLUT callback prototypes:
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void special(int key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void passive(int x, int y);
void idle(void);

// Ah, the fatal error routine.
void fatal_error(char *error_str, int exit_code = 0);

#endif __main_h__

// endof main.h