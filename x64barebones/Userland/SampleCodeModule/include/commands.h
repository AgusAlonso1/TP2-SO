#ifndef COMMANDS_H
#define COMMANDS_H

#include <syscallFunctions.h>
#include <libc.h>
#include <snake.h>
#include <themes.h>


void man();

void registers();

void time();

void div0();

void invalidOp();

void clear();

void zoomIn();

void zoomOut();

int loop(int argc, char** argv);

int ps(int argc, char** argv);

int kill(int argc, char** argv);

int nice(int argc, char** argv);

int block(int argc, char** argv);

int minfo(int argc, char** argv);

int cat(int argc, char** argv);

int wc(int argc, char** argv);

int filter(int argc, char** argv);

#endif
