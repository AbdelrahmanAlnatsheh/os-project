#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip>

using namespace std;

void loadProcesses(const string& filename, vector<int>& ids, vector<int>& arrivals, vector<int>& bursts, int& contextSwitch, int& quantum) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening the file\n";
        exit(1);
    }
    int numProcesses;
    file >> numProcesses;
    ids.resize(numProcesses);
    arrivals.resize(numProcesses);
    bursts.resize(numProcesses);
    for (int i = 0; i < numProcesses; ++i) {
        file >> ids[i] >> arrivals[i] >> bursts[i];
    }
    file >> contextSwitch >> quantum;
    file.close();
}

void fcfs(const vector<int>& ids, const vector<int>& arrivals, const vector<int>& bursts, int contextSwitch) {
    int n = ids.size();
    vector<int> startTimes(n), finishTimes(n), waitingTimes(n), turnaroundTimes(n);
    int currentTime = 0;
    double totalWaitingTime = 0, totalTurnaroundTime = 0, totalActiveTime = 0;

    cout << "Gantt Chart for FCFS:" << endl;
    for (int i = 0; i < n; ++i) {
        if (currentTime < arrivals[i]) {
            currentTime = arrivals[i];
        }
        startTimes[i] = currentTime;
        currentTime += bursts[i];
        finishTimes[i] = currentTime;
        currentTime += contextSwitch; 

        waitingTimes[i] = startTimes[i] - arrivals[i];
        turnaroundTimes[i] = finishTimes[i] - arrivals[i];
        totalWaitingTime += waitingTimes[i];
        totalTurnaroundTime += turnaroundTimes[i];
        totalActiveTime += bursts[i];

        cout << "| P" << ids[i] << " (" << startTimes[i] << "-" << finishTimes[i] << ") ";
    }
    cout << "|" << endl;

    cout << "\nProcess Metrics:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << "Process " << ids[i] << ": Start Time = " << startTimes[i]
            << ", Finish Time = " << finishTimes[i]
            << ", Waiting Time = " << waitingTimes[i]
            << ", Turnaround Time = " << turnaroundTimes[i] << endl;
    }

    cout << "Average Waiting Time = " << totalWaitingTime / n << endl;
    cout << "Average Turnaround Time = " << totalTurnaroundTime / n << endl;

    
    double cpuUtilization = 100.0 * totalActiveTime / (currentTime - contextSwitch);
    cout << "CPU Utilization = " << fixed << setprecision(2) << cpuUtilization << "%" << endl;
}


