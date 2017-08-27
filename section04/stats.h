#ifndef SVR_CORE_STATS_H_
#define SVR_CORE_STATS_H_

#define STATS_SHM_KEY 0x78022468
struct stats {
  int msg_cnt;
  int last_cnt;
  int conn_cnt;
  int64_t start_time;
  int64_t last_time;
};


#endif // SVR_CORE_STATS_H_
