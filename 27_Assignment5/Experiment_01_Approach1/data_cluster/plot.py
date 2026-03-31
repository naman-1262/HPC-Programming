import matplotlib.pyplot as plt
import os

# ── CHANGE THESE FILE NAMES ONLY ─────────────────────────────────────────────
files = [
    "cluster_serial_deffered_config_1.txt",
    "cluster_serial_deffered_config_2.txt",
    "cluster_serial_deffered_config_3.txt",
]

labels = ["Config 1", "Config 2", "Config 3"]
plot_title = "Cluster Data – Particles vs Total Time"
# ─────────────────────────────────────────────────────────────────────────────

# This makes sure files are always found relative to plot.py's location
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

def read_file(filename):
    particles, times = [], []
    filepath = os.path.join(BASE_DIR, filename)   # ← fix is here
    with open(filepath, "r") as f:
        for line in f:
            parts = line.strip().split()
            if len(parts) == 2:
                try:
                    particles.append(float(parts[0]))
                    times.append(float(parts[1]))
                except ValueError:
                    continue  # skip header
    return particles, times

colors  = ["tab:blue", "tab:orange", "tab:green"]
markers = ["o", "s", "^"]

fig, axes = plt.subplots(1, 3, figsize=(16, 5))
fig.suptitle(plot_title, fontsize=14, fontweight="bold")

for i, (filename, label) in enumerate(zip(files, labels)):
    particles, times = read_file(filename)
    ax = axes[i]
    ax.plot(particles, times, marker=markers[i], color=colors[i], linewidth=2)
    ax.set_xscale("log")
    ax.set_yscale("log")
    ax.set_title(label, fontsize=12)
    ax.set_xlabel("Number of Particles", fontsize=11)
    ax.set_ylabel("Total Time (s)", fontsize=11)
    ax.grid(True, which="both", linestyle="--", alpha=0.6)

plt.tight_layout()
plt.savefig(os.path.join(BASE_DIR, "output_plot.png"), dpi=150, bbox_inches="tight")
plt.show()