#pragma once

// interface to control device stuff

enum gpio_input {
    I_PB1,
    I_PB2,
    I_PB1_PB2
};

// interface to make a behaviour

struct behaviour {
    void (*Input)(gpio_input in);
    void (*Tick)(void);
    void (*DrawFrame)(void);
    void (*PreRender)(void);
    void (*PostRender)(void);
};

extern behaviour current_behaviour;

// default behaviour
extern behaviour default_behav;

void DefaultBehaviourInit(void);
void DefaultBehaviourDebug(const char* command);