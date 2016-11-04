Instructions to Run the Program:

gcc -o philosopher philosopher.c sem.c -std=c11 -pthread

gcc -o host host.c sem.c -std=c11 -pthread

./host <Number of Philosophers> <Number of Iterations>

(Please delete the temporary files at /tmp before rerunning the code).
