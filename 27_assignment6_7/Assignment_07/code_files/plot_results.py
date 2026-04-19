#!/usr/bin/env python3
"""
plot_results.py
───────────────
Reads benchmark_results.csv produced by run_benchmark.sh and generates
two sets of plots for each of the 5 configurations:
  • Execution Time  vs Number of Cores
  • Speedup         vs Number of Cores  (with ideal-speedup reference line)

All 10 sub-plots are arranged on a single A4-landscape figure and also
saved individually for easier inclusion in a report.
"""

import csv
import os
import sys
from collections import defaultdict

import matplotlib
matplotlib.use("Agg")           # non-interactive backend (works without display)
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import numpy as np

# ── Paths ────────────────────────────────────────────────────────────────────
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
CSV_FILE   = os.path.join(SCRIPT_DIR, "benchmark_results.csv")
OUT_DIR    = os.path.join(SCRIPT_DIR, "plots")
os.makedirs(OUT_DIR, exist_ok=True)

# ── Colour palette (one per config) ─────────────────────────────────────────
PALETTE = ["#E63946", "#2A9D8F", "#E9C46A", "#457B9D", "#6A0572"]
IDEAL_COLOR = "#AAAAAA"

# ── Read CSV ─────────────────────────────────────────────────────────────────
data = defaultdict(dict)   # data[config_label][threads] = time_sec

if not os.path.isfile(CSV_FILE):
    print(f"ERROR: {CSV_FILE} not found. Run run_benchmark.sh first.", file=sys.stderr)
    sys.exit(1)

config_order = []
with open(CSV_FILE, newline="") as f:
    reader = csv.DictReader(f)
    for row in reader:
        label   = row["config"]
        threads = int(row["threads"])
        time    = float(row["time_sec"])
        data[label][threads] = time
        if label not in config_order:
            config_order.append(label)

if not config_order:
    print("ERROR: CSV file is empty or has no data rows.", file=sys.stderr)
    sys.exit(1)

n_configs = len(config_order)

# ── Derived human-readable labels ────────────────────────────────────────────
def pretty_label(raw):
    """Turn 'Config1_250x100_0.9M' → 'Config 1\n250×100, 0.9M pts'"""
    parts = raw.replace("Config", "").split("_", 2)
    idx   = parts[0] if parts else "?"
    rest  = parts[1:]
    dims  = rest[0] if len(rest) > 0 else ""
    pts   = rest[1] if len(rest) > 1 else ""
    dims  = dims.replace("x", "×")
    pts   = pts.replace("M", "M pts")
    return f"Config {idx}\n{dims}, {pts}"

# ── Thread lists ──────────────────────────────────────────────────────────────
ALL_THREADS  = sorted({t for cfg in data.values() for t in cfg.keys()})
PAR_THREADS  = [t for t in ALL_THREADS if t > 1]   # 2,4,8,16

# ── Helper: build speedup series ─────────────────────────────────────────────
def speedup_series(cfg_data):
    t_serial = cfg_data.get(1, None)
    if t_serial is None or t_serial == 0:
        return [], []
    threads = sorted([t for t in cfg_data if t > 0])
    sp = [t_serial / cfg_data[t] for t in threads if cfg_data.get(t, 0) > 0]
    th = [t for t in threads if cfg_data.get(t, 0) > 0]
    return th, sp

# ═══════════════════════════════════════════════════════════════════════════════
#  FIGURE 1: Combined overview (2 rows × n_configs columns)
# ═══════════════════════════════════════════════════════════════════════════════
fig, axes = plt.subplots(
    2, n_configs,
    figsize=(4.5 * n_configs, 8),
    constrained_layout=True
)
if n_configs == 1:
    axes = axes.reshape(2, 1)

fig.suptitle(
    "HPC Assignment 07 – OpenMP Parallel PIC Performance\n"
    "Execution Time & Speedup vs Number of Cores",
    fontsize=14, fontweight="bold", y=1.01
)