void srt(vector<int> ids, vector<int> arrivals, vector<int> bursts, int contextSwitch) {
    int n = ids.size();
    vector<int> remaining(bursts.begin(), bursts.end());
    vector<int> startTimes(n, -1), finishTimes(n), waitingTimes(n), turnaroundTimes(n);
    vector<bool> completed(n, false);
    int currentTime = 0, completedProcesses = 0;
    double totalWaitingTime = 0, totalTurnaroundTime = 0, totalActiveTime = 0;

    cout << "Gantt Chart for SRT:" << endl;
    while (completedProcesses < n) {
        int shortest = -1;
        int minRemaining = numeric_limits<int>::max();
        for (int i = 0; i < n; ++i) {
            if (!completed[i] && arrivals[i] <= currentTime && remaining[i] < minRemaining) {
                minRemaining = remaining[i];
                shortest = i;
            }
        }

        if (shortest == -1) {
            currentTime++;
        }
        else {
            if (startTimes[shortest] == -1) { 
                startTimes[shortest] = currentTime;
            }
            currentTime++;
            remaining[shortest]--;

            if (remaining[shortest] == 0) {
                finishTimes[shortest] = currentTime;
                waitingTimes[shortest] = finishTimes[shortest] - bursts[shortest] - arrivals[shortest];
                turnaroundTimes[shortest] = finishTimes[shortest] - arrivals[shortest];
                totalWaitingTime += waitingTimes[shortest];
                totalTurnaroundTime += turnaroundTimes[shortest];
                totalActiveTime += bursts[shortest];
                completed[shortest] = true;
                completedProcesses++;
                currentTime += contextSwitch; 
            }

            cout << "| P" << ids[shortest] << " (" << currentTime - 1 << "-" << currentTime << ") ";
        }
    }
    cout << "|" << endl;

    
    double cpuUtilization = 100.0 * totalActiveTime / (currentTime - contextSwitch);

    cout << "Metrics for SRT:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << "Process " << ids[i]
            << ": Start Time = " << startTimes[i]
            << ", Finish Time = " << finishTimes[i]
            << ", Waiting Time = " << waitingTimes[i]
            << ", Turnaround Time = " << turnaroundTimes[i] << endl;
    }
    cout << "Average Waiting Time = " << totalWaitingTime / n << endl;
    cout << "Average Turnaround Time = " << totalTurnaroundTime / n << endl;
    cout << "CPU Utilization = " << fixed << setprecision(2) << cpuUtilization << "%" << endl;
}
void roundRobin(vector<int> ids, vector<int> arrivals, vector<int> bursts, int quantum, int contextSwitch) {
    int n = ids.size();
    vector<int> remaining(bursts.begin(), bursts.end());
    vector<int> startTimes(n, -1), finishTimes(n), waitingTimes(n), turnaroundTimes(n);
    queue<int> q;
    int currentTime = 0;
    double totalWaitingTime = 0, totalTurnaroundTime = 0;

    
    cout << "Gantt Chart for Round Robin:" << endl;

    
    int lastTime = 0; 
    while (true) {
        bool allDone = true;
        for (int i = 0; i < n; ++i) {
            if (arrivals[i] <= currentTime && remaining[i] > 0) {
                allDone = false;
                if (startTimes[i] == -1) startTimes[i] = currentTime;
                if (!q.empty() && q.front() == i) continue;
                q.push(i);
            }
        }

        if (allDone && q.empty()) break;

        if (!q.empty()) {
            int idx = q.front();
            q.pop();

            
            cout << "| P" << ids[idx] << " (" << currentTime << ") ";

            
            int execTime = min(quantum, remaining[idx]);
            remaining[idx] -= execTime;
            lastTime = currentTime + execTime; 
            currentTime += execTime;


            if (remaining[idx] == 0) {
                finishTimes[idx] = currentTime;
                turnaroundTimes[idx] = finishTimes[idx] - arrivals[idx];
                waitingTimes[idx] = turnaroundTimes[idx] - bursts[idx];
                totalWaitingTime += waitingTimes[idx];
                totalTurnaroundTime += turnaroundTimes[idx];
            }
            else {
                q.push(idx);
            }
            currentTime += contextSwitch; 
        }
    }
    cout << "|" << endl;

   
    cout << "\nMetrics for Round Robin:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << "Process " << ids[i] << ": Finish Time = " << finishTimes[i]
            << ", Waiting Time = " << waitingTimes[i]
            << ", Turnaround Time = " << turnaroundTimes[i] << endl;
    }

    
    cout << "Average Waiting Time = " << totalWaitingTime / n << endl;
    cout << "Average Turnaround Time = " << totalTurnaroundTime / n << endl;
    double cpuUtilization = 100.0 * (lastTime - contextSwitch * (n - 1)) / lastTime;
    cout << "CPU Utilization = " << fixed << setprecision(2) << cpuUtilization << "%" << endl;
}

int main() {
    vector<int> ids, arrivals, bursts;
    int contextSwitch, quantum;
    string filename = "C:/sk-system/processes.txt"; 

    loadProcesses(filename, ids, arrivals, bursts, contextSwitch, quantum);

    cout << "First-Come First-Served Scheduling:\n";
    fcfs(ids, arrivals, bursts, contextSwitch);
    cout << endl;

    cout << "Shortest Remaining Time Scheduling:\n";
    srt(ids, arrivals, bursts, contextSwitch);
    cout << endl;

    cout << "Round Robin Scheduling:\n";
    roundRobin(ids, arrivals, bursts, quantum, contextSwitch);
    cout << endl;

    return 0;
}