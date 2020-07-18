#include <float.h>
#include "Schedule.h"

// FCFS & SJF constructors
Schedule::Schedule(unsigned short N, float *B, float *A)
: N(N), burst(B), arrival(A), priority(NULL), q(0), process(N), remain(N) {}

// Priority constructor
Schedule::Schedule(unsigned short N, float *B, float *A, unsigned short *P)
: N(N), burst(B), arrival(A), priority(P), q(0), process(N), remain(N), prio_cpy(N) {}

// RR constructor
Schedule::Schedule(unsigned short N, float *B, float *A, unsigned short Q)
: N(N), burst(B), arrival(A), priority(NULL), q(Q), process(N), remain(N) {}

// get things ready and start the time
void Schedule::start_time(vector<float> &time_line, vector<unsigned short> &execution){
	// copy the burst time array
	for (unsigned short i = 0; i < N; ++i)
		remain[i] = burst[i];

	// sort the processes by arrival time
	// copy the arrival time array
	vector<float> vect(N);
	for (unsigned short i = 0; i < N; ++i)
		vect[i] = arrival[i];
	// sort ascending
	for (unsigned short i = 0; i < N; ++i){
		process[i] = 0;
		for (unsigned short j = 1; j < N; ++j)
			if (vect[j] < vect[process[i]])
				process[i] = j;
		vect[process[i]] = FLT_MAX;
	}

	// set starting time = 0
	if (arrival[process[0]] != 0){
		execution.push_back(0);
		time_line.push_back(0);
	}
	time_line.push_back(arrival[process[0]]);
}

// to calculate active processes number and next arrival time
void Schedule::calc_active_next(unsigned short &active, float &next, const vector<float> &time_line){
	active = 0;
	for (unsigned short i = 0; i < process.size(); ++i){
		if (arrival[process[i]] <= time_line.back())
			++active;
		else {
			next = arrival[process[i]];
			return;
		}
	}
}

// to get index of the minimum element in a vector (after sorting)
template <typename FUS>
unsigned short Schedule::min_index(const FUS *const ptr, const unsigned short size){
	unsigned short index = 0;
	for (unsigned short i = 1; i < size; ++i)
		if (ptr[process[i]] < ptr[process[index]])
			index = i;
	return index;
}

// execute processes before final arrival
template <typename FUS>
void Schedule::execute(vector<float> &time_line, const unsigned short index, const float next, FUS *const ptr){
	// if process terminates before next arrival
	if (remain[process[index]] <= next - time_line.back()){
		time_line.push_back(time_line.back() + remain[process[index]]);
		if (sizeof(*ptr) == sizeof(float))
			remain[process[index]] = FLT_MAX;
		else
			prio_cpy[process[index]] = 0xFFFF;
		process.erase(process.begin() + index);
	}
	// if next arrival comes before termination
	else {
		remain[process[index]] -= next - time_line.back();
		time_line.push_back(next);
	}
}

// execute processes after final arrival
template <typename FUS>
void Schedule::execute_all(vector<float> &time_line, vector<unsigned short> &execution, const unsigned short index, FUS *const ptr){
	time_line.push_back(time_line.back() + remain[process[index]]);
    execution.push_back(process[index] + 1);
    if (sizeof(*ptr) == sizeof(float))
		remain[process[index]] = FLT_MAX;
	else
		prio_cpy[process[index]] = 0xFFFF;
}

// to organize the time line
void Schedule::organize_timeline(vector<float> &time_line, vector<unsigned short> &execution, unsigned short size){
	unsigned short i = 0;
	while (i < size - 1){
		if (execution[i] == execution[i+1]){
			execution.erase(execution.begin() + i+1);
			time_line.erase(time_line.begin() + i+1);
			--size;
		}
		else
			++i;
	}
}

// to calculate the average waiting time
float Schedule::calc_avg_wait(const vector<float> &time_line, const vector<unsigned short> &execution, const unsigned short size){
	float avg_wait = 0;
	for (unsigned short i = 0; i < N; ++i){
		for (unsigned short j = size-1; j >= 0; --j)
			if (execution[j] == i+1){
                avg_wait += time_line[j+1] - burst[i] - arrival[i];
                break;
			}
	}
	return (avg_wait / N);
}

// First Come First Serve
float Schedule::fcfs(vector<float> &time_line, vector<unsigned short> &execution){
	start_time(time_line, execution);

	for (unsigned short i = 0; i < N; ++i){
		execution.push_back(process[i] + 1);
		time_line.push_back(time_line.back() + burst[process[i]]);
	}

	return calc_avg_wait(time_line, execution, execution.size());
}

