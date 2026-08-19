// 输出nlp_delayd时间


#ifndef WAIT_NLP_NEXT_TIME
#define WAIT_NLP_NEXT_TIME               4   //设置相邻两个意图之间等待时长档位数 0~6档 7档可调 值越大，响应越慢,若长词漏意图，可适当调大
#endif

#ifndef NLP_NEXT_DELAY_TIME
#define NLP_NEXT_DELAY_TIME              3   //设置相邻输出超时响应时长档位数 0~6档 值越大，响应越慢
#endif

void nlp_timer_init(void);
void set_state_nlp_end(void);
void update_nlp_next_time(int wait_time);
int get_wait_time();


void nlp_delay_init(void);
void update_nlp_delay_time(int wait_time);
void set_nlp_delay_end(void);
int get_delay_time();