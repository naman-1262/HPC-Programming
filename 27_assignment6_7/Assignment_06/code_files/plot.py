import matplotlib.pyplot as plt

threads = [1,2,4,8,16]
times = [3.625616,1.660862,1.007260,0.917742,0.793877]
speedup = [times[0]/t for t in times]
efficiency = [speedup[i]/threads[i] for i in range(len(threads))]

plt.figure(figsize=(6,4))
plt.plot(threads, times, marker='o', linewidth=2)
plt.xlabel('Number of Threads')
plt.ylabel('Execution Time (s)')
plt.title('Time vs Threads (1000x400)')
plt.grid(True)
plt.tight_layout()
plt.savefig('time_vs_threads_1000x400.png', dpi=300)

plt.figure(figsize=(6,4))
plt.plot(threads, speedup, marker='o', linewidth=2)
plt.xlabel('Number of Threads')
plt.ylabel('Speedup')
plt.title('Speedup vs Threads (1000x400)')
plt.grid(True)
plt.tight_layout()
plt.savefig('speedup_vs_threads_1000x400.png', dpi=300)

plt.figure(figsize=(6,4))
plt.plot(threads, efficiency, marker='o', linewidth=2)
plt.xlabel('Number of Threads')
plt.ylabel('Efficiency')
plt.title('Efficiency vs Threads (1000x400)')
plt.grid(True)
plt.tight_layout()
plt.savefig('efficiency_vs_threads_1000x400.png', dpi=300)

plt.show()