// Shortest Job First
float Schedule::sjf(vector<float> &time_line, vector<unsigned short> &execution, bool preemptive){
	start_time(time_line, execution);

	// if not all processes had arrived
	while (time_line.back() < arrival[process.back()]){
		float next;									// next arrival time
		unsigned short active;						// active processes number
		calc_active_next(active, next, time_line);

		// if there is no active processes
		if (active == 0){
			execution.push_back(0);
			time_line.push_back(next);
		}
		// if there are some active processes
		else {
			// get shortest remaining time
			unsigned short min_remain = min_index(&remain[0], active);
			execution.push_back(process[min_remain] + 1);
			if (preemptive)
				execute(time_line, min_remain, next, &remain[0]);
			else
				execute(time_line, min_remain, time_line.back() + remain[process[min_remain]], &remain[0]);
		}
	}
	// when all processes arrive
	for (unsigned short i = 0; i < process.size(); ++i){
        unsigned short min_remain = min_index(&remain[0], process.size());
        execute_all(time_line, execution, min_remain, &remain[0]);
	}

	organize_timeline(time_line, execution, execution.size());
	return calc_avg_wait(time_line, execution, execution.size());
}

// Priority
float Schedule::prior(vector<float> &time_line, vector<unsigned short> &execution, bool preemptive){
	if (priority == NULL)
		return -1;

	start_time(time_line, execution);
	// copy the priority array
	for (unsigned short i = 0; i < N; ++i)
		prio_cpy[i] = priority[i];

	// if not all processes had arrived
	while (time_line.back() < arrival[process.back()]){
		float next;									// next arrival time
		unsigned short active;						// active processes number
		calc_active_next(active, next, time_line);

		// if there is no active processes
		if (active == 0){
			execution.push_back(0);
			time_line.push_back(next);
		}
		// if there are some active processes
		else {
			// get highest priority
			unsigned short max_prior = min_index(&prio_cpy[0],active);
			execution.push_back(process[max_prior] + 1);
			if (preemptive)
				execute(time_line, max_prior, next, &prio_cpy[0]);
			else
				execute(time_line, max_prior, time_line.back() + remain[process[max_prior]], &prio_cpy[0]);
		}
	}
	// when all processes arrive
	for (unsigned short i = 0; i < process.size(); ++i){
        unsigned short max_prior = min_index(&prio_cpy[0],process.size());
        execute_all(time_line, execution, max_prior, &prio_cpy[0]);
	}

	organize_timeline(time_line, execution, execution.size());
	return calc_avg_wait(time_line, execution, execution.size());
}

// Round Robin
float Schedule::rr(vector<float> &time_line, vector<unsigned short> &execution){
	if (q == 0)
		return -1;

	start_time(time_line, execution);

	// if not all processes had arrived
	unsigned short index = 0;						// index of process to be executed
	while (time_line.back() < arrival[process.back()]){
		float next;									// next arrival time
		unsigned short active = 0;					// active processes number
		calc_active_next(active, next, time_line);

		// if there is no active processes
		if (active == 0){
			execution.push_back(0);
			time_line.push_back(next);
		}
		// if there are some active processes
		else {
			// before the arrival of the next process
			while (time_line.back() < next){
				// if the selected process had arrived
				if (arrival[process[index]] <= time_line.back()){
					execution.push_back(process[index] + 1);
					// if process terminates before quantum time ends
					if (remain[process[index]] <= q){
						time_line.push_back(time_line.back() + remain[process[index]]);
						process.erase(process.begin() + index);
						--active;
						if (active == 0)
							break;
					}
					// if quantum time ends before termination
					else {
						time_line.push_back(time_line.back() + q);
						remain[process[index]] -= q;
						++index;
					}
				}
				// if the selected process didn't arrive
				else
					index = 0;
			}
		}
	}
	// when all processes arrive
	while (1){
		execution.push_back(process[index] + 1);
		// if process terminates before quantum time ends
		if (remain[process[index]] <= q){
			time_line.push_back(time_line.back() + remain[process[index]]);
			process.erase(process.begin() + index);
			if (process.empty())
				break;
            if (index == process.size())
                index = 0;
		}
		// if quantum time ends before termination
		else {
			time_line.push_back(time_line.back() + q);
			remain[process[index]] -= q;
			index = (index + 1) % process.size();
		}
	}

	organize_timeline(time_line, execution, execution.size());
	return calc_avg_wait(time_line, execution, execution.size());
}
