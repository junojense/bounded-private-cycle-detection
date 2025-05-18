import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns


def plot_and_export(ax, filename_prefix):
    # Export to SVG and EPS formats
    for ext in ['svg', 'eps']:
        ax.figure.savefig(f"{filename_prefix}.{ext}", format=ext)
    plt.close(ax.figure)


# Load data
logfile = 'filename.log'
if logfile is 'filename.log':
    print("Please set the logfile variable to a valid .log file")
    exit(1);
df = pd.read_csv(logfile)

# Calculate additional columns
df['n_for_echo'] = df['n_for'] + df['n_echo']
df['c_edge_avg'] = df['c_edge'] / df['n']
df['msg_avg'] = df['n_msg'] / df['n']
df['for_avg'] = df['n_for'] / df['n']
df['echo_avg'] = df['n_echo'] / df['n']
df['pub_avg'] = df['n_pub'] / df['n']
df['brd_avg'] = df['n_brd'] / df['n']
df['t_avg'] = df['t'] / df['n']
df['for_echo_avg'] = df['n_for_echo'] / df['n']
df['m_param'] = df['m'] / 49  # for n = 50

# Filter data for l values of 2, 3, 4
df_filtered = df[df['l'].isin([2, 3, 4])]

# Define color palette and line styles
palette = sns.color_palette("colorblind", 3)
line_styles = ['-', '--', '-.']
marker_special = '+'

# Set up font size parameters for readability in LaTeX
font_size = 14
legend_font_size = 12

# Plot 1: Degree vs Average Runtime
fig, ax1 = plt.subplots(figsize=(7, 5))
for pl, color, line_style in zip([2, 3, 4], palette, line_styles):
    dfl = df_filtered[df_filtered['l'] == pl]
    ax1.scatter(dfl['m_param'], dfl['t_avg'], label=f'l={[pl]}', color=color, linestyle=line_style, alpha=0.7, zorder=3)
ax1.set_xlabel(r'growth rate $\overline{m}$', fontsize=font_size)
ax1.set_ylabel(r'avg runtime $t$ ($s$)', fontsize=font_size)
ax1.set_yscale('log')
ax1.legend(fontsize=legend_font_size)
ax1.tick_params(axis='both', which='major', labelsize=font_size)
ax1.grid(True, zorder=1)  # Lower zorder for grid
plt.tight_layout()
plot_and_export(ax1, 'degree-vs-avg-runtime')

# Plot 2: Degree vs Number of Cycles
fig, ax2 = plt.subplots(figsize=(7, 5))
for pl, color, line_style in zip([2, 3, 4], palette, line_styles):
    dfl = df_filtered[df_filtered['l'] == pl]
    ax2.scatter(dfl['m_param'], dfl['n_cyc'], label=f'l={pl}', color=color, linestyle=line_style, alpha=0.7, zorder=3)
ax2.set_xlabel(r'growth rate $\overline{m}$', fontsize=font_size)
ax2.set_ylabel(r'cycle count $c$ (max length $\ell$)', fontsize=font_size)
ax2.set_yscale('log')
ax2.legend(fontsize=legend_font_size)
ax2.tick_params(axis='both', which='major', labelsize=font_size)
ax2.grid(True, zorder=1)  # Lower zorder for grid
plt.tight_layout()
plot_and_export(ax2, 'degree-vs-num-cycles')

# Plot 3: Degree vs Average Sum of Total Forward & Echo Messages
fig, ax3 = plt.subplots(figsize=(7, 5))
for pl, color, line_style in zip([2, 3, 4], palette, line_styles):
    dfl = df_filtered[df_filtered['l'] == pl]
    ax3.scatter(dfl['m_param'], dfl['for_echo_avg'], label=f'l={pl}', color=color, linestyle=line_style, alpha=0.7,
                zorder=3)
ax3.set_xlabel(r'growth rate $\overline{m}$', fontsize=font_size)
ax3.set_ylabel(r'avg total flood/echo messages $M_{f+e}$', fontsize=font_size)
ax3.set_yscale('log')
ax3.legend(fontsize=legend_font_size)
ax3.tick_params(axis='both', which='major', labelsize=font_size)
ax3.grid(True, zorder=1)  # Lower zorder for grid
plt.tight_layout()
plot_and_export(ax3, 'degree-vs-flood-echo')

# Plot 4: Average Sum of Cycle Lengths vs Average Total Trace Messages
fig, ax4 = plt.subplots(figsize=(7, 5))
for pl, color in zip([2, 3, 4], palette):
    dfl = df_filtered[df_filtered['l'] == pl]
    ax4.scatter(dfl['m_param'], dfl['pub_avg'], label=f'l={pl}', color=color, alpha=0.7, zorder=3)
ax4.set_xlabel(r'growth rate $\overline{m}$', fontsize=font_size)
ax4.set_ylabel(r'avg total trace messages $M_{t}$', fontsize=font_size)
ax4.set_yscale('log')
ax4.legend(fontsize=legend_font_size)
ax4.tick_params(axis='both', which='major', labelsize=font_size)
ax4.grid(True, zorder=1)  # Lower zorder for grid
plt.tight_layout()
plot_and_export(ax4, 'degree-vs-trace-messages')
