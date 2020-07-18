#include <vector>
using namespace std;

#ifndef SCHEDULE_H
#define SCHEDULE_H

class Schedule {
private:
	unsigned short N;							// number of processes
	float *burst;								// pointer to burst time array
	float *arrival;								// pointer to arrival time array
	unsigned short *priority;					// pointer to priority array
	unsigned short q;							// quantum time

	vector<unsigned short> process;				// processes order array
	vector<float> remain;						// remaining time array
    vector<unsigned short> prio_cpy;            // copy of priority array

	void start_time(vector<float> &time_line, vector<unsigned short> &execution);
	void calc_active_next(unsigned short &active, float &next, const vector<float> &time_line);
	template <typename FUS>
    unsigned short min_index(const FUS *const ptr, const unsigned short size);
	template <typename FUS>
    void execute(vector<float> &time_line, const unsigned short index, const float next, FUS *const ptr);
	template <typename FUS>
    void execute_all(vector<float> &time_line, vector<unsigned short> &execution, const unsigned short index, FUS *const ptr);
	void organize_timeline(vector<float> &time_line, vector<unsigned short> &execution, unsigned short size);
	float calc_avg_wait(const vector<float> &time_line, const vector<unsigned short> &execution, const unsigned short size);

public:
    Schedule(unsigned short N, float *B, float *A);
    Schedule(unsigned short N, float *B, float *A, unsigned short *P);
    Schedule(unsigned short N, float *B, float *A, unsigned short Q);
	float fcfs(vector<float> &time_line, vector<unsigned short> &execution);
	float sjf(vector<float> &time_line, vector<unsigned short> &execution, bool preemptive = 1);
    float prior(vector<float> &time_line, vector<unsigned short> &execution, bool preemptive = 1);
    float rr(vector<float> &time_line, vector<unsigned short> &execution);
};

#endif // SCHEDULE_H
