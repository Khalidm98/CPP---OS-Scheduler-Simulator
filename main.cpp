#include <vector>
#include <string>
#include <iostream>
#include "Schedule.h"
using namespace std;

int main(){
	while (1){
		string algorithm;							// scheduling algorithm
		getline(cin, algorithm);
		if (algorithm == "q" || algorithm == "Q")
			return 0;

		unsigned short N;							// number of processes
		cin >> N;

		vector<float> burst(N);						// burst time array
		for (unsigned short i = 0; i < N; ++i)
			cin >> burst[i];
		
		vector<float> arrival(N);					// arrival time array
		for (unsigned short i = 0; i < N; ++i)
			cin >> arrival[i];

		float avg_wait = 0;							// average waiting time
		vector<float> time_line;					// execution time line
		vector<unsigned short> execution;			// processes execution order

		// first come first serve
		if (algorithm == "FCFS"){
			Schedule instance(N, &burst[0], &arrival[0]);
			avg_wait = instance.fcfs(time_line, execution);
		}
		// shortest job first non-preemptive
		else if (algorithm == "SJF_N"){
			Schedule instance(N, &burst[0], &arrival[0]);
			avg_wait = instance.sjf(time_line, execution, 0);
		}
		// shortest job first preemptive
		else if (algorithm == "SJF_P"){
			Schedule instance(N, &burst[0], &arrival[0]);
			avg_wait = instance.sjf(time_line, execution);
		}
		// priority non-preemptive
		else if (algorithm == "PRIOR_N"){
			vector<unsigned short> priority(N);				// priority array
			for (unsigned short i = 0; i < N; ++i)
				cin >> priority[i];

			Schedule instance(N, &burst[0], &arrival[0], &priority[0]);
			avg_wait = instance.prior(time_line, execution, 0);
		}
		// priority preemptive
		else if (algorithm == "PRIOR_P"){
			vector<unsigned short> priority(N);				// priority array
			for (unsigned short i = 0; i < N; ++i)
				cin >> priority[i];

			Schedule instance(N, &burst[0], &arrival[0], &priority[0]);
			avg_wait = instance.prior(time_line, execution);
		}
		// round robin
		else if (algorithm == "RR"){
			unsigned short quantum;							// quantum time
			cin >> quantum;

			Schedule instance(N, &burst[0], &arrival[0], quantum);
			avg_wait = instance.rr(time_line, execution);
		}
		else
			continue;

		// validation
		for (unsigned short i = 0; i < execution.size(); ++i){
			cout << "[" << time_line[i] << "]  ";
			if (execution[i] == 0)
				cout << "idle  ";
			else
				cout << "P" << execution[i] << "  ";
		}
		cout << "[" << time_line.back() << "]\n";
		cout << "Average waiting time = " << avg_wait << endl << endl;
		cin.ignore();
	}
}
