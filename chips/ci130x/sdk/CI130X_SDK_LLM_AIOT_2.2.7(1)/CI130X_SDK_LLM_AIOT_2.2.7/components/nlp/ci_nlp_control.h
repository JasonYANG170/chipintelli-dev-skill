// 输出nlp_delayd时间


#ifndef WAIT_NLP_NEXT_TIME
#define WAIT_NLP_NEXT_TIME                1800   //default exit wakeup time,unit ms
#endif



void nlp_timer_init(void);
void set_state_nlp_end(void);
void update_nlp_next_time(void);