for ci, label in enumerate(config_order):
    cfg_data = data[label]
    color    = PALETTE[ci % len(PALETTE)]
    plabel   = pretty_label(label)

    # ── Execution time plot ──────────────────────────────────────
    ax_time = axes[0, ci]
    sorted_threads = sorted(cfg_data.keys())
    times          = [cfg_data[t] for t in sorted_threads]

    ax_time.plot(sorted_threads, times, "o-", color=color,
                 linewidth=2, markersize=7, markerfacecolor="white",
                 markeredgewidth=2, zorder=5)

    # Annotate values
    for t, v in zip(sorted_threads, times):
        ax_time.annotate(f"{v:.2f}s", xy=(t, v),
                         xytext=(0, 8), textcoords="offset points",
                         ha="center", fontsize=7, color=color)

    ax_time.set_title(plabel, fontsize=9, fontweight="bold")
    ax_time.set_xlabel("Cores", fontsize=8)
    ax_time.set_ylabel("Time (s)", fontsize=8)
    ax_time.set_xticks(sorted_threads)
    ax_time.tick_params(labelsize=7)
    ax_time.grid(True, linestyle="--", alpha=0.4)
    ax_time.set_facecolor("#F9F9F9")

    # ── Speedup plot ─────────────────────────────────────────────
    ax_sp = axes[1, ci]
    th, sp = speedup_series(cfg_data)

    if th:
        max_t = max(th)
        ideal_x = np.array([1] + PAR_THREADS)
        ideal_y = ideal_x.astype(float)
        ax_sp.plot(ideal_x, ideal_y, "--", color=IDEAL_COLOR,
                   linewidth=1.5, label="Ideal", zorder=3)
        ax_sp.plot(th, sp, "s-", color=color,
                   linewidth=2, markersize=7, markerfacecolor="white",
                   markeredgewidth=2, zorder=5, label="Measured")

        for t, s in zip(th, sp):
            ax_sp.annotate(f"{s:.2f}×", xy=(t, s),
                           xytext=(0, 8), textcoords="offset points",
                           ha="center", fontsize=7, color=color)

        ax_sp.legend(fontsize=7)

    ax_sp.set_xlabel("Cores", fontsize=8)
    ax_sp.set_ylabel("Speedup", fontsize=8)
    ax_sp.set_xticks(sorted(set(th + [1])) if th else [1])
    ax_sp.tick_params(labelsize=7)
    ax_sp.grid(True, linestyle="--", alpha=0.4)
    ax_sp.set_facecolor("#F9F9F9")

combined_path = os.path.join(OUT_DIR, "all_configs_combined.png")
fig.savefig(combined_path, dpi=150, bbox_inches="tight")
plt.close(fig)
print(f"Saved: {combined_path}")

# ═══════════════════════════════════════════════════════════════════════════════
#  FIGURE 2: Individual per-config plots (saved separately)
# ═══════════════════════════════════════════════════════════════════════════════
for ci, label in enumerate(config_order):
    cfg_data = data[label]
    color    = PALETTE[ci % len(PALETTE)]
    plabel   = pretty_label(label)

    fig2, (ax_t, ax_s) = plt.subplots(1, 2, figsize=(10, 4), constrained_layout=True)
    fig2.suptitle(f"Performance – {plabel.replace(chr(10), ' | ')}",
                  fontsize=12, fontweight="bold")

    sorted_threads = sorted(cfg_data.keys())
    times = [cfg_data[t] for t in sorted_threads]

    # Time
    ax_t.plot(sorted_threads, times, "o-", color=color,
              linewidth=2.5, markersize=9, markerfacecolor="white",
              markeredgewidth=2.5, zorder=5)
    for t, v in zip(sorted_threads, times):
        ax_t.annotate(f"{v:.3f}s", xy=(t, v),
                      xytext=(0, 10), textcoords="offset points",
                      ha="center", fontsize=8.5, color=color)
    ax_t.set_title("Execution Time vs Cores", fontsize=10, fontweight="bold")
    ax_t.set_xlabel("Number of Cores", fontsize=9)
    ax_t.set_ylabel("Execution Time (seconds)", fontsize=9)
    ax_t.set_xticks(sorted_threads)
    ax_t.grid(True, linestyle="--", alpha=0.4)
    ax_t.set_facecolor("#F7F7F7")

    # Speedup
    th, sp = speedup_series(cfg_data)
    if th:
        ideal_x = np.array([1] + PAR_THREADS)
        ax_s.plot(ideal_x, ideal_x.astype(float), "--",
                  color=IDEAL_COLOR, linewidth=1.8, label="Ideal Speedup")
        ax_s.plot(th, sp, "s-", color=color,
                  linewidth=2.5, markersize=9, markerfacecolor="white",
                  markeredgewidth=2.5, zorder=5, label="Measured Speedup")
        for t, s in zip(th, sp):
            ax_s.annotate(f"{s:.2f}×", xy=(t, s),
                          xytext=(0, 10), textcoords="offset points",
                          ha="center", fontsize=8.5, color=color)
        ax_s.legend(fontsize=9)
    ax_s.set_title("Speedup vs Cores", fontsize=10, fontweight="bold")
    ax_s.set_xlabel("Number of Cores", fontsize=9)
    ax_s.set_ylabel("Speedup", fontsize=9)
    ax_s.set_xticks(sorted(set(th + [1])) if th else [1])
    ax_s.grid(True, linestyle="--", alpha=0.4)
    ax_s.set_facecolor("#F7F7F7")

    out_path = os.path.join(OUT_DIR, f"config{ci+1}_{label.split('_')[0]}.png")
    fig2.savefig(out_path, dpi=150, bbox_inches="tight")
    plt.close(fig2)
    print(f"Saved: {out_path}")

# ═══════════════════════════════════════════════════════════════════════════════
#  FIGURE 3: Overlay – all configs on same axes (useful for comparison)
# ═══════════════════════════════════════════════════════════════════════════════
fig3, (ax_ot, ax_os) = plt.subplots(1, 2, figsize=(13, 5), constrained_layout=True)
fig3.suptitle("All Configurations – Comparison Overview",
              fontsize=13, fontweight="bold")

for ci, label in enumerate(config_order):
    cfg_data = data[label]
    color    = PALETTE[ci % len(PALETTE)]
    short    = pretty_label(label).replace("\n", " ")

    sorted_threads = sorted(cfg_data.keys())
    times = [cfg_data[t] for t in sorted_threads]
    ax_ot.plot(sorted_threads, times, "o-", color=color,
               linewidth=2, markersize=6, label=short)

    th, sp = speedup_series(cfg_data)
    if th:
        ax_os.plot(th, sp, "s-", color=color,
                   linewidth=2, markersize=6, label=short)

# Ideal on speedup
if PAR_THREADS:
    ideal_x = np.array([1] + PAR_THREADS)
    ax_os.plot(ideal_x, ideal_x.astype(float), "--",
               color=IDEAL_COLOR, linewidth=1.5, label="Ideal")

for ax, ylabel, title in [
    (ax_ot, "Execution Time (s)", "Execution Time vs Cores"),
    (ax_os, "Speedup",            "Speedup vs Cores"),
]:
    ax.set_xlabel("Number of Cores", fontsize=10)
    ax.set_ylabel(ylabel, fontsize=10)
    ax.set_title(title, fontsize=11, fontweight="bold")
    ax.legend(fontsize=8, loc="upper right")
    ax.grid(True, linestyle="--", alpha=0.4)
    ax.set_facecolor("#F7F7F7")

    all_t = sorted({t for cfg in data.values() for t in cfg})
    ax.set_xticks(all_t)

overview_path = os.path.join(OUT_DIR, "overview_all_configs.png")
fig3.savefig(overview_path, dpi=150, bbox_inches="tight")
plt.close(fig3)
print(f"Saved: {overview_path}")

print("\n✓ All plots saved to:", OUT_DIR)
